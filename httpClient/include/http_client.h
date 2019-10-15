#pragma once

class http_request;
class chunk_consumer;

class http_client {
private:
	std::string host_;
	int port_;

public:
	http_client(const std::string& host, int port) : host_(host), port_(port) {}

	void send_recv(const http_request&, chunk_consumer&);
};

