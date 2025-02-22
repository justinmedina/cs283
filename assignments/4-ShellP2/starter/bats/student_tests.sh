#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

setup() {
    # start instance of the shell before each test
    test_shell="./dsh"
}

@test "Example: check ls runs without errors" {
    run $test_shell <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Built-in: exit command" {
    run $test_shell <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Built-in: cd without argument (should not change directory)" {
    original_dir=$(pwd)
    run $test_shell <<EOF
cd
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"$original_dir"* ]]
}

@test "Built-in: cd with valid directory" {
    mkdir -p test_dir
    run $test_shell <<EOF
cd test_dir
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"test_dir"* ]]
    rmdir test_dir
}

@test "Built-in: cd with invalid directory" {
    run $test_shell <<EOF
cd does_not_exist
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"cd failed"* ]]
}


@test "External: echo command" {
    run $test_shell <<EOF
echo Hello World
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello World"* ]]
}

@test "Handling empty input" {
    run $test_shell <<EOF

exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"warning: no commands provided"* ]]
}

@test "Handling multiple spaces between commands" {
    run $test_shell <<EOF
ls      -l
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Handling quoted arguments" {
    run $test_shell <<EOF
echo "Hello    World"
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello    World"* ]]
}

@test "Handling failed command execution" {
    run $test_shell <<EOF
invalidcommand
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Command not found in PATH"* ]]
}


@test "External: Running a background process" {
    run $test_shell <<EOF
sleep 1 &
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Command substitution" {
    run $test_shell <<EOF
echo \$(echo test)
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"test"* ]]
}

@test "rc: returns last command exit status error" {
    run $test_shell <<EOF
not_a_real_command
rc
exit
EOF
    [[ "$output" == *"2"* ]]  # 2 is "command not found"
}

@test "rc: returns last command exit status success" {
    run $test_shell <<EOF
ls
rc
exit
EOF
    [[ "$output" == *"0"* ]]  # 0 is "ok"
}
