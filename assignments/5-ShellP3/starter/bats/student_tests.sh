#!/usr/bin/env bats

#############################################################
# Student test cases for dsh shell implementation
# Add as many test cases as needed
#############################################################

@test "Simple Command Execution" {
    run "./dsh" <<EOF
echo Hello, World!
EOF

    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello, World!"* ]]
}

@test "Built-in cd Command" {
    run "./dsh" <<EOF
cd ..
pwd
EOF

    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
    [[ "$output" != *"/current/directory/path"* ]] # Ensure it changed
}

@test "Redirection: Output to File" {
    run "./dsh" <<EOF
echo 'Test Output' > output.txt
cat output.txt
EOF

    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
    [[ "$output" == *"Test Output"* ]]
}

@test "Redirection: Append to File" {
    run "./dsh" <<EOF
echo 'First Line' > append.txt
echo 'Second Line' >> append.txt
cat append.txt
EOF

    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
    [[ "$output" == *"First Line"* ]]
    [[ "$output" == *"Second Line"* ]]
}

@test "Input Redirection" {
    run "./dsh" <<EOF
echo 'This is input' > input.txt
cat < input.txt
EOF

    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
    [[ "$output" == *"This is input"* ]]
}

@test "Multiple Pipes" {
    run "./dsh" <<EOF
ls | grep dsh | wc -l
EOF

    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
}

@test "Command Not Found" {
    run "./dsh" <<EOF
nonexistentcommand
EOF

    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -ne 0 ]  # Ensure it fails
    [[ "$output" == *"not found"* ]]
}

@test "Handling Too Many Pipes" {
    run dsh3 -c "echo test | cat | cat | cat | cat | cat | cat | cat | cat | cat"
    [ "$status" -eq 1 ]  # Expect exit code 1
    [[ "$output" == *"error: piping limited to 8 commands"* ]]  # Ensure the correct error message is displayed
}

@test "Exit Command" {
    run "./dsh" <<EOF
exit
EOF

    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
}

