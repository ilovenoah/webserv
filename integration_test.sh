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

# 色の設定
GREEN="\033[0;32m"
RED="\033[0;31m"
RESET="\033[0m"

# HTTPレスポンスをテストする関数
assert_response() {
    url="$1"
    expected_status="$2"
    declare -a expected_headers=("${!3}")
    expected_body="$4"

    # curlを使用してHTTPレスポンスを取得
    response=$(curl -s -i $url)

    # HTTPステータスコードを取得
    status=$(echo "$response" | grep HTTP/ | awk '{print $2}')

    # レスポンスヘッダーを取得
    headers=$(echo "$response" | grep -o '^[^ ]*:' | tr '\n' ' ')
    IFS=' ' read -ra headers_array <<< "$headers"

    # レスポンスボディを取得
    body=$(echo "$response" | sed -n '/^\r$/,$p')

    # 結果の表示
    echo "HTTP Status Code: $status"
    echo "Headers:"
    for header in "${headers_array[@]}"; do
        echo "  $header"
    done
    echo "Body:"
    echo "$body"

    # 結果の検証
    if [[ "$status" == "$expected_status" ]]; then
        echo -e "${GREEN}Status OK${RESET}"
    else
        echo -e "${RED}Status KO${RESET}"
    fi

    header_found=0
    for expected_header in "${expected_headers[@]}"; do
        if echo "${headers_array[@]}" | grep -q "$expected_header"; then
            header_found=$((header_found+1))
        fi
    done

    if [[ $header_found -eq ${#expected_headers[@]} ]]; then
        echo -e "${GREEN}Headers OK${RESET}"
    else
        echo -e "${RED}Headers KO${RESET}"
    fi

    if [[ "$body" == *"$expected_body"* ]]; then
        echo -e "${GREEN}Body OK${RESET}"
    else
        echo -e "${RED}Body KO${RESET}"
    fi
}

# 期待されるヘッダーの配列を定義
declare -a expected_headers=("Location:" "Content-Type:" "Content-Security-Policy-Report-Only:")

# 関数を呼び出してテストを実行
assert_response "http://google.com" "301" expected_headers[@] "aaaaaaa"
