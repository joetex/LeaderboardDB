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
		K* keys;
		V* values;
		MMNode() {
			keys = new K[N];
			values = new V[N];
		}
	};

	class MMTreeNode {
	public:
		K cmin;
		K cmax;
		unsigned int span = 0;
		unsigned int size = 0;
		MMNode* node = nullptr;
		SparseNode<MMTreeNode>* parent = nullptr;
		unsigned int dataId = -1;

		void incrementParents(unsigned int count = 1) {
			SparseNode<MMTreeNode>* cur = parent;
			
			//increment self and all parents up to root
			while (cur != nullptr) {
				cur->dataSpan += count;
				cur = cur->parent;
			}
		}

		void decrementParents(unsigned int count = 1) {
			SparseNode<MMTreeNode>* cur = parent;
			
			//decrement self and all parents up to root
			while (cur != nullptr) {
				cur->dataSpan -= count;
				cur = cur->parent;
			}
		}
	};


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
		
		std::vector<std::tuple<unsigned int, K, V>> ranks;
		ranks.reserve(count);

		//auto start = chrono::high_resolution_clock::now();

		MMTreeNode* treeNode = search(key);

		//auto finish = chrono::high_resolution_clock::now();
		//cout << ">> Searching:  " << ": " << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << "ms\n";

		//start = chrono::high_resolution_clock::now();

		MMNode* node = treeNode->node;
		int rankPos = treeNode->span;
		while (node != nullptr && ranks.size() < count) {
			for (int i = 0; i < treeNode->size; i++) {
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
		current->span = startingRank(current, key);
		return current;
	}

	void traverse(K key, void(itemViewer)(MMTreeNode* data)) {
		nodes.traverse(key, itemViewer);
	}

	unsigned int startingRank(MMTreeNode*treeNode, K key) {

		unsigned int total = 0;
			SparseNode<MMTreeNode>* current = treeNode->parent;
			int childId = 0;
			int prevChildId = current->childId;
		if (isAscending) {


			// accumilate all parent and predecessor children counts
			
			int nodeCount = 0;
			int childCount = 0;

			while (current != nullptr) {
				childId = current->childId;
				if (childId == -1) {  //root node is never a child
					childId = 0;
				}
				for (int i = 0; i < current->n; i++) {
					
					if (!current->leaf && prevChildId == 0 && i <= prevChildId ) {
						total += current->children[i]->dataSpan;
					}

					if (current->data[i]->cmin >= key) break;

					total += current->data[i]->size;
					//if (i >= current->n) break;
					/*if( !current->leaf && current->data[i] != treeNode)
					total += current->children[i]->dataSpan;
					if(i < current->n && current->data[i] != treeNode )
					total += current->data[i]->size;*/
					childCount++;
				}
				nodeCount++;
				current = current->parent;
				prevChildId = childId;
			}

			std::cout << "Node count: " << nodeCount << endl;
			std::cout << "Child count: " << childCount << endl;

			//return childId;

			/*while (cur != nullptr) {

				total += cur->size;

				cur = cur->prev;
			}*/
		}
		else {
			// accumilate all parent and predecessor children counts
			SparseNode<MMTreeNode>* root;
			int nodeCount = 0;
			int childCount = 0;
			while (current != nullptr) {
				childId = current->childId;
				if (childId == -1) {  //root node is never a child
					childId = 0;
				}
				for (int i = 0; i < childId + 1; i++) {
					if (i >= current->n) break;
					if (!current->leaf)
						total += current->children[i]->dataSpan;
					if (current->data[i] != treeNode)
						total += current->data[i]->size;
					childCount++;
				}
				nodeCount++;
				if (current->parent == nullptr) {
					root = current;
				}
				current = current->parent;
				
			}

			total = root->dataSpan - total;
			//while (current != nullptr) {
			//	childId = current->childId;
			//	if (childId == -1) {  //root node is never a child
			//		childId = 0;
			//	}
			//	for (int i = childId; i <= current->m; i++) {
			//		if (!current->leaf && i <= current->n)
			//			total += current->children[i]->dataSpan;
			//		if (i < (current->n) && current->data[i] != treeNode)
			//			total += current->data[i]->size;
			//		childCount++;
			//	}
			//	nodeCount++;
			//	current = current->parent;
			//}
			std::cout << "Node count: " << nodeCount << endl;
			std::cout << "Child count: " << childCount << endl;
			//return childId;
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
			pageCount++;
			nodes.insert(newNode);
			insertKVToNode(newNode, key, value);
			return newNode;
		}
		
		insertKVToNode(current, key, value);

		//reached capacity, split in half into two nodes
		if (current->size >= m) {
			current = split(current);
			
		}
		else {
			updateMinMax(current);
		}
		return current;
	}

	

	// split into two nodes, moving to the right
	MMTreeNode* split(MMTreeNode* left) {
		MMTreeNode* right = new MMTreeNode();
		right->node = new MMNode();
		pageCount++;

		// keep (m*0.8) keys on left node
		// move (m*0.2) keys on right node
		int j = 0;
		for (int i = mSplit ; i < m; i++) {
			
			right->node->keys[j] = left->node->keys[i];
			right->node->values[j] = left->node->values[i];
			j++;
		}

		// update sizes to match moved keys
		left->size = mSplit;
		right->size = m - mSplit;

		// update new minmax of each
		updateMinMax(left);
		updateMinMax(right);


		//---------------------------
		// connect the nodes in doubly linked list
		//---------------------------
		
		// old one connected with left, is now connected with new right
		MMNode* farRight = left->node->next;
		if( farRight != nullptr )
			farRight->prev = right->node;

		// right is now connected in middle between left and far right
		right->node->next = farRight;
		right->node->prev = left->node;

		// left is connected to new right
		left->node->next = right->node;

		//---------------------------
		
		
		// add to the sparse tree
		nodes.insert(right);

		// update span counts 
		left->decrementParents(m - mSplit);
		right->incrementParents(m - mSplit);

		return right;
	}

	MMTreeNode* updateMinMax(MMTreeNode* treeNode) {
		MMNode* node = treeNode->node;
		if (treeNode->size <= 0) return treeNode;
		//from -infinite to +infinite
		if (isAscending) {
			
		treeNode->cmin = node->keys[0];
		treeNode->cmax = node->keys[treeNode->size -1];

			
		}
		// from +infinite to -infinite
		else {
			treeNode->cmax = node->keys[0];
			treeNode->cmin = node->keys[treeNode->size - 1];
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


	MMTreeNode* insertKVToNode(MMTreeNode* treeNode, K key, V value) {
		MMNode* node = treeNode->node;
		K startPos = 0;
		int i;

		//there are no items, insert immediately
		if (treeNode->size == 0) {
			node->keys[0] = key;
			node->values[0] = value;
			treeNode->size++;
			treeNode->incrementParents(1);
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
			for (i = 0; i < treeNode->size; i++) {
				K nodeKey = node->keys[i];
				if ((this->*sortFunc)(key, nodeKey)) {
					mmShiftRight<K>(node->keys, i, treeNode->size);
					mmShiftRight<V>(node->values, i, treeNode->size);
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
		treeNode->size++;
		treeNode->incrementParents(1);

		return treeNode;
	}
};