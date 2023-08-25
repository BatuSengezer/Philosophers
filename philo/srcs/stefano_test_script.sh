#!/bin/bash

# Configurable variables
numbers_of_philosophers=5
time_to_die=800
time_to_eat=200
time_to_sleep=200
wait_seconds=20
max_attempts=10

attempt=1
while [[ $attempt -le $max_attempts ]]; do
    echo "Running attempt $attempt..."

    # Start the program and use tee to output both to terminal and to a file
    stdbuf -oL /Users/batu/Desktop/programming/42_core_curr/my_philosophers/philo/philo $numbers_of_philosophers $time_to_die $time_to_eat $time_to_sleep 2>&1 | tee philo_output.txt &
    philo_pid=$!

    # Wait for the specified time
    sleep $wait_seconds

    # Check if the word "died" is present in the output
    if grep -q "died" philo_output.txt; then
        echo "A philosopher died during attempt $attempt."
        exit 1
    fi

    # If the process is still running after wait_seconds
    if ps -p $philo_pid > /dev/null; then
        # Kill the process if it's still running
        kill $philo_pid
        wait $philo_pid 2>/dev/null

        # Increase attempt count
        ((attempt++))
    fi
done

echo "We were successful!"