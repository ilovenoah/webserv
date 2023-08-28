# ベースとなるDockerイメージを指定
FROM ubuntu:latest

# 必要なパッケージをインストール
RUN apt update && apt install -y git && apt install -y build-essential

# SSHクライアントの設定
RUN mkdir -p -m 0700 ~/.ssh && ssh-keyscan github.com >> ~/.ssh/known_hosts

WORKDIR /home
