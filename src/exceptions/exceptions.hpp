
#pragma once

#include <exception>
#include <string>

namespace si {

class SiException: public std::exception {
public:
	SiException(std::string _info): info("SiException, info: ") {
		info += _info;
	}
	
	virtual const char* what() const throw() {
		return info.c_str();
	}
	
protected:
	std::string info;
};



class ParseError: public SiException {
public:
	ParseError(std::string _info):
		SiException(std::string("Error while parsing: ") + _info) {}
};

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

}
