#include "pch.h"
#include <URL.h>


const static std::string url_regex_str = //"(?:"
"(ssh|sftp|ftp|http|https):\\/\\/" //protocol
"(?:([^@ ]*)@)?"			//userInfo
"([^:?#/ ]+)"				//host
"(?::(\\d+))?"				//port
"([^?# ]*)"					//path
"(?:\\?([^# ]*))?"			//query
"(?:#([^ ]*))?";			//fragment

const static std::regex urlRegex(url_regex_str);

URL::URL(const std::string& spec) {
	std::smatch match;
	if (std::regex_match(spec, match, urlRegex)) {
		protocol_ = std::string(match[1].first, match[1].second);
		userInfo_ = std::string(match[2].first, match[2].second);
		host_ = std::string(match[3].first, match[3].second);
		std::string sPort = std::string(match[4].first, match[4].second);
		port_ = sPort.empty() ? 0 : std::stoi(sPort);
		path_ = std::string(match[5].first, match[5].second);
		query_ = std::string(match[6].first, match[6].second);
		fragment_ = std::string(match[7].first, match[7].second);
	} else {
		throw std::invalid_argument("Malformed URL, " + spec);
	}
}

