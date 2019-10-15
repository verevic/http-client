#pragma once

/** Thrown to report illegal state.  */
class illegal_state : public std::logic_error {
public:
	explicit illegal_state(const std::string& s) : std::logic_error(s.c_str()) {}
	explicit illegal_state(const char* s) : std::logic_error(s) {}
};

