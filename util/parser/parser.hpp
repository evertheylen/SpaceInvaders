
/* [bake me]

dependencies["headers"] = [
	"exceptions>>headers"
]

[stop baking] */

#pragma once

#include <string>

#include "libs/picojson.hpp"

#include "exceptions/exceptions.hpp"

namespace si {
namespace util {

// Some duplicate code here, but I keep it mainly for the more accurate error messages

template <typename T>
T get(const picojson::value& conf, const std::string& s) {
	if (conf.is<picojson::object>()) {
		picojson::object m = conf.get<picojson::object>();
		auto where = m.find(s);
		if (where == m.end()) {
			throw si::ParseError(std::string("Did not contain ") + s);
		}
		if (not where->second.is<T>()) throw ParseError(std::string("'") + s + "' of config is not of required type");
		return where->second.get<T>();
	} else {
		throw ParseError("config is not an object");
	}
}

template <typename T>
T get(const picojson::object& m, const std::string& s) {
	auto where = m.find(s);
	if (where == m.end()) {
		throw si::ParseError(std::string("Did not contain ") + s);
	}
	if (not where->second.is<T>()) throw ParseError(std::string("'") + s + "' of config is not of required type");
	return where->second.get<T>();
}

template <>
unsigned int get<unsigned int>(const picojson::value& conf, const std::string& s);

template <>
unsigned int get<unsigned int>(const picojson::object& m, const std::string& s);

template <typename T>
T convert(const picojson::value& val) {
	if (not val.is<T>()) throw ParseError("value is not of required type");
	return val.get<T>();
}

}
}
