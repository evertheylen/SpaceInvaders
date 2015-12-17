
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
// making code shorter since (some time now) (insert year when C's preprocessor was first created)
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

template <typename T>
void true_or_throw(bool cond, std::string msg = "") {
	if (not cond) {
		throw T(msg);
	}
}

class CmdError: public SiException {
public:
	CmdError(std::string& _arg, std::string _info):
		SiException(_info), arg(_arg) {}
	
	virtual const char* what() const throw() {
		return (std::string("Error while parsing argument '") + arg + "': " + info).c_str();
	}
	
private:
	std::string arg;
};

// You don't like macro's? Fine.
#undef ADD_EXCEPTION

}
