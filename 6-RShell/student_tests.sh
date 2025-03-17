#!/usr/bin/env bats

start_server() {
  ./dsh -s -i 127.0.0.1 -p 1458 &
  server_pid=$!  
  sleep 1  
}

stop_server() {
  kill $server_pid  
}

#TEST COMMANDS IN CLIENT AND SERVER (MAKE SURE THEY WORK)

@test "Example: check ls runs without errors" {
    start_server  

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
ls
EOF

    [ "$status" -eq 0 ]
    stop_server 
}

@test "Commands work" {
    start_server 

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
uname
EOF

    [ "$status" -eq 0 ]

    stop_server  
}

@test "Multiple commands in a row" {
    start_server

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
uname -a
pwd
EOF

    [ "$status" -eq 0 ]
    [ "${#output}" -gt 0 ]

   stop_server
}

@test "Remote cd command works and pwd is correct" {
    start_server

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
cd /tmp
pwd
EOF

    echo ""
    echo "pwd output: $output"
    echo ""

    actual_pwd_output=$(echo "$output" | grep -E "^dsh4> /tmp" | sed 's/^dsh4> //')

    echo "actual_pwd_output: $actual_pwd_output"

    [ "$status" -eq 0 ]  
    [ "$actual_pwd_output" = "/tmp" ] 

    stop_server
}

@test "quotes and normal words keep spaces on quotes" {
    start_server

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
echo cs is "    super   fun  "
EOF

    echo ""
    echo "Full output: $output"
    echo ""

    actual_output=$(echo "$output" | grep -E "^dsh4> cs is" | sed 's/^dsh4> //')

    echo "Extracted actual_output: '$actual_output'"

    [ "$status" -eq 0 ]  
    [ "$actual_output" = "cs is     super   fun  " ]  

    stop_server
}

@test "cd to no directory error" {
    start_server 

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
cd /nonexistent
EOF

    echo "cd error output: $output"

    [ "$status" -eq 0 ]  
    [[ "$output" == *"cd: No such directory"* ]] 

    stop_server  
}

@test "Pipe command" {
    start_server

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
echo "hello world" | wc -l
EOF

    echo "Pipe command test output: $output"

    [ "$status" -eq 0 ] 
    [[ "$output" == *"1"* ]]  

    stop_server
}

@test "Append redirection" {
    start_server  

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
echo "hello" > append.txt
echo "world" >> append.txt
cat append.txt
EOF

    echo "Append redirection test output: $output"

    [ "$status" -eq 0 ]  
    [[ "$output" == *"hello"* ]]
    [[ "$output" == *"world"* ]]

    stop_server 
}

@test "Output redirect" {
    start_server

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
echo "hello world" > out.txt
cat out.txt
EOF

    echo "Output redirect test output: $output"

    [ "$status" -eq 0 ]
    [[ "$output" == *"hello world"* ]]

    stop_server
}

@test "spaces" {
    start_server

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
echo "hello    world" > out.txt
cat out.txt
EOF

    echo "Spaces test output: $output"

    [ "$status" -eq 0 ]
    [[ "$output" == *"hello    world"* ]] 

    stop_server
}

@test "Input redirect" {
    start_server

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
echo "hello world" > input.txt
cat < input.txt
EOF

    echo "Input redirect test output: $output"

    [ "$status" -eq 0 ]
    [[ "$output" == *"hello world"* ]]

    stop_server
}


@test "Client connects to server and receives 'socket client mode: addr:127.0.0.1:1458'" {
  start_server  

  run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
EOF
  echo "$output" | grep -q "socket client mode:  addr:127.0.0.1:1458"
  [ $? -eq 0 ]  

  stop_server  
}

@test "test basic command works" {
  start_server

  run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
echo "Hello World!"
EOF

  echo "$output"  

  [[ "$output" == *"Hello World!"* ]]

  stop_server
}

@test "Client-server: stop-server terminates server" {
    ./dsh -s -i 127.0.0.1 -p 1458 > server_output.txt 2>&1 &
    server_pid=$!
    sleep 5

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
stop-server
EOF

    server_exit_status=$?

    echo "Server response:"
    cat server_output.txt
    
    [[ "$server_exit_status" -eq 0 ]]

    rm server_output.txt

}

@test "Client-server: exit command does not terminate server" {
    ./dsh -s -i 127.0.0.1 -p 1458 > server_output.txt 2>&1 &
    server_pid=$!
    sleep 5  

    run ./dsh -c -i 127.0.0.1 -p 1458 <<EOF
exit
EOF

    sleep 2

    if ps -p $server_pid > /dev/null; then
        echo "Server is still running."
        server_exit_status=0
    else
        echo "Server has stopped."
        server_exit_status=1
    fi

    echo "Server response:"
    cat server_output.txt

    [ "$server_exit_status" -eq 0 ] || fail "Server terminated unexpectedly after 'exit' command."

    rm server_output.txt

    stop_server
}












