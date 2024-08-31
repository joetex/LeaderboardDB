
#pragma once
#include <string>
#include <map>
#include <vector>

typedef std::vector<std::string> ArgList;

class IAction {

public:
	IAction() {};
	virtual int Execute(ArgList args) = 0;
};



