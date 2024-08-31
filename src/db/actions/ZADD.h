#pragma once 

#include "IAction.h"
#include <string>

class ZADD : public IAction {

public:
	ZADD()  {}
	int Execute(ArgList args) {
		return 0;
	}
};