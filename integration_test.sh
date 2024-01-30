#!/bin/bash

echo -e "
====================================================================
 _    _  _____ _                         ______       _            
| |  | ||____ | |                       |___  /      | |           
| |  | |    / / |__  ___  ___ _ ____   __  / /__  ___| |_ ___ _ __ 
| |/\| |    \ \ '_ \/ __|/ _ \ '__\ \ / / / / _ \/ __| __/ _ \ '__|
\  /\  /.___/ / |_) \__ \  __/ |   \ V /./ /  __/\__ \ ||  __/ |   
 \/  \/ \____/|_.__/|___/\___|_|    \_/ \_/ \___||___/\__\___|_|   

====================================================================
"

webserv_client() {
    host="$1"
    port="$2"
    lines="$3"

    res=$(telnet $host $port <<EOF\n$lines\nEOF\n)
	echo "$res"
}

GREEN="\u001b[32m"
RESET="\033[0m"

assert_response() {
    host="$1"
    port="$2"
    lines="$3"
	exp_status_code="$4"
	exp_headers="$5"
	exp_body="$6"

	# Provided output
	output=$(webserv_client $host $port $lines)

	http_response=$(echo "$output" | awk '/HTTP\/1.1/{flag=1; next} flag')

	# Extract HTTP status code
	status_code=$(echo "$http_response" | awk 'NR==1 {print $2}')

	# Extract headers as an array
	headers=$(echo "$http_response" | awk '/^[A-Za-z-]*:/ {print $1}' | tr '\n' ' ')
	IFS=' ' read -ra headers_array <<< "$headers"

	# Extract body
	body=$(echo "$http_response" | sed -n '/^<HTML>/,$p')

	# Print the results
	echo "HTTP Status Code: $status_code"
	echo "Headers:"
	for header in "${headers_array[@]}"; do
	echo "  $header"
	done
	echo "Body:"
	echo "$body"

	# Assert the results and expecexpectations
	if [[ "$status_code" == "$exp_status_code" ]]; then
		echo "$GREEN" "Status OK" "$RESET"
	else
		echo "${RED}Status KO${RESET}"
	fi
	for header in "${headers_array[@]}"; do
		is_find = 0
		for exp_header in "${exp_headers[@]}"; do
			if [ "$header" = "$exp_header" ]; then
				is_find = 1
				break
			fi
		done
		if [ is_find -eq 0 ]; then
			echo "$RED" "Header KO" "$RESET"
			break
		fi
	done
	if [[ is_find -eq 1 ]]; then
		echo "$GREEN" "Header OK" "$RESET"
	fi
	if [[ "$body" == "$exp_body" ]]; then
		echo "$GREEN" "Body OK" "$RESET"
	else
		echo "$RED" "Body KO" "$RESET"
	fi
}

declare -a headers=("Location: http://www.google.com/" "Content-Type: text/html; charset=UTF-8" "Content-Security-Policy-Report-Only: object-src 'none';base-uri 'self';script-src 'nonce-TTIGkYYoZFPRiSm1mrk7eg' 'strict-dynamic' 'report-sample' 'unsafe-eval' 'unsafe-inline' https: http:;report-uri https://csp.withgoogle.com/csp/gws/other-hp")

assert_response "google.com" "80" "GET / HTTP/1.1\nHost: google.com\n\n" "301" "$headers" "aaaaaaa"
