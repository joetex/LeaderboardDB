#pragma once

#include <vector>
#include <algorithm>
#include <utility>
#include <chrono>
#include <cmath>
#include "db/datastructures/MinMaxNode.h"

/**
*
* Recursive MinMax
* Stores data using Linked List for sortedset and Skiplist tree
*
*/
template <unsigned int N, bool ASC>
class MinMaxTree {
public:
	typedef MinMaxNode<N> MMNode;

	MMNode* root = nullptr;
	//typedef K(MinMaxTree<K, V, N, ASC>::* sortFuncType)(K, K);
	//sortFuncType sortFunc;

	MinMaxTree() {
		root = new MMNode(true);
	}

	MMNode* insert(unsigned long long key, unsigned long long data) {
		//cout << "--------------" << key << "------------------------------------" << endl;
		//MMNode* newRoot = root->insert(key, data);

		MMNode* newRoot = nullptr;
		if (ASC) newRoot = root->search(key);
		else newRoot = root->revsearch(key);

		newRoot = newRoot->insertLeaf(key, data, true);


		if (root != nullptr) {
			while (root->parent != nullptr) {
				root = root->parent;
			}
		}
		//display(nullptr);
		return root;
	}

	int display(MMNode* next = nullptr, int maxDepth = -1, int depth = 0) {

		if (next == nullptr) {
			next = root;
		}

		if (maxDepth <= -1 || depth <= maxDepth)
			cout << std::string(depth, '\t') << "Child = " << next->record->cmin << "," << next->record->cmax << " :: " << next->record->span << endl;

		if (next->nextNode != nullptr) {
			//cout << " :: " << next->nextNode->cmin;
		}
		if (next->parent != nullptr) {
			//cout  << " :: Parent = " << next->parent->cmin << "," << next->parent->cmax;
		}
		if (next->record->leaf) {
		for (int i = 0; i < next->record->size; i++) {
			cout << std::string(depth, '\t') << "\tKey[" << next->record->keys[i]->data << "] = " << next->record->keys[i]->key << endl;
		}

		}
		else {
			int highestDepth = 0;
			for (int i = 0; i < next->record->size; i++) {
				//if (next->leaf) {
					//cout << std::string(depth, '\t') << "\tKey["<<i<<"] = " << next->data[i]->key << endl;
				//}
				//else {
				//if( i < next->chsize )
				int d = display(next->record->children[i], maxDepth, depth + 1);
				if (d > highestDepth) {
					highestDepth = d;
					if (next == root) {
						//cout << "Deepest: " << highestDepth;

					}
				}
				//return depth;
			//}


			}
		}
		

		return depth;

	}


	//reverse the ranking in DESC order, so rank 1 is largest number
	std::vector<std::tuple<unsigned int, unsigned long long, unsigned long long>> revrange(unsigned long long key, unsigned int count, int offset = 0) {
		/*if (ASC) {
			return range(key, count, offset);
		}*/
		MMNode* nearest = root->searchWithCount(key);
		std::vector<std::tuple<unsigned int, unsigned long long, unsigned long long>> ranks;
		ranks.reserve(count);
		if (nearest == nullptr) return ranks;

		int i = nearest->record->size - 1;
		for (; i > 0; i--) {
			if (key >= nearest->record->keys[i]->key)
				break;
		}

		unsigned int maxCount = root->record->span;

		unsigned long long offsetKey = key;
		int absOffset = abs(offset);
		int startPos = i;
		MMNode* cur = nearest;
		while (absOffset > 0 && cur != nullptr) {

			if (offset > 0)		 startPos++;
			else if (offset < 0) startPos--;
			absOffset--;


			//start at end of previous node
			if (startPos < 0) {
				if (cur->prevNode != nullptr) {
					cur = cur->prevNode;
					startPos = cur->record->size - 1;
				}
				else {
					startPos = 0;
					break;
				}
			}

			//start at beggining of next node
			if (startPos >= cur->record->size) {
				if (cur->nextNode != nullptr) {
					cur = cur->nextNode;
					startPos = 0;
				}
				else {
					startPos = cur->record->size - 1;
					break;
				}
			}
		}
		if (startPos < 0) startPos = 0;

		int rankPos = (maxCount - (nearest->tempspan + (i + 1))) + offset;
		offsetKey = cur->record->keys[startPos]->key;


		while (cur != nullptr && ranks.size() < count) {
			for (int i = startPos; i >= 0; i--) {
				unsigned long long k = cur->record->keys[i]->key;
				unsigned long long v = cur->record->keys[i]->data;
				if (ranks.size() > 0) {
					unsigned long long prevK = std::get<1>(ranks[ranks.size() - 1]);
					if (prevK != k) {
						rankPos++;
					}
				}
				else {
					rankPos++;
				}
				if (cur->record->keys[i]->key > offsetKey)//!(this->*sortFunc)(key, node->data[i]->key))
					continue;

				ranks.push_back({ rankPos, k, v });

				if (ranks.size() >= count) break;
			}
			cur = cur->prevNode;
			if (cur != nullptr)
				startPos = cur->record->size - 1;
		}

		return ranks;

	}

	// pull the rankings in ASC order, so rank 1 is lowest number 
	std::vector<std::tuple<unsigned int, unsigned long long, unsigned long long>> range(unsigned long long key, unsigned int count, int offset = 0) {
		/*if (!ASC) {
			return revrange(key, count, offset);
		}*/

		MMNode* nearest = root->revsearchWithCount(key);
		std::vector<std::tuple<unsigned int, unsigned long long, unsigned long long>> ranks;
		ranks.reserve(count);
		if (nearest == nullptr) return ranks;

		int i = 0;
		for (; i < nearest->record->size; i++) {
			if (key <= nearest->record->keys[i]->key)
				break;
		}

		unsigned long long offsetKey = key;
		int absOffset = abs(offset);
		int startPos = i;
		if (startPos < 0) startPos = 0;
		MMNode* cur = nearest;
		while (absOffset > 0 && cur != nullptr) {

			if (offset > 0)		 startPos++;
			else if (offset < 0) startPos--;
			absOffset--;


			//start at end of previous node
			if (startPos < 0) {
				if (cur->prevNode != nullptr) {
					cur = cur->prevNode;
					startPos = cur->record->size - 1;
				}
				else {
					startPos = 0;
					break;
				}
			}

			//start at beggining of next node
			if (startPos >= cur->record->size) {
				if (cur->nextNode != nullptr) {
					cur = cur->nextNode;
					startPos = 0;
				}
				else {
					startPos = cur->record->size - 1;
					break;
				}
			}
		}

		int rankPos = nearest->tempspan + (i ) + offset;
		offsetKey = cur->record->keys[startPos]->key;


		while (cur != nullptr && ranks.size() < count) {
			for (int i = startPos; i < cur->record->size; i++) {
				unsigned long long k = cur->record->keys[i]->key;
				unsigned long long v = cur->record->keys[i]->data;
				if (ranks.size() > 0) {
					unsigned long long prevK = std::get<1>(ranks[ranks.size() - 1]);
					if (prevK != k) {
						rankPos++;
					}
				}
				else {
					rankPos++;
				}
				if (offsetKey > cur->record->keys[i]->key)//!(this->*sortFunc)(key, node->data[i]->key))
					continue;

				ranks.push_back({ rankPos, k, v });

				if (ranks.size() >= count) break;
			}
			cur = cur->nextNode;
			startPos = 0;
		}

		return ranks;

	}

	MMNode* remove(unsigned long long key, unsigned long long data) {
		MMNode* found = root->remove(key, data);

		return found;
	}

	MMNode* search(unsigned long long key) {

		MMNode* found = root->search(key);

		return found;
	}

	MMNode* searchValue(unsigned long long data) {
		return nullptr;
	}

	unsigned long long mmClamp(unsigned long long key, unsigned long long cmin, unsigned long long cmax) {
		return max(min(key, cmax), cmin);
	}
};