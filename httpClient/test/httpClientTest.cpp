#include "pch.h"
#include <gtest/gtest.h>

#include "http_request.h"
#include "http_client.h"
#include "tcp_client.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


TEST(HttpClientTests, TestSendRequest) {
	// sendReceive("archeologia.ru", 80, "/modules/gallery/displayimage.php", "album=lastup&cat=0&pos=4");
	// std::cout << "encoded:\"" << "/modules/gallery/displayimage.php" << urlEncode("?album=lastup&cat=0&pos=4") << std::endl;
	std::unique_ptr<http_request> httpRequest(http_request::GET("http://archeologia.ru/modules/gallery/displayimage.php?album=lastup&cat=0&pos=4"));
	http_client httpClient(httpRequest->host(), httpRequest->port());
	ostream_chunk_consumer consumer(std::cout);
	httpClient.send_recv(*httpRequest, consumer);
}

struct TestStatusData {
	std::string input;

	int status;
	std::string comment;
};

static std::regex statusRegex("([^/]+)\\/([^ ]+) (\\d+) (.+)");
void testStatus(const TestStatusData& test) {
	std::smatch match;
	EXPECT_TRUE(std::regex_match(test.input, match, statusRegex));
	std::string strStatus(match[3].first, match[3].second);
	int status = std::stoi(strStatus);
	EXPECT_EQ(test.status, status);
	std::string msg(match[4].first, match[4].second);
	EXPECT_EQ(test.comment, msg);
}

static TestStatusData statusTestData[] = {
	{"HTTP/1.1 200 OK", 200, "OK"},
	{"HTTP/1.1 400 Bad request", 400, "Bad request"},
	{"HTTP/1.1 404 Not Found", 404, "Not Found"},
};

TEST(HttpClientTests, TestStatus) {
	for (const auto& test : statusTestData)
		testStatus(test);
}

TEST(HttpClientTests, TestChunkSize) {
	EXPECT_EQ(31, std::stoi("1f", 0, 16));
	EXPECT_EQ(43, std::stoi("2B", 0, 16));
	EXPECT_EQ(12, std::stoi("C", 0, 16));
}

