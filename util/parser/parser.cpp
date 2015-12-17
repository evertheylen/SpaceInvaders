
#include "parser.hpp"

template <>
unsigned int si::util::get<unsigned int>(const picojson::value& conf, const std::string& s) {
	double v = si::util::get<double>(conf, s);
	if (v < 0) {
		throw ParseError(s + " is not positive");
	}
	return static_cast<unsigned int>(v);
}

template <>
unsigned int si::util::get<unsigned int>(const picojson::object& m, const std::string& s) {
	double v = si::util::get<double>(m, s);
	if (v < 0) {
		throw ParseError(s + " is not positive");
	}
	return static_cast<unsigned int>(v);
}
