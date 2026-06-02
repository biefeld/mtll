import subprocess
import threading
import os
import time
import queue
import json
import uuid
import logging

logging.basicConfig(level=logging.DEBUG, format='%(asctime)s %(levelname)s %(message)s')
log = logging.getLogger(__name__)

# Monkey-patch BEFORE importing flask/flask-sock so gevent works with blocking I/O
from gevent import monkey
monkey.patch_all()

from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
from flask_sock import Sock

app = Flask(__name__, static_folder="static")
CORS(app)
sock = Sock(app)

# Resolve binary: env var > ./mtll > ../mtll (one dir up, common layout)
def find_binary():
    candidates = [
        os.environ.get("MTLL_BINARY", ""),
        "./mtll",
        "../mtll",
        os.path.join(os.path.dirname(__file__), "mtll"),
        os.path.join(os.path.dirname(__file__), "..", "mtll"),
    ]
    for c in candidates:
        if c and os.path.isfile(c) and os.access(c, os.X_OK):
            log.info(f"Using binary: {c}")
            return os.path.abspath(c)
    return None

BINARY_PATH = find_binary()
if not BINARY_PATH:
    log.warning("MTLL binary not found! Set MTLL_BINARY env var or place ./mtll next to server.py")


class MTLLSession:
    def __init__(self, sid):
        self.sid = sid
        self.q = queue.Queue()
        self.lock = threading.Lock()
        self.last_used = time.time()
        self.proc = None
        self._spawn()

    def _spawn(self):
        if not BINARY_PATH:
            log.error("Cannot spawn: no binary")
            return
        log.debug(f"[{self.sid[:8]}] spawning {BINARY_PATH}")
        self.proc = subprocess.Popen(
            [BINARY_PATH],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,   # separate so stderr doesn't pollute stdout
            text=True,
            bufsize=1,
        )
        threading.Thread(target=self._read_stdout, daemon=True).start()
        threading.Thread(target=self._read_stderr, daemon=True).start()

    def _read_stdout(self):
        try:
            for line in self.proc.stdout:
                stripped = line.rstrip('\n')
                log.debug(f"[{self.sid[:8]}] stdout: {stripped!r}")
                self.q.put(stripped)
        except Exception as e:
            log.error(f"stdout reader error: {e}")
        finally:
            self.q.put(None)

    def _read_stderr(self):
        try:
            for line in self.proc.stderr:
                log.warning(f"[{self.sid[:8]}] stderr: {line.rstrip()!r}")
        except Exception:
            pass

    def send_lines(self, lines):
        self.last_used = time.time()
        if not self.proc:
            return
        if self.proc.poll() is not None:
            log.warning(f"[{self.sid[:8]}] process died (rc={self.proc.returncode}), respawning")
            self._spawn()
        for line in lines:
            log.debug(f"[{self.sid[:8]}] stdin: {line!r}")
            try:
                self.proc.stdin.write(line + '\n')
            except BrokenPipeError:
                log.error("Broken pipe on stdin write")
                return
        self.proc.stdin.flush()

    def collect(self, timeout=0.5):
        lines = []
        deadline = time.time() + timeout
        while time.time() < deadline:
            remaining = max(0.01, deadline - time.time())
            try:
                item = self.q.get(timeout=remaining)
                if item is None:
                    break
                lines.append(item)
                # reset deadline after each line — burst reading
                deadline = time.time() + 0.1
            except queue.Empty:
                break
        return lines

    def kill(self):
        if self.proc and self.proc.poll() is None:
            self.proc.terminate()


sessions: dict[str, MTLLSession] = {}
sessions_lock = threading.Lock()

def get_session(sid):
    with sessions_lock:
        if sid not in sessions:
            sessions[sid] = MTLLSession(sid)
        return sessions[sid]

def _cleanup_loop():
    while True:
        time.sleep(60)
        now = time.time()
        with sessions_lock:
            dead = [k for k, s in sessions.items() if now - s.last_used > 300]
            for k in dead:
                sessions[k].kill()
                del sessions[k]
                log.info(f"Cleaned up session {k[:8]}")

threading.Thread(target=_cleanup_loop, daemon=True).start()


@app.route('/')
def index():
    return send_from_directory('static', 'index.html')

@app.route('/health')
def health():
    return jsonify({
        'status': 'ok',
        'binary': BINARY_PATH,
        'binary_found': bool(BINARY_PATH),
        'sessions': len(sessions),
    })

@sock.route('/ws/<sid>')
def ws_handler(ws, sid):
    session = get_session(sid)
    log.info(f"WS connected: {sid[:8]}")

    if not BINARY_PATH:
        ws.send(json.dumps({'output': ['ERROR: MTLL binary not found. Set MTLL_BINARY env var.', f'Tried: ./mtll, ../mtll, $MTLL_BINARY']}))

    try:
        while True:
            raw = ws.receive()
            if raw is None:
                break
            try:
                msg = json.loads(raw)
            except json.JSONDecodeError:
                log.warning(f"Bad JSON from client: {raw!r}")
                continue

            lines = msg.get('lines', [])
            if not lines:
                continue

            session.send_lines(lines)
            output = session.collect(timeout=0.5)

            log.debug(f"[{sid[:8]}] sending back {len(output)} lines")
            ws.send(json.dumps({'output': output}))

    except Exception as e:
        log.error(f"WS error for {sid[:8]}: {e}")


if __name__ == '__main__':
    port = int(os.environ.get('PORT', 5000))
    log.info(f"Starting on port {port}")
    log.info(f"Binary: {BINARY_PATH}")
    app.run(host='0.0.0.0', port=port, debug=False)