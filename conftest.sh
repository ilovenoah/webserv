#!/bin/bash

# 関数の定義
check_webserv_status() {
    # 引数として与えられた設定ファイルのパスを変数に格納
    config_file="$1"

    echo
    echo "file path: $1"

    # webservを設定ファイルとともに起動し、終了ステータスを取得
    ./webserv "$config_file"
    status=$?

    # 終了ステータスが1かどうかをチェック
    if [ $status -eq 1 ]; then
        echo "終了ステータスは1です。"
        return 0
    else
        echo "終了ステータスは1ではありません。"
        return 1
    fi
    echo
}

conf_files=$(find ./conf/error -type f -name '*.conf')
for conf_file in $conf_files; do
    check_webserv_status $conf_file
done
