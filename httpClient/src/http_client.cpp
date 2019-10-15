#include "pch.h"

#include "http_client.h"
#include "http_request.h"
#include "tcp_client.h"
#include "exception.h"

#include <cstring>

class http_chunk_consumer : public chunk_consumer {
private:
	const static std::regex statusRegex_;

	int httpStatus_ = -1;
	bool chunked_ = false;
	int chunkSize_ = -1;
	int contentLength_ = -1;
	bool parsingHeader_ = true;
	size_t contentPos_ = 0;
	std::string content_;	// not processed yet raw data

	chunk_consumer& consumer_;

	void readStatus() {
		size_t eoln = content_.find_first_of('\n', contentPos_);
		if (eoln == std::string::npos) {
			throw illegal_state("getStatus invoked with an incomplete line");
		}
		size_t to = eoln;
		if (content_[to - 1] == '\r')
			to--;
		std::smatch match;
		std::string line = content_.substr(contentPos_, to - contentPos_);
		if (std::regex_match(line, match, statusRegex_)) {
			std::string strStatus(match[3].first, match[3].second);
			httpStatus_ = std::stoi(strStatus);
			if (httpStatus_ != 200) {
				std::cout << content_ << std::endl;
				char msg[100];
				std::sprintf(msg, "Server replied with status %d(%s)", httpStatus_, std::string(match[4].first, match[4].second).c_str());
				throw std::runtime_error(msg);
			}
		} else {
			std::cout << "Unexpected status line:" << line << std::endl;
			throw std::runtime_error("Unexpected status line received from server");
		}
		contentPos_ = eoln + 1;
	}

	void parseHeader() {
		while (parsingHeader_) {
			size_t eoln = content_.find_first_of('\n', contentPos_);
			if (eoln == std::string::npos) {
				return; // keep reading
			}
			// status
			if (httpStatus_ < 0) {
				readStatus();
				continue;
			}

			if (eoln - contentPos_ <= 2) {
				parsingHeader_ = false;
				contentPos_ = eoln + 1;
				return;
			}

			size_t separator = content_.find_first_of(':', contentPos_);
			if (separator == std::string::npos) {
				std::cout << std::endl << "Invalid message header entry(" << content_.substr(contentPos_, eoln - contentPos_) << ")" << std::endl;
				throw std::runtime_error("Invalid message header entry");
			}

			std::string key = content_.substr(contentPos_, separator - contentPos_);
			std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return std::tolower(c); });
			if (key.compare("transfer-encoding") == 0) {
				// just need to know if it's chunked, all others (compress, gzip, ...) could be ignored at this point
				std::string value = content_.substr(separator + 1, eoln - separator - 1);
				chunked_ = value.find("chunked") != std::string::npos;
			} else if (key.compare("content-length") == 0) {
				size_t from = separator + 1;
				if (content_[from] == ' ')
					from++;
				size_t to = eoln;
				if (content_[to - 1] == '\r')
					to--;
				contentLength_ = std::stoi(content_.substr(from, to - from));
			}
			contentPos_ = eoln + 1;
		}
	}

	bool readContextWithLength() {
		size_t remaining = content_.size() - contentPos_;
		if (remaining > 0) {
			if (remaining > contentLength_) {
				remaining = contentLength_;
			}
			consumer_.on_chunk(&content_[contentPos_], remaining);
			contentLength_ -= remaining;

			content_.clear();
			contentPos_ = 0;
		}
		return contentLength_ > 0;
	}

public:
	http_chunk_consumer(chunk_consumer& consumer) : consumer_(consumer) {}
	bool on_chunk(const char* buffer, size_t len) {
		content_.append(buffer, len);
		//
		// - read status
		// - read header
		// - read body:
		//   - read content or
		//   - read chunks:
		//     - read chunk
		//     - read size

		// header
		if (parsingHeader_) {
			parseHeader();
			if (parsingHeader_)
				return true;
		}

		// body
		// content-length has been specified
		if (contentLength_ >= 0) {
			return readContextWithLength();
		}
		// chunked
		if (chunked_) {
			while (true) {
				// chunk size
				if (chunkSize_ < 0) {
					size_t eoln = content_.find_first_of('\n', contentPos_);
					if (eoln == std::string::npos) {
						return true; // keep reading
					}
					// empty line -- done
					if (eoln - contentPos_ <= 1)
						return false;
					// chunk size
					size_t from = contentPos_;
					size_t to = eoln;
					if (content_[to-1] == '\r') {
						to--;
					}
					chunkSize_ = std::stoi(content_.substr(from, to-from), 0, 16);
					if (chunkSize_ == 0)
						return false;

					contentPos_ = eoln + 1;
				}
				// chunk body
				size_t remaining = content_.size() - contentPos_;
				size_t report = std::min(remaining, (size_t)chunkSize_);
				consumer_.on_chunk(&content_[contentPos_], report);
				chunkSize_ -= remaining;
				if (chunkSize_ > 0) {
					contentPos_ = 0;
					content_.clear();
					return true;
				} 

				chunkSize_ = -1;
				contentPos_ += report;
				// chunk normally ends with an extra end-of-line (not counted in the chunkSize_)
				size_t eoln = content_.find_first_of('\n', contentPos_);
				if (eoln == std::string::npos) {
					return true; // keep reading
				}
				contentPos_ = eoln + 1;
			}
		}
	}
};
const std::regex http_chunk_consumer::statusRegex_("([^/]+)\\/([^ ]+) (\\d+) (.+)"); // HTTP/1.1 200 OK


void http_client::send_recv(const http_request& request, chunk_consumer& consumer) {
	// create tpc client per request... might think of reusing client...
	const std::string& host = request.host();
	int port = request.port();
	tcp_client client(host, port);
	client.connect();

	std::ostringstream httpRequest;
	httpRequest << request.method() << " " << request.path();
	const std::string& query = request.query();
	if (!query.empty()) {
		httpRequest << "?" << query;
	}
	httpRequest << " HTTP/1.1\n"
		"Host: " << host << "\n"
		"cache-control: no-cache\n"
		"Accept-Encoding: identity\n"
		"Connection: close\n\n";
	// No body since I'm using GET
	std::string sreq = httpRequest.str();
	client.send(sreq.c_str(), sreq.size());

	// chunk collector
	http_chunk_consumer httpConsumer(consumer);
	client.recieve(httpConsumer);
}

