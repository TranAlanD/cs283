#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "command" {
    run ./dsh <<EOF

uname
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "cd command works and pwd is correct" {
    run ./dsh <<EOF
cd /tmp
pwd
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "$(echo "$output" | head -n 1)" = "/tmp" ]
}

@test "Command works if you have multiple spaces" {
    run ./dsh <<EOF
echo      multiple spaces 
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "$(echo "$output" | head -n 1)" = "multiple spaces" ]
}

@test "Command keeps spaces if quoted" {
    run ./dsh <<EOF
echo "    CS283,    is    fun // :)      "
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "$(echo "$output" | head -n 1)" = "    CS283,    is    fun // :)      " ]
}

@test "Invalid command" {
    run ./dsh <<EOF
invalidcmd
EOF

    # Assertions
    [ "$status" -eq 0 ]  
    [ "$(echo "$output" | head -n 1)" = "Command execution error: No such file or directory" ]

}

@test "Multiple commands in a row" {
    run ./dsh <<EOF
uname -a
pwd
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${#output}" -gt 0 ]
}

@test "quotes and normal words keep spaces on quotes" {
    run ./dsh <<EOF
echo cs is "    super   fun  "
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "$(echo "$output" | head -n 1)" = "cs is     super   fun  " ]
}

@test "cd to no directory error" {
    run ./dsh <<EOF
cd /nonexistent
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" == *"cd: No such file or directory"* ]]
}

@test "exit and echo work" {
    run ./dsh <<EOF
echo "Hello, World!"
EOF

    cleaned_output=$(echo "$output" | sed -n '1p')

    # Assertions
    [ "$cleaned_output" = "Hello, World!" ]
}

@test "Built-in exit command works" {
    run ./dsh <<EOF
exit
EOF
    echo "Debug: Actual output: [$output]"
}

