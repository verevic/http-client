#pragma once

class chunk_consumer;

class tcp_client
{
private:
	int fd_ = -1;
	std::string host_;
	int port_;

public:
	tcp_client(const std::string& host, int port) : host_(host), port_(port) {}
	virtual ~tcp_client();

	void connect();
	void disconnect();
	void send(const char* buf, size_t n);
	// let caller group/chop chunks into messages/etc
	void recieve(chunk_consumer&);
};

class chunk_consumer
{
public:
	virtual ~chunk_consumer() {}

	virtual bool on_chunk(const char*, size_t len) = 0;	// returns true to keep reading
};

class ostream_chunk_consumer : public chunk_consumer {
	std::ostream& out_;
public:
	ostream_chunk_consumer(std::ostream& out) : out_(out) {}
	~ostream_chunk_consumer() { out_.flush(); } // 
	bool on_chunk(const char* chunk, size_t len) { out_.write(chunk, len); }
};

