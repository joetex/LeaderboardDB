#pragma once 
#include <memory>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "IAction.h"
#include "ZADD.h"
#include "../MemoryDB.h"


typedef std::unique_ptr<IAction> IActionPtr;
typedef std::map<std::string, IActionPtr> ActionMap;

class ActionSystem {
	ActionMap actionList;

	MemoryDB *memoryDB;

public:
	ActionSystem(MemoryDB* db) {
		memoryDB = db;
		RegisterActions();
	}

	void RegisterActions() {
		actionList["ZADD"] = IActionPtr(new ZADD());
	}

	int Execute(std::string command) {
		ArgList args = ParseCommandArguments(command);
		std::string actionName = args[0];

		if (actionList[actionName] == nullptr) {
			return -1;
		}
		return actionList[actionName]->Execute(args);
	}

	ArgList ParseCommandArguments(std::string command) {
		ArgList args;
		std::stringstream commandStream(command);
		std::string part;
		while (std::getline(commandStream, part, ' ')) {
			args.push_back(part);
		}
		return args;
	}
};