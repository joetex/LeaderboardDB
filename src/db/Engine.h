#pragma once
#include <MemoryDB.h>
#include "actions/ActionSystem.h"
#include <chrono>
#include <iostream>

using namespace std;

class Engine {

	MemoryDB* memoryDB;
	ActionSystem* actionSystem;

public:
	Engine() {
		memoryDB = new MemoryDB();
		actionSystem = new ActionSystem(memoryDB);

		//Run();
	}

	~Engine() {
		delete memoryDB;
		delete actionSystem;
	}

	void Run() {
		auto start = chrono::high_resolution_clock::now();
		
		json cols = json::array({ "Joe", "US", 12 });

		for (unsigned int i = 0; i < 1000; i++)
			memoryDB->CreateMember("testTable", cols);

		auto finish = chrono::high_resolution_clock::now();
		cout << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << "ms\n";
	}

	int Execute(string command) {
		try {
			return actionSystem->Execute(command);
		}
		catch (exception e) {
			cout << e.what();
		}
		return -1;
	}


};