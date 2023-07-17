#include <iostream>
#include <string>
#include <cstring>  // For std::strerror()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // For close()
#include <sys/event.h> // For kqueue() and kevent()
#include <errno.h> // For errno
#include <fcntl.h>  // For fcntl()

#define MAX_BUFFER_SIZE 1024
#define PORT 8080

bool setSocketNonBlocking(int sfd) {
    int flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "fcntl get failed" << std::endl;
        return false;
    }

    flags |= O_NONBLOCK;
    int s = fcntl(sfd, F_SETFL, flags);
    if (s == -1) {
        std::cerr << "fcntl set failed" << std::endl;
        return false;
    }

    return true;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "socket creation failed" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "bind failed" << std::endl;
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "listen failed" << std::endl;
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    if (!setSocketNonBlocking(server_fd)) {
        close(server_fd);
        return -1;
    }

    int kq = kqueue();
    if (kq == -1) {
        std::cerr << "kqueue creation failed" << std::endl;
        close(server_fd);
        return -1;
    }

    struct kevent evSet;
    EV_SET(&evSet, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1) {
        std::cerr << "kevent set failed" << std::endl;
        close(kq);
        close(server_fd);
        return -1;
    }

    while (true) {
        struct kevent evList[32];
        int nev = kevent(kq, NULL, 0, evList, 32, NULL);
        if (nev < 1) {
            std::cerr << "kevent wait failed" << std::endl;
            break;
        }

        for (int i = 0; i < nev; i++) {
            if (evList[i].ident == server_fd) {
                while (true) {
                    struct sockaddr in_addr;
                    socklen_t in_len = sizeof in_addr;

                    new_socket = accept(server_fd, &in_addr, &in_len);
                    if (new_socket == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break; // 已处理所有接続
                        } else {
                            std::cerr << "accept failed: " << std::strerror(errno) << std::endl;
                            break;
                        }
                    }

                    if (!setSocketNonBlocking(new_socket)) {
                        close(new_socket);
                        continue;
                    }

                    EV_SET(&evSet, new_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
                    if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1) {
                        std::cerr << "kevent set failed for new socket" << std::endl;
                        close(new_socket);
                        continue;
                    }
                }
            } else {
                while (true) {
                    ssize_t count = read(evList[i].ident, buffer, sizeof buffer);
                    if (count == -1) {
                        if (errno == EAGAIN) {
                            break;
                        }

                        std::cerr << "read error: " << std::strerror(errno) << std::endl;
                        break;
                    } else if (count == 0) {
                        break;
                    }

                    send(evList[i].ident, buffer, count, 0);
                }

                close(evList[i].ident);
            }
        }
    }

    close(kq);
    close(server_fd);
    return 0;
}