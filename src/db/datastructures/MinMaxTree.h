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
template <typename K, typename V, unsigned int N, bool ASC>
class MinMaxTree {
public:
	typedef MinMaxNode<K, V, N, ASC> MMNode;

	MMNode* root = nullptr;
	typedef K(MinMaxTree<K, V, N, ASC>::* sortFuncType)(K, K);
	sortFuncType sortFunc;

	MinMaxTree() {
		root = new MMNode(true);
	}

	MMNode* insert(K key, V data) {
		//cout << "--------------" << key << "------------------------------------" << endl;
		MMNode* newRoot = root->insert(key, data);
		if (root != nullptr) {
			while (root->getParent() != nullptr) {
				root = root->getParent();
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
			cout << std::string(depth, '\t') << "Child = " << next->getMin() << "," << next->getMax() << " :: " << next->getSpan() << endl;

		if (next->getNext() != nullptr) {
			//cout << " :: " << next->nextNode->cmin;
		}
		if (next->getParent() != nullptr) {
			//cout  << " :: Parent = " << next->parent->cmin << "," << next->parent->cmax;
		}
		for (int i = 0; i < next->getKeyCount(); i++) {
			cout << std::string(depth, '\t') << "\tKey[" << next->getKey(i)->data << "] = " << next->getKey(i)->key << endl;
		}
		int highestDepth = 0;
		for (int i = 0; i < next->getChildrenCount(); i++) {
			//if (next->leaf) {
				//cout << std::string(depth, '\t') << "\tKey["<<i<<"] = " << next->data[i]->key << endl;
			//}
			//else {
			//if( i < next->chsize )
			int d = display(next->getChild(i), maxDepth, depth + 1);
			if (d > highestDepth) {
				highestDepth = d;
				if (next == root) {
					//cout << "Deepest: " << highestDepth;

				}
			}
			//return depth;
		//}


		}

		return depth;

	}


	//reverse the ranking in DESC order, so rank 1 is largest number
	std::vector<std::tuple<unsigned int, K, V>> revrange(K key, unsigned int count, int offset = 0) {
		/*if (ASC) {
			return range(key, count, offset);
		}*/
		MMNode* nearest = root->search(key);
		std::vector<std::tuple<unsigned int, K, V>> ranks;
		ranks.reserve(count);
		if (nearest == nullptr) return ranks;

		int i = nearest->getKeyCount() - 1;
		for (; i > 0; i--) {
			if (key >= nearest->getKey(i)->key)
				break;
		}

		unsigned int maxCount = root->getSpan();

		K offsetKey = key;
		int absOffset = abs(offset);
		int startPos = i;
		MMNode* cur = nearest;
		while (absOffset > 0 && cur != nullptr) {

			if (offset > 0)		 startPos++;
			else if (offset < 0) startPos--;
			absOffset--;


			//start at end of previous node
			if (startPos < 0) {
				if (cur->getPrev() != nullptr) {
					cur = cur->getPrev();
					startPos = cur->getKeyCount() - 1;
				}
				else {
					startPos = 0;
					break;
				}
			}

			//start at beggining of next node
			if (startPos >= cur->getKeyCount()) {
				if (cur->getNext() != nullptr) {
					cur = cur->getNext();
					startPos = 0;
				}
				else {
					startPos = cur->getKeyCount() - 1;
					break;
				}
			}
		}
		if (startPos < 0) startPos = 0;

		int rankPos = (maxCount - (nearest->getTempSpan() + (i + 1))) + offset;
		offsetKey = cur->getKey(startPos)->key;


		while (cur != nullptr && ranks.size() < count) {
			for (int i = startPos; i >= 0; i--) {
				K k = cur->getKey(i)->key;
				V v = cur->getKey(i)->data;
				if (ranks.size() > 0) {
					K prevK = std::get<1>(ranks[ranks.size() - 1]);
					if (prevK != k) {
						rankPos++;
					}
				}
				else {
					rankPos++;
				}
				if (cur->getKey(i)->key > offsetKey)//!(this->*sortFunc)(key, node->data[i]->key))
					continue;

				ranks.push_back({ rankPos, k, v });

				if (ranks.size() >= count) break;
			}
			cur = cur->getPrev();
			if (cur != nullptr)
				startPos = cur->getKeyCount() - 1;
		}

		return ranks;

	}

	// pull the rankings in ASC order, so rank 1 is lowest number 
	std::vector<std::tuple<unsigned int, K, V>> range(K key, unsigned int count, int offset = 0) {
		/*if (!ASC) {
			return revrange(key, count, offset);
		}*/

		MMNode* nearest = root->revsearch(key);
		std::vector<std::tuple<unsigned int, K, V>> ranks;
		ranks.reserve(count);
		if (nearest == nullptr) return ranks;

		int i = 0;
		for (; i < nearest->getKeyCount(); i++) {
			if (key <= nearest->getKey(i)->key)
				break;
		}

		K offsetKey = key;
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
				if (cur->getPrev() != nullptr) {
					cur = cur->getPrev();
					startPos = cur->getKeyCount() - 1;
				}
				else {
					startPos = 0;
					break;
				}
			}

			//start at beggining of next node
			if (startPos >= cur->getKeyCount()) {
				if (cur->getNext() != nullptr) {
					cur = cur->getNext();
					startPos = 0;
				}
				else {
					startPos = cur->getKeyCount() - 1;
					break;
				}
			}
		}

		int rankPos = nearest->getTempSpan() + (i - 1) + offset;
		offsetKey = cur->getKey(startPos)->key;


		while (cur != nullptr && ranks.size() < count) {
			for (int i = startPos; i < cur->getKeyCount(); i++) {
				K k = cur->getKey(i)->key;
				V v = cur->getKey(i)->data;
				if (ranks.size() > 0) {
					K prevK = std::get<1>(ranks[ranks.size() - 1]);
					if (prevK != k) {
						rankPos++;
					}
				}
				else {
					rankPos++;
				}
				if (offsetKey > cur->getKey(i)->key)//!(this->*sortFunc)(key, node->data[i]->key))
					continue;

				ranks.push_back({ rankPos, k, v });

				if (ranks.size() >= count) break;
			}
			cur = cur->getNext();
			startPos = 0;
		}

		return ranks;

	}

	MMNode* remove(K key, V data) {
		MMNode* found = root->remove(key, data);

		return found;
	}

	MMNode* search(K key) {

		MMNode* found = root->search(key);

		return found;
	}

	MMNode* searchValue(V data) {
		return nullptr;
	}

	K mmClamp(K key, K cmin, K cmax) {
		return max(min(key, cmax), cmin);
	}
};