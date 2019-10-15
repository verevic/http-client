#include "pch.h"

#include <gtest/gtest.h>
#include <URL.h>

struct UrlTestData {
	std::string spec;

	std::string protocol;
	std::string userInfo;
	std::string host;
	int port;
	std::string path;
	std::string query;
	std::string fragment;
};

void testURL(const UrlTestData& data) {
	URL url = URL::fromString(data.spec);
	EXPECT_EQ(url.protocol(), data.protocol);
	EXPECT_EQ(url.userInfo(), data.userInfo);
	EXPECT_EQ(url.host(), data.host);
	EXPECT_EQ(url.port(), data.port);
	EXPECT_EQ(url.path(), data.path);
	EXPECT_EQ(url.query(), data.query);
	EXPECT_EQ(url.fragment(), data.fragment);
}

static UrlTestData urlTestData[] = {
	{"http://localhost", "http", "", "localhost", 0, "", "", ""},
	{"http://localhost/", "http", "", "localhost", 0, "/", "", ""},
	{"http://verevic:password@localhost", "http", "verevic:password", "localhost", 0, "", "", ""},
	{"http://verevic@localhost", "http", "verevic", "localhost", 0, "", "", ""},
	{"http://verevic:password@mail.ru:8080/folder/index.html?id=123&name=aname#123", "http", "verevic:password", "mail.ru", 8080, "/folder/index.html", "id=123&name=aname", "123"},
};

TEST(URLTests, ParseURL) {
	for (const UrlTestData& test : urlTestData) {
		testURL(test);
	}
}
