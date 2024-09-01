#pragma once

#include <vector>
#include <algorithm>
#include <utility>
#include <chrono>

//#include "db/datastructures/SparseBTree.h"
//#include <functional>


template <typename K, typename V, unsigned int N, bool ASC>
class RecursiveMinMax {

public:

	class MMKey {
	public:
		K key;
		V value;
	};

	class MMNode {
	public:
		K cmin;
		K cmax;

		//0 = internal
		//1 = leaf
		bool leaf = true;
		MMNode** children;
		MMKey** data;

		MMNode* parent = nullptr;
		unsigned int childId = -1;

		unsigned int size = 0;

		unsigned int nsplit = 0;


		MMNode(bool isLeaf) : leaf(isLeaf), nsplit( floor(N*0.5) ) {
			if (isLeaf)
				data = new MMKey * [N];
			else
				children = new MMNode * [N];
		}

		MMNode* searchBestNode(K key) {
			if (leaf) {
				for (int i = 0; i < size; i++) {
					if (key == data[i]->key)
						return this;
					if (key >= cmin && key <= cmax)
						return this;
				}
				return nullptr;
			}

			for (int i = 0; i < size; i++) {
				if (key >= children[i]->cmin && key <= children[i]->cmax) {
					MMNode* found = children[i]->searchBestNode(key);
					if (found)
						return found;
				}
			}
			return nullptr;
		}

		MMNode* search(K key) {
			if (leaf) {
				for (int i = 0; i < size; i++) {
					if (key == data[i]->key)
						return this;
				}
				return nullptr;
			}

			if (key >= cmin && key <= cmax) {
				for (int i = 0; i < size; i++) {
					MMNode* found = children[i]->search(key);
					if (found)
						return found;
				}
			}
			return nullptr;
		}


		MMNode *insert(MMNode* node) {
			return insertNode(node);
		}

		MMNode *insert(K key, V value) {

			if (!leaf) {
				if (key >= children[size - 1]->cmax) {
					return children[size - 1]->insert(key, value);
				}
				
				for (int i = 0; i < size; i++) {

					if (key >= cmin && key <= cmax) {
						return children[i]->insert(key, value);
					}

					if (key > cmax) continue;
				}
			}

			return insertLeaf(key, value);
		}

		MMNode *insertLeaf(K key, V value) {
			//first kv or greater than entire list
			if (size == 0 || key >= data[size - 1]->key) {
				data[size] = new MMKey();
				data[size]->key = key;
				data[size]->value = value;
				size++;
				updateMinMaxLeaf();
			}

			//find index where key is less, insert into index and shift to right
			int i = 0;
			for (; i < size; i++) {
				if (key < data[i]->key) {
					mmShiftRight<MMKey*>(data, i, size);
					data[i] = new MMKey();
					data[i]->key = key;
					data[i]->value = value;
					size++;
					if (i == 0)
						updateMinMaxLeaf();
					break;
				}
			}

			//reached capacity, split the node
			if (size == N) {
				return splitLeaf(); //new root?
			}
			return nullptr;
		}

		void updateMinMaxLeaf() {
			if (size <= 0) return;
			//from -infinite to +infinite
			if (ASC) {
				cmin = data[0]->key;
				cmax = data[size - 1]->key;

				MMNode* cur = parent;
				while (cur != nullptr) {
					cur->updateMinMaxNode();
					cur = cur->parent;
				}
			}
			// from +infinite to -infinite
			else {
				cmax = data[0]->key;
				cmin = data[size - 1]->key;
			}
		}
		MMNode* splitLeaf() {

			MMNode* right = new MMNode(true);

			for (int i = nsplit, j=0; i < N; i++) {
				right->data[j++] = data[i];
			}

			// update sizes to match moved keys
			size = nsplit;
			right->size = N - nsplit;
			updateMinMaxLeaf();
			right->updateMinMaxLeaf();

			if (parent == nullptr) {
				MMNode* newRoot = new MMNode(false);
				newRoot->insertNode(this);
				newRoot->insertNode(right);
				return parent;
			}
			else
				parent->insertNode(right);
		}


		MMNode* insertNode(MMNode* node) {

			node->parent = this;

			if (size == 0 || node->cmin > children[size-1]->cmax) {
				children[size] = node;
				node->childId = size;
				size++;
				updateMinMaxNode();
			}

			//find index where key is less, insert into index and shift to right
			int i = 0;
			for (; i < size; i++) {
				if (node->cmin < children[i]->cmin) {
					mmShiftRight<MMNode*>(children, i, size);
					children[i] = node;
					node->childId = i;
					size++;
					if (i == 0)
						updateMinMaxNode();
					break;
				}
			}

			//reached capacity, split the node
			if (size == N) {
				return splitNode();
			}
			return nullptr;

		}

		void updateMinMaxNode() {
			if (size <= 0) return;
			//from -infinite to +infinite
			if (ASC) {
				cmin = children[0]->cmin;
				if (children[size - 1]->cmax > 10000) {
					int test = 1;
				}
				cmax = children[size - 1]->cmax;
			}
			// from +infinite to -infinite
			else {
				cmax = children[0]->cmin;
				cmin = children[size - 1]->cmax;
			}
		}

		MMNode* splitNode() {

			MMNode* right = new MMNode(false);

			int j = 0;
			for (int i = nsplit; i < N; i++) {
				right->children[j] = children[i];
				right->children[j]->childId = j;
				j++;
			}

			// update sizes to match moved keys
			size = nsplit;
			right->size = N - nsplit;
			updateMinMaxNode();
			right->updateMinMaxNode();

			if (parent == nullptr) {
				MMNode* newRoot = new MMNode(false);
				newRoot->insertNode(this);
				newRoot->insertNode(right);

				newRoot->updateMinMaxNode();
				return newRoot;
			}
			else 
				parent->insertNode(right);
		}

		template <typename T>
		void mmShiftRight(T* arr, int i, unsigned int& size) {
			for (int idx = size - 1; idx >= i; idx--) {
				arr[idx + 1] = arr[idx];
			}
		}
	};

	MMNode* root = nullptr;

	//bool isAscending = false;
	typedef K (RecursiveMinMax<K, V, N, ASC>::*sortFuncType)(K, K);
	sortFuncType sortFunc;

	RecursiveMinMax()  {

		if (ASC) {
			sortFunc = &RecursiveMinMax<K,V,N, ASC>::ascSort;
		}
		else {
			sortFunc = &RecursiveMinMax<K, V, N, ASC>::descSort;
		}

		root = new MMNode(true);
	}

	K ascSort(K a, K b) {
		return a <= b;
	}
	K descSort(K a, K b) {
		return a >= b;
	}


	MMNode* search(K key) {
		
		MMNode* found = root->search(key);

		return found;
	}

	MMNode* searchBestNode(K key) {
		MMNode* found = root->searchBestNode(key);

		return found;
	}


	MMNode* insert(K key, V value) {
		//MMTreeNode* current = nodes.search(key);
		
		//First insert overall, cannot occur again
		
		MMNode *newRoot = root->insert(key, value);

		if (newRoot) {
			root = newRoot;
		}

		return newRoot;
	}

	


	K mmClamp(K key, K cmin, K cmax) {
		return max(min(key, cmax), cmin);
	}

	


};