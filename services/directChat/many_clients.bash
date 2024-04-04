#!/bin/bash

PORT=8041
ADDRESS="localhost"

function connect_to_service() {
    client_number="$1"

    # Calculate the duration of the connection
    start_time=$SECONDS
    response=$(nc "$ADDRESS" "$PORT" 2>&1)
    end_time=$SECONDS
    duration=$((end_time - start_time))

    # Print the result
    echo "$client_number: $duration seconds"
    echo "$response"
    sleep 1
}

num_processes="$1"

for ((i=1; i<=num_processes; i++)); do
    connect_to_service "$i" &
done

wait

echo "All processes completed successfully."
