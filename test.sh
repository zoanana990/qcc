#!/bin/bash

declare -a tests=(
    "example/1 0"  
    "example/2 1" 
    "example/3 1"
    "example/4 1"
    "example/5 1"
    "example/6 0" 
)

total_tests=${#tests[@]}
pass_count=0
fail_count=0

for test_case in "${tests[@]}"; do
    cmd=$(echo "$test_case" | cut -d ' ' -f 1)
    expected_result=$(echo "$test_case" | cut -d ' ' -f 2)

    echo "> Test command: ./qcc $cmd"
    echo "----------------------------------------"

    ./qcc $cmd

    if [ $? -eq $expected_result ]; then
        echo "========================================"
        ((pass_count++))
    else
        echo "Unexpected behavior, Test failed"
        ((fail_count++))
    fi
    
done

pass_rate=$(echo "scale=2; $pass_count * 100 / $total_tests" | bc)

echo "#######################################"
echo "Total Tests: $total_tests"
echo "Passed: $pass_count"
echo "Failed: $fail_count"
echo "Pass Rate: $pass_rate%"

if [ $fail_count -gt 0 ]; then
    exit 1
else
    echo "All tests passed!"
    exit 0
fi
