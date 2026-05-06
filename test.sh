#!/usr/bin/env bash
TOTAL=0
PASSED=0
BINARY="mtll"

trap 'rm -f out.tmp' EXIT

echo -e "\n-=== Running tests ===-\n"

for test in tests/*; do
    TOTAL=$((TOTAL + 1))
    name=$(echo "$test" | cut -f 2 -d '/' | cut -f 2- -d '-')

    ./$BINARY < "$test/test.in" 1> out.tmp 2> /dev/null
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