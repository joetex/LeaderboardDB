#pragma once

#include <vector>
#include <algorithm>
#include <utility>
#include <chrono>
#include <cmath>
#include <unordered_map>
//#include "db/datastructures/SparseBTree.h"
//#include <functional>
//#include "db/datastructures/MinMaxNodeSerializer.h"
//#include "db/datastructures/MemoryPool.h"


class MinMaxKey {
public:
	unsigned long long key;
	unsigned long long data;
	class MinMaxKey(unsigned long long k, unsigned long long d) : key(k), data(d) {}
};


template <unsigned int N>
class MinMaxNode {

	typedef MinMaxNode<N> MMNode;
	typedef MinMaxKey MMKey;

	struct MMRecord {
		bool leaf = true;
		//used by parents to find specific child node
		unsigned long long cmin = 0;
		unsigned long long cmax = 0;

		//size of node array
		unsigned int size = 0;

		//count of all children under this parent
		// used to determine rank position the specific keys
		unsigned int span = 0;

		//for removing a child from parent
		unsigned int childId = -1;


		MMNode** children = nullptr;
		MMKey** keys = nullptr;
		
	} ;

public:
	
	//Record to be encoded into database
	MMRecord* record;

	//------ NOT ENCODED

	//when searching, calculates how many keyvalues are in previous nodes
	unsigned int tempspan = 0;

	//linked list nodes
	MMNode* nextNode = nullptr;
	MMNode* prevNode = nullptr;

	//parent node
	MMNode* parent = nullptr;
	//size of children array
	//unsigned int chsize = 0;

	//point where split occurs
	//unsigned int nsplit = 0;

	//point where split occurs
	static inline unsigned int nsplit = floor(N * 0.5);
	static inline unsigned int nlog2 = 16;// ceil(N / log2(N));
	static inline unsigned int halfN = N - nsplit;

public:

	MinMaxNode(bool isLeaf) {

		record = new MMRecord();


		record->leaf = isLeaf;
		if (isLeaf) {
			record->keys = new MMKey * [N];
		}
		else {
			record->children = new MMNode * [N];
		}
	}

	~MinMaxNode() {
		if (record->leaf) {
			for (int i = 0; i < record->size; i++)
				delete record->keys[i];
			delete[] record->keys;
		}
		else {
			for (int i = 0; i < record->size; i++)
				delete record->children[i];
			delete[] record->children;
		}
	}

	void display(int depth = 0) {

		//cout << "=======================" << endl;
		cout << std::string(depth, '\t') << "Child = " << record->cmin << "," << record->cmax << " :: " << record->span << endl;

		for (int i = 0; i < record->size; i++) {
			cout << std::string(depth, '\t') << "\tKey[" << record->keys[i]->data << "] = " << record->keys[i]->key << endl;
		}

		for (int i = 0; record->leaf && i < record->size; i++) {
			record->children[i]->display(depth + 1);
		}
		//cout << "=======================" << endl;
	}

	//merging will require identifying a node that can fit its sibling to the right
	MinMaxNode* merge(MinMaxNode* node) {

	}



	MinMaxNode* revsearch(unsigned long long key) {
		MMNode* child = nullptr;
		if (record->leaf) {
			return this;
		}

		int i = 0;
		child = record->children[0];
		if (key <= child->record->cmax) {
			return child->revsearch(key);
		}

		/*for (; i < childCount; i+= nlog2)
			if (key <= children[i]->cmax) break;
		i = mmClamp(i - nlog2, 0, childCount - 1);*/
		for (; i < record->size; i++) {
			child = record->children[i];
			if (key <= child->record->cmax)
				break;
		}
			
		if (i >= record->size) i = record->size - 1;
		return child->revsearch(key);
	}

	MinMaxNode* revsearchWithCount(unsigned long long key, unsigned int count = 0) {
		MMNode* child = nullptr;
		if (record->leaf) {
			tempspan = count;
			//tempspan = (count + 1);
			return this;
		}

		int i = 0;

		child = record->children[0];
		if (key <= child->record->cmax) {
			return child->revsearchWithCount(key, count);
		}

		for (; i < record->size; i++) {
			child = record->children[i];
			if (key <= child->record->cmax)
				break;
			count += child->record->span;
		}
			
		if (i >= record->size) i = record->size - 1;
		return child->revsearchWithCount(key, count);
	}

	MinMaxNode* search(unsigned long long key) {
		MMNode* child = nullptr;
		if (record->leaf)
			return this;

		child = record->children[0];
		if (key < child->record->cmin) {
			return child->search(key);
		}
		int i = record->size - 1;

		/*for (; i >= 0; i-= nlog2)
			if (key >= children[i]->cmin) break;
		i = mmClamp(i + nlog2, 0, childCount - 1);*/
		for (; i >= 0; i--) {
			child = record->children[i];
			if (key >= child->record->cmin)//&& key <= children[i]->cmax)
				break;
		}
		
		if (i < 0) i = 0;
		return child->search(key);
	}

	MinMaxNode* searchWithCount(unsigned long long key, unsigned int count = 0) {
		MMNode* child = nullptr;
		if (record->leaf) {
			MinMaxNode* root = this;
			while (root->parent != nullptr)
				root = root->parent;

			tempspan = root->record->span - (count + record->size);
			return this;
		}

		child = record->children[0];
		if (key < child->record->cmin) {
			return child->searchWithCount(key, count);
		}
		int i = record->size - 1;
			
		for (; i >= 0; i--) {
			child = record->children[i];
			if (key >= child->record->cmin)//&& key <= children[i]->cmax)
				break;
			count += child->record->span;
		}

		if (i < 0) i = 0;
		return child->searchWithCount(key, count);
	}

	/*MinMaxNode* insert(K key, V data) {
		MinMaxNode* node = nullptr;
		if (ASC) node = search(key);
		else node = revsearch(key);
		return node->insertLeaf(key, data);
	}*/




	MinMaxNode* insertLeaf(unsigned long long key, unsigned long long d, bool ASC=true) {
		unsigned int keyCount = record->size;
		MMKey* kv = new MMKey(key, d);
		MMKey* temp = nullptr;
		//first kv or greater than entire list
		if (keyCount == 0 || key > record->keys[keyCount - 1]->key) {
			record->keys[keyCount] =  kv;
			record->size = keyCount + 1;
			incrementSpan(1);
			updateMinMaxLeaf();
		}
		else {

			//find index where key is less, insert into index and shift to right
			int i = 0;
			int logN = log2(N);

			if (ASC) {
				for (; i < keyCount; i+= logN)
					if (key < record->keys[i]->key) break;
				i = mmClamp(i-logN, 0, keyCount - 1);

				for (; i < keyCount; i++) {
					temp = record->keys[i];
					if (key < temp->key) {
						break;
					}
				}
			}
			else {
				for (; i < keyCount; i += logN)
					if (key <= record->keys[i]->key) break;
				i = mmClamp(i - logN, 0, keyCount - 1);

				for (; i < keyCount; i++) {
					temp = record->keys[i];
					if (key <= temp->key) {
						break;
					}
				}
			}

			mmShiftRight(record->keys, i, keyCount);
			record->keys[i] =  kv;
			record->size = keyCount + 1;
			incrementSpan(1);
			if (i == 0)
				updateMinMaxLeaf();
		}

		//reached capacity, split the node
		if (record->size == N) {
			return splitLeaf(); //new root?
		}
		else {
			// old one connected with left, is now connected with new right
			MinMaxNode* farRight = nextNode;
			if (farRight != nullptr)
				farRight->prevNode = this;
		}
		return nullptr;
	}

	inline int mmClamp(int key, int cmin, int cmax) {
		return max(min(key, cmax), cmin);
	}

	MinMaxNode* insertNode(MinMaxNode* node, int atIndex = -1) {
		if (atIndex == -1)
			return nullptr;

		node->parent = this;

		unsigned int childCount = record->size;
		mmShiftRightNode(record->children, atIndex, childCount);
		record->children[atIndex] =  node;
		node->record->childId = atIndex;
		childCount += 1;
		record->size = childCount;
		//children[atIndex] = node;
		//node->childId = atIndex;
		//size++;
		updateMinMaxNode();

		//reached capacity, split the node
		if (childCount == N)
			return splitNode();

		return nullptr;
	}

	MinMaxNode* splitLeaf() {
		MinMaxNode* right = new MinMaxNode(true);
		unsigned int keyCount = record->size;
		for (int i = nsplit, j = 0; i < keyCount; i++) {
			right->record->keys[j++] = record->keys[i];
		}

		//remove count entirely from parent tree
		MinMaxNode* cur = this;

		// update sizes to match moved keys
		record->size = nsplit;
		right->record->size = N - nsplit;
		decrementSpan(N - nsplit);

		updateMinMaxLeaf();
		right->updateMinMaxLeaf();

		// old one connected with left, is now connected with new right
		MinMaxNode* farRight = nextNode;
		if (farRight != nullptr)
			farRight->prevNode = right;

		// right is now connected in middle between left and far right
		right->nextNode = farRight;
		right->prevNode = this;

		// left is connected to new right
		nextNode = right;

		MinMaxNode* p = parent;

		if (p == nullptr) {
			MinMaxNode* newRoot = new MinMaxNode(false);
			newRoot->insertNode(this, 0);
			newRoot->insertNode(right, 1);
			newRoot->incrementSpan(N - nsplit);
			right->incrementSpan(right->record->size);

			return newRoot;
		}
		else {
			p->insertNode(right, record->childId + 1);
			right->incrementSpan(right->record->size);
		}
	}


	MinMaxNode* splitNode() {
		MinMaxNode* right = new MMNode(false);
		MinMaxNode* child = nullptr;
		unsigned int spanMove = 0;
		unsigned int childCount = record->size;
		int j = 0;
		for (int i = nsplit; i < childCount; i++) {
			child = record->children[i];
			child->record->childId = j;
			child->parent = right;
			spanMove += child->record->span;
			right->record->children[j] =  child;
			j++;
		}

		// update sizes to match moved keys
		record->size = nsplit;
		//size = nsplit;
		right->record->size = N - nsplit;

		decrementSpan(spanMove);

		updateMinMaxNode();
		right->updateMinMaxNode();


		MinMaxNode* p = parent;

		if (p == nullptr) {
			MinMaxNode* newRoot = new MMNode(false);
			newRoot->insertNode(this, 0);
			newRoot->insertNode(right, 1);
			right->incrementSpan(spanMove);
			newRoot->incrementSpan(record->span);
			return newRoot;
		}
		else {
			p->insertNode(right, record->childId + 1);
			right->incrementSpan(spanMove);
		}
	}

	MinMaxNode* remove(unsigned long long key, unsigned long long d) {

		MinMaxNode* nearest = search(key);
		MMKey* kv = nullptr;

		int i = 0;
		unsigned int keyCount = nearest->record->size;
		for (; i < keyCount; i++) {
			kv = nearest->record->keys[i];
			if (d == kv->data) {
				break;
			}
		}

		if (kv == nullptr) {
			return nullptr;
		}

		delete kv;
		if (keyCount > 1) {
			MMKey** nearestKeys = nearest->getKeys();
			mmShiftLeft(nearestKeys, i, keyCount);
		}
		keyCount -= 1;
		nearest->record->size = keyCount;
		//nearest->size--;
		nearest->decrementSpan(1);

		if (keyCount == 0) {
			//leaf has no children, update the linked list pointers for leaf nodes
			MinMaxNode* farRight = nearest->nextNode;
			MinMaxNode* left = nearest->prevNode;

			if (farRight != nullptr) {
				if (left != nullptr) {
					farRight->prevNode = left;
					left->nextNode = farRight;
				}
				else {
					farRight->prevNode = nullptr;
				}
			}
			else if (left != nullptr) {
				left->nextNode = nullptr;
			}

			//remove this node from tree recursively incase parent is also becomes empty
			removeNode(nearest);
		}
		else {
			nearest->updateMinMaxLeaf();
		}
	}

	MinMaxNode* removeNode(MinMaxNode* node) {
		MinMaxNode* p = node->parent;
		if (p == nullptr)
			return nullptr;

		unsigned int pCount = p->record->size;
		if (pCount > 1) {
			MinMaxNode** parentChildren = p->getChildren();
			mmShiftLeftNode(parentChildren, node->childId, pCount);
		}
		//p->size--;
		pCount -= 1;
		p->record->size = pCount;
		delete node;

		if (pCount == 0 && p->parent != nullptr) {
			removeNode(p);
		}
		else {
			p->updateMinMaxNode();
		}
	}

	void updateMinMaxLeaf() {
		if (record->size <= 0) return;
		//from -infinite to +infinite
		record->cmin = record->keys[0]->key;
		record->cmax = record->keys[record->size - 1]->key;

		if (parent != nullptr)
			parent->updateMinMaxNode();
	}

	void updateMinMaxNode() {
		if (record->size <= 0) return;
		//from -infinite to +infinite
		record->cmin = (record->children[0]->record->cmin);
		record->cmax = (record->children[record->size - 1]->record->cmax);
		if (parent != nullptr)
			parent->updateMinMaxNode();
	}

	void incrementSpan(unsigned int count) {
		record->span = record->span + count;
		//span += count;
		if (parent != nullptr)
			parent->incrementSpan(count);
	}

	void decrementSpan(unsigned int count) {
		if (count > record->span) record->span = 0;
		else record->span = record->span - count;
		if (parent != nullptr)
			parent->decrementSpan(count);
	}

	void mmShiftRight(MMKey** arr, int i, unsigned int size) {
		for (int idx = size - 1; idx >= i; idx--) {
			arr[idx + 1] = arr[idx];
		}
	}

	void mmShiftLeft(MMKey** arr, int i, unsigned int size) {
		for (; i < size - 1; i++) {
			arr[i] = arr[i + 1];
		}
	}

	void mmShiftRightNode(MinMaxNode** arr, int i, unsigned int size) {
		for (int idx = size - 1; idx >= i; idx--) {
			arr[idx]->record->childId = idx + 1;
			arr[idx + 1] = arr[idx];
		}
	}

	void mmShiftLeftNode(MinMaxNode** arr, int i, unsigned int size) {
		for (; i < size - 1; i++) {
			arr[i + 1]->record->childId = i;
			arr[i] = arr[i + 1];
		}
	}

};
