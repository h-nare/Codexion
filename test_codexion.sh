
#!/bin/bash

pass=0
fail=0

run_test()
{
    name="$1"
    coders="$2"
    burnout="$3"
    compile="$4"
    debug="$5"
    refactor="$6"
    required="$7"
    cooldown="$8"
    scheduler="$9"
    expected="${10}"

    output=$(./codexion "$coders" "$burnout" "$compile" \
        "$debug" "$refactor" "$required" "$cooldown" "$scheduler")

    count=$(printf '%s\n' "$output" | grep -c "is compiling")
    deaths=$(printf '%s\n' "$output" | grep -c "burned out")
    dongles=$(printf '%s\n' "$output" | grep -c "has taken a dongle")

    if [ "$count" -eq "$expected" ] && \
       [ "$deaths" -eq 0 ] && \
       [ "$dongles" -eq $((count * 2)) ]; then
        echo "PASS: $name"
        pass=$((pass + 1))
    else
        echo "FAIL: $name (compiles=$count, burnouts=$deaths, dongles=$dongles)"
        fail=$((fail + 1))
    fi
}

run_test "2 coders FIFO" 2 1000 200 200 100 3 50 fifo 6
run_test "2 coders EDF" 2 1000 200 200 100 3 50 edf 6
run_test "3 coders FIFO" 3 1000 200 200 100 3 50 fifo 9
run_test "3 coders EDF" 3 1000 200 200 100 3 50 edf 9
run_test "5 coders FIFO" 5 1000 200 200 100 3 50 fifo 15
run_test "5 coders EDF" 5 1000 200 200 100 3 50 edf 15
run_test "7 coders EDF" 7 1500 200 200 100 3 50 edf 21
run_test "10 coders FIFO" 10 2000 200 200 100 3 50 fifo 30
run_test "10 coders EDF" 10 2000 200 200 100 3 50 edf 30
run_test "Zero cooldown FIFO" 5 1000 200 200 100 3 0 fifo 15
run_test "Zero cooldown EDF" 5 1000 200 200 100 3 0 edf 15

echo "----------------------"
echo "PASSED: $pass"
echo "FAILED: $fail"

