# include <iostream>
# include <string>
# include <fstream>
# include <sstream>
# include <vector>
# include <map>
# include <unistd.h>
# include <cstring>
# include <sys/epoll.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <fcntl.h>
# include <algorithm>

int get_socket(void)
{
    int socketfd;
    struct addrinfo hints, *res = NULL;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;        // IPv4もしくはIPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo("0.0.0.0", "4242", &hints, &res) != 0) {
        std::cerr << "getaddrinfo failed." << std::endl;
        return -1;
    }

    socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socketfd == -1) {
        std::cerr << "socket creation failed." << std::endl;
        freeaddrinfo(res);
        return -1;
    }

    int optval = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        std::cerr << "setsockopt failed." << std::endl;
        close(socketfd);
        freeaddrinfo(res);
        return -1;
    }

    if (res->ai_family == AF_INET6) {
        int no = 0;
        setsockopt(socketfd, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&no, sizeof(no));
    }

    if (bind(socketfd, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "bind failed." << std::endl;
        close(socketfd);
        freeaddrinfo(res);
        return -1;
    }

    if (listen(socketfd, 10) == -1) { // 10は待ち行列の最大数
        std::cerr << "listen failed." << std::endl;
        close(socketfd);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);
    return socketfd;
}

int main(void) {
    int server_fd = get_socket();
    if (server_fd < 0) {
        std::cerr << "Failed to get server socket." << std::endl;
        return 1;
    }

    while (true) {
        struct sockaddr_storage client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*) &client_addr, &addr_len);
        if (client_fd == -1) {
            std::cerr << "accept failed." << std::endl;
            continue;
        }

        char buffer[1024];
        int bytes_received;
        while ((bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
            // bufferにnull終端を追加
            buffer[bytes_received] = '\0';

            // 受信データをレスポンスとして作成
            std::string response_data = buffer;

            // HTTPヘッダーを含む完全なレスポンスを作成
            std::stringstream response;
            response << "HTTP/1.1 200 OK\r\n";
            response << "Content-Type: text/plain\r\n";
            response << "Content-Length: " << response_data.size() << "\r\n";
            response << "\r\n";
            response << response_data;

            // レスポンスをクライアントに送信
            send(client_fd, response.str().c_str(), response.str().size(), 0);
        }

        if (bytes_received == -1) {
            std::cerr << "recv failed." << std::endl;
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}