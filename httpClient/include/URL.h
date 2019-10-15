#pragma once

class URL {
private:
	std::string protocol_;
	std::string userInfo_;
	std::string host_;
	int port_;
	std::string path_;
	std::string query_;
	std::string fragment_;

	URL(const std::string& spec);

public:
	static URL fromString(const std::string& spec) { return URL(spec); }

	const std::string& protocol() const { return protocol_; }
	const std::string& userInfo() const { return userInfo_; }
	const std::string& host() const { return host_; }
	int port() const { return port_; }
	const std::string& path() const { return path_; }
	const std::string& query() const { return query_; }
	const std::string& fragment() const { return fragment_; }
};

