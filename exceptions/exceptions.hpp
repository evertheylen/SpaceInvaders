
#pragma once

#include <exception>
#include <string>

namespace si {

class SiException: public std::exception {
public:
	SiException(const std::string& _info):
		info(_info) {}
	
	virtual const char* what() const throw() {
		return (std::string("SiException, info: ") + info).c_str();
	}
	
protected:
	std::string info;
};

// yay macro's
// making code shorter since 
#define ADD_EXCEPTION(type, msg) \
	class type: public SiException {\
	public:\
		type(const std::string& _info): SiException(_info) {}\
		\
		virtual const char* what() const throw() {\
			return (std::string("") + msg).c_str();\
		}\
}

ADD_EXCEPTION(ParseError, "Error while parsing: " + info);

// You don't like macro's? Fine.
#undef ADD_EXCEPTION

}
