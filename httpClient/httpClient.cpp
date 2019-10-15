// httpClient.cpp : Defines the entry point for the application.
//
#include "pch.h"

#include "http_request.h"
#include "http_client.h"
#include "tcp_client.h"

using namespace std;

void usage() {
}

int main(int argc, const char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <URL>" << std::endl;
		return 1;
	}

	try {
		std::unique_ptr<http_request> httpRequest(http_request::GET(argv[1]));
		http_client httpClient(httpRequest->host(), httpRequest->port());
		ostream_chunk_consumer consumer(std::cout);
		httpClient.send_recv(*httpRequest, consumer);
	} catch (const std::exception& e) {
		std::cerr << "Caught an exception. " << e.what() << std::endl;
	}

	return 0;
}
