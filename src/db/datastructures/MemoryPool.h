#pragma once 

#include <boost/interprocess/managed_shared_memory.hpp>
#include <cstdlib> //std::system
#include <sstream>

#include "db/datastructures/MinMaxNode.h"

class MMNodeMapper {

	typedef MinMaxNode<128> MMNode;

	struct MMNodeStatus {
		MMNode* node = nullptr;
		bool isDirty = false;
		bool isAllocated = false;
	};

	std::unordered_map<unsigned long long, MMNodeStatus> nodes;




	MMNodeMapper() {

	}

public:
	bool contains(unsigned long long key) {
		return nodes.contains(key);
	}

	bool insert(unsigned long long key) {
		return nodes.contains(key);
	}

};