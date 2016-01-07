
#include "util.hpp"
#include <iostream>

using namespace si::util;

std::vector<std::string> split(std::string& s, char delim) {
	std::vector<std::string> result;
	std::string buf;
	for (char c: s) {
		if (c == delim) {
			result.push_back(buf);
			buf.clear();
		} else {
			buf += c;
		}
	}
	result.push_back(buf);
	return result;
}

