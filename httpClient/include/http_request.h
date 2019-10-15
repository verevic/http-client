#pragma once

class URL;

class http_request {
public:
	virtual ~http_request() {}

	virtual const std::string& host() const = 0;
	virtual int port() const = 0;
	virtual const std::string& method() const = 0;
	virtual const std::string& path() const = 0;
	virtual const std::string& query() const = 0;

	virtual const std::string& body() const = 0;

	static http_request* GET(const URL&);
	static http_request* GET(const std::string&);
};

