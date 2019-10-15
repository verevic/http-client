#include "pch.h"
#include "http_request.h"

#include "URL.h"

namespace HTTP_CONST {
	std::string Localhost("localhost");
	std::string Get("GET");
	std::string NoBody;
	std::string Root("/");
}

class http_request_impl : public http_request {
	URL url_;
public:
	http_request_impl(const URL& url) : url_(url) {}	// generated URL(const URL&) is ok

	const std::string& host() const {
		const std::string& host = url_.host();
		if (!host.empty())
			return host;

		return HTTP_CONST::Localhost;
	}
	int port() const {
		int port = url_.port();
		if (port == 0)
			port = 80;
		return port;
	}
	// const std::string& method() const;
	const std::string& path() const {
		const std::string& path = url_.path();
		if (!path.empty()) {
			return path;
		}
		return HTTP_CONST::Root;
	}
	const std::string& query() const {
		return url_.query();
	}

	// const std::string& body() const;
};

class http_get_request : public http_request_impl {
public:
	http_get_request(const URL& url) : http_request_impl(url) {}

	const std::string& method() const {
		return HTTP_CONST::Get;
	}
	const std::string& body() const {
		return HTTP_CONST::NoBody;
	}
};

http_request* http_request::GET(const URL& url) {
	return new http_get_request(url);
}

http_request* http_request::GET(const std::string& spec) {
	return new http_get_request(URL::fromString(spec));
}

