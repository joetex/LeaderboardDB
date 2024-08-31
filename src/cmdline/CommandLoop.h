#pragma once 
#include "../db/Engine.h"

class CommandLoop {


public:
	CommandLoop();

	void Begin(Engine* engine);
	void End();
};