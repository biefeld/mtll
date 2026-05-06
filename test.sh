#!/usr/bin/env bash
TOTAL=0
PASSED=0
BINARY="mtll"
VALGRIND=0

if [ "$1" == "--valgrind" ]; then
    VALGRIND=1
    RUNNER="valgrind --leak-check=full --error-exitcode=1 --track-origins=yes -s"
else
    RUNNER=""
fi


trap 'rm -f out.tmp' EXIT

echo -e "\n-=== Running tests ===-\n"

for test in tests/*; do
    TOTAL=$((TOTAL + 1))
    name=$(echo "$test" | cut -f 2 -d '/' | cut -f 2- -d '-')

    if [ $VALGRIND -eq 1 ]; then
        $RUNNER ./$BINARY < "$test/test.in" 1> out.tmp  # keep valgrind stderr visible
    else
        ./$BINARY < "$test/test.in" 1> out.tmp 2> /dev/null
    fi
    
    # check valgrind exit code
    if [ $? -ne 0 ] && [ $VALGRIND -eq 1 ]; then
        echo -e "\t❌ $name failed (memory error)."
        continue
    fi

    STDOUT_DIFF=$(diff "$test/test.out" out.tmp)
    if [ "$STDOUT_DIFF" ]; then
        echo -e "\t❌ $name failed."
        echo -e "STDOUT diff:\n $STDOUT_DIFF\n"
    else
        echo -e "\t✅ $name passed!"
        PASSED=$((PASSED + 1))
    fi
done

if [ $PASSED -eq $TOTAL ]; then
    printf "\n✅ "
else
    printf "\n❌ "
fi

echo "$PASSED/$TOTAL tests passed"
[ $PASSED -eq $TOTAL ]