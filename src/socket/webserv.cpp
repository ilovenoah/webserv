#include "webserv.hpp"
#define MAXLEN 10
class servverConfig
{
  private:
	const char *port;
	const char *host;

  public:
	const char *getPort() const
	{
		return port;
	}
	const char *getHost() const
	{
		return host;
	}
};

int setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

std::vector<int> webserver(const std::vector<servverConfig> &confvec)
{
	std::vector<int> sockets;
	struct addrinfo hints, *res;
	int sockfd;

	for (size_t i = 0; i < confvec.size(); i++)
	{
		std::memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;	 // IPv4もしくはIPv6
		hints.ai_socktype = SOCK_STREAM; // TCP
		hints.ai_flags = AI_PASSIVE;	 // サーバーソケット用
		if (getaddrinfo(confvec[i].getHost(), confvec[i].getPort(), &hints, &res) != 0)
		{
			std::cerr << "getaddrinfo failed." << std::endl;
			continue; // toerrorhondol
		}
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd == -1)
		{
			std::cerr << "socket creation failed." << std::endl;
			freeaddrinfo(res);
			continue; // toerrorhondol
		}
		if (setNonBlocking(sockfd) == -1)
		{
			std::cerr << "Setting non-blocking failed." << std::endl;
			close(sockfd); // 不要なソケットを閉じる
			freeaddrinfo(res);
			continue;
		}
		int optval = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		{
			std::cerr << "setsockopt failed." << std::endl;
			close(sockfd);
			freeaddrinfo(res);
			continue;
		}
		if (res->ai_family == AF_INET6)
		{
			int no = 0;
			setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no));
		}
		if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
		{
			std::cerr << "Bind failed for " << confvec[i].getHost() << ":" << confvec[i].getPort()
					  << ". Error: " << strerror(errno) << std::endl;
			close(sockfd);	   // ソケットを閉じる
			freeaddrinfo(res); // メモリを解放
			continue;
		}
		if (listen(sockfd, MAXLEN) == -1)
		{ // 10は待ち行列の最大数
			std::cerr << "listen failed." << std::endl;
			continue; // toerrorhondol
		}
		freeaddrinfo(res);
		sockets.push_back(sockfd);
	}
	return sockets;
}

int main(void)
{
	std::vector<int> sockets = webserver(confvec);

	int epollfd = epoll_create1(0);
	if (epollfd == -1)
	{
		std::cerr << "epoll_create1 failed." << std::endl;
		return 1;
	}

	for (int sockfd : sockets)
	{
		struct epoll_event event;
		event.data.fd = sockfd;
		event.events = EPOLLIN;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) == -1)
		{
			std::cerr << "epoll_ctl failed for sockfd: " << sockfd << std::endl;
			return 1;
		}
	}

	struct epoll_event events[10]; // 一度に処理する最大のイベント数
	while (true)
	{
		int n = epoll_wait(epollfd, events, 10, -1);
		for (int i = 0; i < n; i++)
		{
			if (events[i].events & EPOLLIN)
			{
				char buffer[1024];
				ssize_t bytesRead = read(events[i].data.fd, buffer, sizeof(buffer));
				if (bytesRead < 0) 
				{
					// これはエラーを示しています
					perror("Read error"); // エラーの詳細を表示します
					close(events[i].data.fd); // ソケットを閉じます
					continue;
				}
				struct epoll_event event;
				event.data.fd = events[i].data.fd;
				event.events = EPOLLIN | EPOLLOUT;
				epoll_ctl(epollfd, EPOLL_CTL_MOD, events[i].data.fd, &event);
			}
			if (events[i].events & EPOLLOUT)
			{
				// データを書き込む処理

				// 書き込みが完了したら、EPOLLOUT の監視を停止します
				struct epoll_event event;
				event.data.fd = events[i].data.fd;
				event.events = EPOLLIN;
				epoll_ctl(epollfd, EPOLL_CTL_MOD, events[i].data.fd, &event);
			}
		}
	}
}