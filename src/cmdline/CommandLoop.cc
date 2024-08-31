
#include <iostream>
#include <string>
#include "cmdline/CommandLoop.h"

CommandLoop::CommandLoop() {
	
}

void CommandLoop::Begin(Engine* engine) {

	std::string command = "";

	while (true) {
		std::cout << "> ";
		std::getline(std::cin, command);


		if (command == "exit" || command == "EXIT") {
			End();
			break;
		}

		int returnCode = engine->Execute(command);
		std::cout << command << " executed with return code: " << returnCode << std::endl;

	}

}

void CommandLoop::End() {

}