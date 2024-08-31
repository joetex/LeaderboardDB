#pragma once

#include <vector>
#include <algorithm>
#include <utility>
#include <chrono>

#include "db/datastructures/SparseBTree.h"
//#include <functional>


template <typename K, typename V, unsigned int N>
class MinMaxSort {

	unsigned int m;
	unsigned int mSplit;

public:

	class MMNode {
	public:
		MMNode* next = nullptr;
		MMNode* prev = nullptr;
		unsigned int size = 0;
		unsigned int span = 0;
		bool isDirty = true;
		unsigned int pageId = 0;
		K* keys;
		V* values;
		//MMNodeList list;
		MMNode() {
			keys = new K[N];
			values = new V[N];
			//for (int i = 0; i < N; i++) {
				//*(keys + i) = nullptr;
				//*(values + i) = nullptr;
			//}
		}
	};

	struct MMTreeNode {
		K cmin;
		K cmax;
		unsigned int span = 0;
		unsigned int treeNodeCount = 0;
		MMNode* node = nullptr;
	};

	std::vector<unsigned int> spans;

	MMNode* root = null;
	SparseBTree<MMTreeNode> nodes;
	int pageCount = 0;
	bool isAscending = false;



	typedef K (MinMaxSort<K, V, N>::*sortFuncType)(K, K);
	//std::function< K (K, K)> sortFunc;

	//template <typename T>
	

	sortFuncType sortFunc;


	MinMaxSort(bool ascending = true) :
		isAscending(ascending),
		m(N),
		//mSplit(1),
		mSplit(floor(N * 0.5)), 
		nodes(N) {

		if (ascending) {
			sortFunc = &MinMaxSort<K,V,N>::ascSort;
		}
		else {
			sortFunc = &MinMaxSort<K, V, N>::descSort;
		}
	}
	K ascSort(K a, K b) {
		return a <= b;
	}
	K descSort(K a, K b) {
		return a >= b;
	}


	std::vector<std::tuple<unsigned int, K, V>> range(K key, unsigned int count) {
		
		std::vector<std::tuple<unsigned int, K,V>> ranks(count);

		//auto start = chrono::high_resolution_clock::now();

		MMTreeNode* treeNode = search(key);

		//auto finish = chrono::high_resolution_clock::now();
		//cout << ">> Searching:  " << ": " << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << "ms\n";

		//start = chrono::high_resolution_clock::now();

		MMNode* node = treeNode->node;
		int rankPos = treeNode->span;
		while (node != nullptr && ranks.size() < count) {
			for (int i = 0; i < node->size; i++) {
				K k = node->keys[i];
				K v = node->values[i];
				if (i > 0) {
					K prevK = node->keys[i - 1];
					if (prevK != k) {
						rankPos++;
					}
				}
				else {
					rankPos++;
				}
				if (!(this->*sortFunc)(key, node->keys[i]))
					continue;

				ranks.push_back({ rankPos, k, v });

				if (ranks.size() >= count) break;
			}
			node = node->next;
		}
		//finish = chrono::high_resolution_clock::now();
		//cout << ">> Looping:  " << ": " << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << "ms\n";


		return ranks;
	}

	MMTreeNode* search(K key) {
		MMTreeNode* current = nodes.search(key);
		current->span = startingRank(current->node);
		return current;
	}

	void traverse(K key, void(itemViewer)(MMTreeNode* data)) {
		nodes.traverse(key, itemViewer);
	}

	unsigned int startingRank(MMNode *node) {

		unsigned int total = 0;
		if (isAscending) {
			MMNode* cur = node->prev;
			return cur->span;

			/*while (cur != nullptr) {

				total += cur->size;

				cur = cur->prev;
			}*/

			

		}
		else {
			MMNode* cur = node->prev;
			return cur->span;
			/*
			while (cur != nullptr) {

				total += cur->size;

				cur = cur->prev;
			}*/
			

		}
		
		return total;
	}

	MMTreeNode*insert(K key, V value) {
		MMTreeNode* current = nodes.search(key);
		
		bool isFound = true;
		if (current == nullptr) {
			isFound = false;
			MMTreeNode* newNode = new MMTreeNode();
			newNode->cmin = key;
			newNode->cmax = key;
			newNode->node = new MMNode();
			newNode->node->pageId = pageCount++;
			if (pageCount == 1) {
				root = newNode;
			}
			spans.push_back(newNode->node->size);
			nodes.insert(newNode);
			insertKVToNode(newNode, key, value);
			return newNode;
		}
		
		insertKVToNode(current, key, value);

		//reached capacity, split in half into two nodes
		if (current->node->size >= m) {
			current = split(current);
			nodes.insert(current);
		}
		else {
			updateMinMax(current);
		}
		return current;
	}

	//split into two nodes, moving to the right
	MMTreeNode* split(MMTreeNode* left) {
		MMTreeNode* right = new MMTreeNode();
		right->node = new MMNode();
		right->node->pageId = pageCount++;
		spans.push_back(0);

		//keep (m*0.8) keys on left node
		//move (m*0.2) keys on right node
		int j = 0;
		for (int i = mSplit ; i < m; i++) {
			
			right->node->keys[j] = left->node->keys[i];
			right->node->values[j] = left->node->values[i];
			j++;
		}

		//update sizes to match moved keys
		left->node->size = mSplit;
		right->node->size = m - mSplit;

		//update new minmax of each
		updateMinMax(left);
		updateMinMax(right);
		spans[left->node->pageId] = left->node->size;
		spans[right->node->pageId] = right->node->size;

		//connect the nodes in doubly linked list

		//old one connected with left, is now connected with right
		MMNode* farRight = left->node->next;
		if( farRight != nullptr )
			farRight->prev = right->node;

		//right is now connected in middle between left and far right
		right->node->next = farRight;
		right->node->prev = left->node;

		//left is connected to new right
		left->node->next = right->node;

		return right;
	}

	MMTreeNode* updateMinMax(MMTreeNode* treeNode) {
		MMNode* node = treeNode->node;
		if (node->size <= 0) return treeNode;
		//from -infinite to +infinite
		if (isAscending) {
			
		treeNode->cmin = node->keys[0];
		treeNode->cmax = node->keys[node->size -1];

			
		}
		// from +infinite to -infinite
		else {
			treeNode->cmax = node->keys[0];
			treeNode->cmin = node->keys[node->size - 1];
		}

		return treeNode;
	}

	K mmClamp(K key, K cmin, K cmax) {
		return max(min(key, cmax), cmin);
	}

	template <typename T>
	void mmShiftRight(T* arr, int i, unsigned int& size) {
		for (int idx = size - 1; idx >= i; idx--) {
			arr[idx + 1] = arr[idx];
		}
	}

	void updateSpans(MMNode* node) {
		unsigned int total = 0;
		if (isAscending) {
			MMNode* cur = node->prev;
			
			while (cur != nullptr) {
				total += cur->size;

				cur = cur->prev;
			}



		}
		else {
			MMNode* cur = node->prev;
			
			while (cur != nullptr) {

				total += cur->size;

				cur = cur->prev;
			}


		}
	}

	MMTreeNode* insertKVToNode(MMTreeNode* treeNode, K key, V value) {
		MMNode* node = treeNode->node;
		K startPos = 0;
		int i;

		//there are no items, insert immediately
		if (node->size == 0) {
			node->keys[0] = key;
			node->values[0] = value;
			node->size++;
			
			return treeNode;
		}

		//skip calculations if list is small enough
		/*if (node->size >= 10) {
			K diff = treeNode->cmax - treeNode->cmin;
			K keyDiff = key - treeNode->cmin;
			if (diff > 0)
				startPos = mmClamp(node->size * floor(keyDiff / diff), 0, node->size - 1);
		}*/

		// to avoid looping through all items in a large list, 
		//   attempt to calculate the predicted index, where key should be inserted
		//   then loop direction forward or backward depending on the predicted index key vs the new key
		//if (key >= node->keys[startPos]) {
			for (i = 0; i < node->size; i++) {
				K nodeKey = node->keys[i];
				if ((this->*sortFunc)(key, nodeKey)) {
					mmShiftRight<K>(node->keys, i, node->size);
					mmShiftRight<V>(node->values, i, node->size);
					break;
				}
			}
			//if (i > node->size)	i = min(m, node->size);
		//}
		//else {
		//	for (i = startPos; i >= 0; i--) {
		//		K nodeKey = node->keys[i];
		//		if ((this->*sortFunc)(key, nodeKey)) {
		//			mmShiftRight<K>(node->keys, i, node->size);
		//			mmShiftRight<V>(node->values, i, node->size);
		//			break;
		//		}
		//	}
		//	//if (i < 0)	i = 0;
		//}
		
		node->keys[i] = key;
		node->values[i] = value;
		node->size++;

		return treeNode;
	}
};