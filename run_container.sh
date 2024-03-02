#!/bin/bash

image_name='webserv_webserv'

# Run your docker command and capture the output
output=$(docker ps)

# Extract the container ID and name from the output
container_id=$(echo "$output" | awk 'NR==2 {print $1}')
container_name=$(echo "$output" | awk 'NR==2 {print $2}')

if [[ $container_name == $image_name ]]; then
	make fclean
	docker compose down
	echo 'Container is down.'
else
	docker compose up -d
	sleep 3s
	echo 'Container starts up.'
	output=$(docker ps)
	container_id=$(echo "$output" | awk 'NR==2 {print $1}')
	container_name=$(echo "$output" | awk 'NR==2 {print $2}')
fi

# Output the results
echo "Container ID: $container_id"
echo "Container Name: $container_name"
