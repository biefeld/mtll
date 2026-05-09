# Multi-Typed Linked List (MTLL)

![Tests](https://github.com/biefeld/mtll/actions/workflows/tests.yml/badge.svg)

A C implementation of a **multi-typed linked list** (MTLL): a linked list capable of holding heterogeneous values (integers, floats, chars, strings, and references to other lists) within a single structure. Supports nested lists, dynamic insertion and deletion, and a fully interactive CLI for exploring the data structure without writing code.

---

## Highlights

- **Heterogeneous typing**: each node independently stores one of five types: `int`, `float`, `char`, `string`, or a reference to another list
- **Nested lists**: lists can reference other lists, with circular reference prevention enforced at runtime
- **Dynamic memory management**: all allocation and deallocation handled manually with `malloc`/`calloc`/`realloc`/`free` (zero  memory leaks verified with Valgrind)
- **Interactive CLI**: inspect and manipulate live data structures without writing a single line of C
- **Negative index support**: INSERT and DELETE accept negative indices for tail-relative addressing
- **CI pipeline**: automated build, test, Valgrind, and coverage checks on every push via GitHub Actions

---

## Project Structure

```
.
├── src/
│   ├── main.c            # Entry point and CLI loop
│   ├── mtll.c            # Core MTLL operations
│   ├── node.c            # Node creation and management
│   ├── commands.c        # Command dispatch
│   └── input_parser.c    # Input parsing and validation
├── include/
│   ├── mtll.h            # MTLL interface (documented)
│   ├── node.h
│   ├── commands.h
│   ├── input_parser.h
│   └── structures.h      # Shared type definitions
├── tests/                # Test cases (input/expected output pairs)
├── test.sh               # Test runner
└── Makefile
```

---

## Build & Run

Requires GCC and GNU Make.

```bash
make        # build
./mtll      # launch CLI
```

```bash
make test        # run test suite
make valgrind    # run tests under Valgrind (Linux)
make coverage    # build with --coverage and generate gcov report
make clean       # remove build artifacts
```

---

## CLI Usage

The CLI reads commands from stdin. Each command operates on the live set of MTLL structures in memory.

### `NEW <length>`

Creates and populates a new MTLL. Values are read line-by-line from stdin after the command.

```
NEW 4
1.0

baguette
5
List 0: 1.00 -> -> baguette -> 5
```

Lists can reference other lists using `{id}`:

```
NEW 3
first
{0}
last
Nested 1: first -> {List 0} -> last
```

---

### `VIEW <id> | ALL`

Print the contents of a list, or a summary of all lists.

```
VIEW 0
hello -> 1 -> 2 -> 3.14 -> a
```

```
VIEW ALL
Number of lists: 3
List 0
List 1
Nested 2
```

---

### `TYPE <id>`

Print the type of each element in a list.

```
TYPE 0
string -> int -> int -> float -> char
```

---

### `INSERT <id> <index> <value>`

Insert a value at a given index. Supports negative indices (tail-relative).

```
VIEW 0
a -> b -> c -> d
INSERT 0 0 start
List 0: start -> a -> b -> c -> d
INSERT 0 -1 end
List 0: start -> a -> b -> c -> d -> end
```

References can be inserted dynamically:

```
INSERT 1 1 {0}
Nested 1: first -> {List 0} -> last
```

---

### `DELETE <id> <index>`

Delete the element at a given index. Supports negative indices.

```
VIEW 0
a -> b -> c -> d
DELETE 0 0
List 0: b -> c -> d
DELETE 0 -1
List 0: b -> c
```

---

### `REMOVE <id>`

Remove an entire list. Removal is blocked if the list is currently referenced by another.

```
REMOVE 0
List 0 has been removed.

Number of lists: 1
List 1
```

```
REMOVE 0
INVALID COMMAND: REMOVE
```

---

### `VIEW-NESTED <id>`

Print a list with nested list contents expanded inline.

```
VIEW-NESTED 2
first -> {a -> b -> c -> d} -> last
```

---

## Testing

The test suite lives in `tests/`, with each case as a directory containing `test.in` and `test.out`. The test driver `test.sh` diffs actual output against expected, and fails on missing files.

```bash
make test
```

```
-=== Running tests ===-

    ✅ boundary_delete_single_element passed!
    ✅ boundary_index_out_of_range passed!
    ...
    ✅ type_reference_node passed!

✅ 37/37 tests passed
```

Cases cover positive paths, invalid commands, boundary conditions, EOF handling, nesting, stress inputs, and Valgrind-clean memory behaviour.

---

## Implementation Notes

**Why a linked list over a dynamic array?**
Node-level heterogeneous typing requires each node to manage its own allocation size, which fits naturally with a linked list. A dynamic array would require a uniform element size or an extra indirection layer, adding complexity without benefit for this use case.

**Memory model**
Every node stores its value via `void*` with a separately allocated `enum TYPE*` tag. On insertion, the value is `realloc`'d to exactly the size required for its type. On deletion or list removal, all node memory is freed bottom-up. There are no global allocations.

**Reference safety**
Each list tracks `num_references` (how many lists point to it) and `num_nested` (how many reference nodes it contains). INSERT validates that a reference target exists, is not itself nested, and does not produce a self-reference. REMOVE is blocked while `num_references > 0`.

---

## API

The MTLL interface is documented in [`include/mtll.h`](include/mtll.h). Key functions:

| Function | Description |
|---|---|
| `mtll_create` | Allocate and populate a new MTLL |
| `mtll_append` | Append an MTLL to the active list |
| `mtll_insert` | Insert a node at a given index |
| `mtll_delete` | Delete a node at a given index |
| `mtll_remove` | Remove an MTLL, updating reference counts |
| `mtll_view` | Print list values |
| `mtll_view_all` | Print index and nesting status of all lists |
| `mtll_view_nested` | Print with nested list contents expanded |
| `mtll_type` | Print the type of each node |
| `mtll_free_all` | Free all allocated memory |