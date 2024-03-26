#!/bin/bash

declare -a tests=(
    "example/1 0"  
    "example/2 0" 
    "example/3 1"
    "example/4 1"
    "example/5 1" 
)

# 遍歷測試
for test_case in "${tests[@]}"; do
    # 分離命令和預期結果
    cmd=$(echo "$test_case" | cut -d ' ' -f 1)
    expected_result=$(echo "$test_case" | cut -d ' ' -f 2)

    echo "> Test command: ./qcc $cmd"
    echo "----------------------------------------"

    # 執行測試命令
    ./qcc $cmd

    # 檢查命令的退出狀態
    if [ $? -eq $expected_result ]; then
        echo "========================================"
    else
        echo "Unexpected behavior"
        exit 1
    fi
    
done

echo "Test Complete"
