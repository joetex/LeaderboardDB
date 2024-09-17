#pragma once

#include <iostream>
#include <vector>
#include <memory>

const uint32_t maxTreeSize = 300;
const uint32_t maxDepth = 1000;


template<typename S, typename T>
class HashTree {


public:
	struct Node {
		S key;
		T value;
		uint32_t nextTreeId = 0;
	};

	struct Tree {
		Node nodes[maxTreeSize];
		uint32_t depth = 0;
	};

	std::vector<Tree> trees;
	typedef std::size_t(*HashFunction)(S, uint32_t);

	HashFunction hashFunc = nullptr;
	HashTree(HashFunction hf) {
		hashFunc = hf;
		createTree();
	}

	size_t createTree() {
		Tree tree;

		//tree->nodes.reserve(maxTreeSize);

		//create the default nodes
		/*for (short i = 0; i < maxTreeSize; i++) {
			tree->nodes.push_back(nullptr);
		}*/

		trees.push_back(tree);
		return trees.size() - 1;
	}

	bool insert(S key, T val) {
		std::size_t hashval = hashFunc(key, 0);
		
		std::size_t lastTreeId = 0;
		uint32_t nodeId = hashval % maxTreeSize;
		Node found = (trees[lastTreeId].nodes[nodeId]);
		uint32_t depth = 0;

		//find a node down the line of trees
		while (found.key != "" && depth < maxDepth) {
			//if( depth > 2 )
			//std::cout << "Collision at treeId: " << lastTreeId << " nodeId: " << nodeId << " depth: " << depth << " " << key << " existing key: " << found->key << endl;
			if (found.nextTreeId == 0) {
				lastTreeId = found.nextTreeId = createTree();
			}
			else {
				lastTreeId = found.nextTreeId;
			}
			nodeId = hashFunc(key, ++depth) % maxTreeSize;
			found = (trees[lastTreeId].nodes[nodeId]);
		}

		if (found.key == "") {
			found.key = key;
			found.value = val;
			found.nextTreeId = 0;
			trees[lastTreeId].nodes[nodeId] = found;
			//std::cout << "Inserted node at depth: " << depth << " using tree " << lastTreeId << " nodeId: " << nodeId << " " << key << endl;
			return true;
		}

		std::cout << "Failed to insert a node at depth: " << depth << " using tree " << lastTreeId << " " << key << endl;
		return false;
	}

	Node* find(S key) {
		std::size_t hashval = hashFunc(key, 0);

		std::size_t lastTreeId = 0;
		uint32_t nodeId = hashval % maxTreeSize;
		Node found = trees[lastTreeId].nodes[nodeId];
		uint32_t depth = 0;

		//find a node down the line of trees
		while (found.key != key && depth < maxDepth) {
			nodeId = hashFunc(key, ++depth) % maxTreeSize;
			lastTreeId = found.nextTreeId;
			found = trees[lastTreeId].nodes[nodeId];
		}

		if (found.key == key) {
			std::cout << "Found node at depth:" << depth << " using treeId:" << lastTreeId << " nodeId:" << nodeId << " key:" << key << endl;
			return &found;
		}

		std::cout << "Failed to find node at depth:" << depth << " using treeId:" << lastTreeId << " nodeId:" << nodeId << " key:" << key << endl;
		return nullptr;
	}

};