#!/usr/bin/env bats

setup() {
    # Start the server in the background
    ./dsh -s -i 127.0.0.1 -p 9094 &
    sleep 1 # Give the server time to start
}

teardown() {
    # Stop the server after tests
    echo "stop-server" | ./dsh -c -i 127.0.0.1 -p 9094
    sleep 1
}

@test "Check ls command" {
    run bash -c "echo 'ls' | ./dsh -c -i 127.0.0.1 -p 9094"
    [[ "$output" =~ "dsh" ]]  # Check if "dsh" binary appears in the output
}

@test "Check pwd command" {
    run bash -c "echo 'pwd' | ./dsh -c -i 127.0.0.1 -p 9094"
    [[ "$output" =~ "/home" ]]  # Assuming running from home directory
}

@test "Change directory and verify" {
    run bash -c "echo 'cd ..' | ./dsh -c -i 127.0.0.1 -p 9094"
    [[ "$output" =~ "Changed directory" ]]
}

@test "Invalid command handling" {
    run bash -c "echo 'invalidcommand' | ./dsh -c -i 127.0.0.1 -p 9094"
    [[ "$output" =~ "execvp failed" ]]
}

@test "Pipe commands" {
    run bash -c "echo 'ls | grep .c' | ./dsh -c -i 127.0.0.1 -p 9094"
    [[ "$output" =~ "rsh_cli.c" ]]
}


@test "Exit command" {
    run bash -c "echo 'exit' | ./dsh -c -i 127.0.0.1 -p 9094"
    [[ "$status" -eq 0 ]]
}

@test "Verify server stops on stop-server command" {
    run bash -c "echo 'stop-server' | ./dsh -c -i 127.0.0.1 -p 9094"
    [[ "$output" =~ "cmd loop returned 0" ]]
}

@test "Ensure multiple clients can connect sequentially" {
    run bash -c "echo 'ls' | ./dsh -c -i 127.0.0.1 -p 9094"
    [[ "$output" =~ "dsh" ]]
    run bash -c "echo 'ls' | ./dsh -c -i 127.0.0.1 -p 9094"
    [[ "$output" =~ "dsh" ]]
}

@test "Test handling of long commands" {
    run bash -c "echo 'echo This is a very long test command to check buffer handling' | ./dsh -c -i 127.0.0.1 -p 9094"
    [[ "$output" =~ "This is a very long test command to check buffer handling" ]]
}


