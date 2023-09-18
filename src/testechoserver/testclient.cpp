#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // ソケットの作成
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // サーバーアドレスの設定
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(4242);  // 4242ポートに接続します。変更が必要な場合はここを変更してください。
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    // サーバーに接続
    if (connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Failed to connect to server." << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << "Connected to server. Enter your message:" << std::endl;

    std::string message;
    while (true) {
        std::getline(std::cin, message);

        if (message == "exit") {
            std::cout << "Exiting..." << std::endl;
            break;
        }

        // サーバーにメッセージを送信
        send(sockfd, message.c_str(), message.size(), 0);

        char buffer[1024];
        int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cerr << "Failed to receive message or server closed the connection." << std::endl;
            break;
        }

        buffer[bytes_received] = '\0';  // Null terminate the received string
        std::cout << "Received from server: " << buffer << std::endl;
    }

    close(sockfd);
    return 0;
}

