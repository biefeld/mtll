CC=gcc
CFLAGS=-Wall -Werror -pedantic -Wvla -Wuninitialized -Wno-unused-result -std=c2x -g
LDFLAGS=-lm -lpthread
INCLUDE=-Iinclude
TARGET=mtll
TEST_SCRIPT=test.sh

build: all
	rm -fr mtll.dSYM

all: $(TARGET)
	@ # requires TARGET rule, which we can change in the constant up the top

$(TARGET): src/main.c src/commands.c src/input_parser.c src/mtll.c src/node.c
	@ $(CC) $^ $(INCLUDE) $(CFLAGS) $(LDFLAGS) -o $@


.PHONY:
test: build
	chmod u+x ${TEST_SCRIPT}
	./${TEST_SCRIPT}

.PHONY:
clean:
	rm -f $(TARGET)

debug: $(TARGET)
	valgrind --leak-check=full -s --track-origins=yes --show-leak-kinds=all --error-exitcode=42 --exit-on-first-error=yes --errors-for-leak-kinds=all --track-fds=yes ./$(TARGET)	