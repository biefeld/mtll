CC=gcc
CFLAGS=-Wall -Werror -pedantic -Wvla -Wuninitialized -Wno-unused-result -std=c2x -g
LDFLAGS=-lm -lpthread
INCLUDE=-Iinclude
TARGET=mtll
TEST_SCRIPT=test.sh

SRCS=src/main.c src/commands.c src/input_parser.c src/mtll.c src/node.c
HDRS=include/commands.h include/input_parser.h include/mtll.h include/node.h include/structures.h
OBJS=$(SRCS:.c=.o)

build: all
	rm -fr mtll.dSYM

all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(SRCS) $(INCLUDE) $(CFLAGS) $(LDFLAGS) -o $@

.PHONY: test
test: build
	chmod u+x ${TEST_SCRIPT}
	./${TEST_SCRIPT}

.PHONY: clean
clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f $(TARGET)-*.gcda $(TARGET)-*.gcno *.gcov
	rm -fr coverage/

.PHONY: valgrind
valgrind: build
	chmod u+x ${TEST_SCRIPT}
	./${TEST_SCRIPT} --valgrind

.PHONY: debug
debug: $(TARGET)
	valgrind --leak-check=full -s --track-origins=yes --show-leak-kinds=all --error-exitcode=42 --exit-on-first-error=yes --errors-for-leak-kinds=all --track-fds=yes ./$(TARGET)

.PHONY: coverage
coverage:
	$(CC) $(SRCS) $(INCLUDE) $(CFLAGS) $(LDFLAGS) --coverage -o $(TARGET)
	chmod u+x ${TEST_SCRIPT}
	./${TEST_SCRIPT}
	mkdir -p coverage/
	gcov -o . $(TARGET)-*.gcno
	mv *.gcov coverage/
	@echo "Coverage report saved to coverage/"