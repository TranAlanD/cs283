
#!/usr/bin/env bats

@test "exit" {
    run ./dsh <<EOF
exit
EOF

    [ "$status" -eq 0 ]
}

@test "no input" {
    run ./dsh <<EOF

EOF

    [ "$status" -eq 0 ]
}

@test "quotes" {
    run ./dsh <<EOF
echo 'hello world' > output.txt
cat output.txt
EOF

    [ "$status" -eq 0 ]
    [[ "${output}" == *"hello world"* ]]
}

@test "Append redirection" {
    run ./dsh <<EOF
echo "hello" > append.txt
echo "world" >> append.txt
cat append.txt
EOF

    [ "$status" -eq 0 ]
    [[ "${output}" == *"hello"* ]]
    [[ "${output}" == *"world"* ]]
}

@test "Input redirect" {
    run ./dsh <<EOF
echo "hello world" > input.txt
cat < input.txt
EOF

    [ "$status" -eq 0 ]
    [[ "${output}" == *"hello world"* ]]
}

@test "Output redirect" {
    run ./dsh <<EOF
echo "hello world" > out.txt
cat out.txt
EOF

    [ "$status" -eq 0 ]
    [[ "${output}" == *"hello world"* ]]
}

@test "Pipe command" {
    run ./dsh <<EOF
echo "hello world" | wc -l"
EOF

    [ "$status" -eq 0 ]
    [[ "${output}" == *"1"* ]]
}

@test "spaces" {
    run ./dsh <<EOF
echo "hello    world" > out.txt
cat out.txt
EOF

    [ "$status" -eq 0 ]
    [[ "${output}" == *"hello    world"* ]]
}

@test "Multiple pipes" {
    run ./dsh <<EOF
echo "hello" | tr '[:lower:]' '[:upper:]' | rev
EOF

    [ "$status" -eq 0 ]
    [[ "${output}" == *"OLLEH"* ]]
}

@test "Invalid command" {
    run ./dsh <<EOF
invalid_command
EOF

    [ "$status" -eq 0 ]
    [[ "${output}" == *"execvp: No such file or directory"* ]]
}

