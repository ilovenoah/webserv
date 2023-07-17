# ベースとなるDockerイメージを指定
FROM ubuntu:latest

# 必要なパッケージをインストール
RUN apt update && apt install -y git && apt install -y build-essential