#!/bin/bash

# Send a GET request to the server and capture the HTTP status code
status_code=$(curl -o /dev/null -s -w "%{http_code}" http://localhost:8080/)

# Check if the status code is 200 (OK)
if [ "$status_code" -eq 200 ]; then
    echo "Test passed: Server returned status code 200"
else
    echo "Test failed: Server returned status code $status_code"
fi
