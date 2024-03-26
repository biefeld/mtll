CC=gcc
FLAGS=-g -O2 -Wall -Werror -pedantic -Wno-unused-result -std=c99
OBJS=mtll.o commands.o input_parser.o node.o main.o
TARGET=mtll
TEST_SCRIPT=test.sh

build: all

all: $(TARGET)
	@ # requires TARGET rule, which we can change in the constant up the top

$(TARGET): $(OBJS)
	@ # requires OBJS rule for all objest within the variable, which again we can change.
	@ # $@ is the target name, in this case TARGET (useful as this will be what the binary is called)
	@ # $^ outputs all requirements/prerequisites, in this case all objects within OBJS (useful as those are our object files which have not been linked yet)
	$(CC) $(FLAGS) $^ -o $@

%.o: %.c
	@ # the -c flag tells us to make object (.o) files and to not link to a binary yet
	@ # as we are only working with one .c to one .o file at a time (given by the % wildcard), we ise $< which is only the first requirement/prerequisitez
	$(CC) -c $(FLAGS) $< -o $@

build_tests: $(TARGET)
	@ echo "Make my tests!"

.PHONY:
test: build_tests
	chmod u+x ${TEST_SCRIPT}
	./${TEST_SCRIPT}

.PHONY:
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

debug: $(TARGET)
	valgrind --leak-check=full -s --track-origins=yes --show-leak-kinds=all --error-exitcode=42 --exit-on-first-error=yes --errors-for-leak-kinds=all --track-fds=yes ./$(TARGET)	