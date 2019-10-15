#include "pch.h"

#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>

#include "tcp_client.h"
#include "exception.h"

tcp_client::~tcp_client() {
	disconnect();
}

namespace error_reporting {
	static char buf[1024];
	const char* errorMsg(const char* name) {
		snprintf(buf, 1024, "%s call failed with %d", name, errno);
		return buf;
	}
}

void tcp_client::connect() {
	if (fd_ >= 0)
		throw illegal_state("Already connected");

	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_ < 0) {
		perror("socket");
		throw std::runtime_error(error_reporting::errorMsg("socket"));
	}

	hostent* entry = gethostbyname(host_.c_str());
	if (!entry) {
		perror("gethostbyname");
		throw std::runtime_error("Cannot resolve address \"" + host_ + "\"");
	}

	sockaddr_in addr{ AF_INET, htons(port_), *(struct in_addr*)entry->h_addr };
	if (::connect(fd_, (const sockaddr*)&addr, sizeof(addr)) != 0) {
		perror("connect");
		throw std::runtime_error(error_reporting::errorMsg("connect"));
	}
}

void tcp_client::disconnect() {
	if (fd_ >= 0) {
		close(fd_);
		fd_ = -1;
	}
}

void tcp_client::send(const char* buf, size_t n) {
	// blocking call
	int sent = ::send(fd_, buf, n, 0);
	if (sent != n) {
		perror("send");
		throw std::runtime_error(error_reporting::errorMsg("send"));
	}
}

#define RECV_BUFFER_SIZE 0x1000 // 4K
void tcp_client::recieve(chunk_consumer& consumer) {
	// receive buffer
	char buffer[RECV_BUFFER_SIZE+1];
	while (true) {
		int res = ::recv(fd_, buffer, RECV_BUFFER_SIZE, MSG_DONTWAIT);
		if (res < 0) {
			int err = errno;
			if (err != EWOULDBLOCK) {
				perror("recv");
				throw std::runtime_error(error_reporting::errorMsg("recv"));
			}
			usleep(1000); // 1 ms. TODO: use select instead
		} else if (res > 0) {
			buffer[res] = 0;
			if (!consumer.on_chunk(buffer, res))
				break;
		}
	}
}


