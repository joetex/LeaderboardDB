#pragma once

#include <vector>
#include <algorithm>
#include <utility>
#include <chrono>
#include <cmath>
//#include "db/datastructures/SparseBTree.h"
//#include <functional>
//#include "db/datastructures/MinMaxNodeSerializer.h"


template <typename K, typename V, unsigned int N, bool ASC>
class MinMaxKey {
public:
	K key;
	V data;
	class MinMaxKey(K k, V d) : key(k), data(d) {}
};


template <typename K, typename V, unsigned int N, bool ASC>
class MinMaxNode {

	typedef MinMaxNode<K, V, N, ASC> MMNode;
	typedef MinMaxKey<K, V, N, ASC> MMKey;

	//used by parents to find specific child node
	K cmin = 0;
	K cmax = 0;

	//count of all children under this parent
	// used to determine rank position the specific keys
	unsigned int span = 0;

	//linked list nodes
	MMNode* nextNode = nullptr;
	MMNode* prevNode = nullptr;

	//parent node
	MMNode* parent = nullptr;

	//0 = internal
	//1 = leaf
	bool leaf = true;
	MMNode** children = nullptr;
	MMKey** keys = nullptr;

	//for removing a child from parent
	unsigned int childId = -1;

	//when searching, calculates how many keyvalues are in previous nodes
	unsigned int tempspan = 0;

	//size of data array
	unsigned int size = 0;

	//size of children array
	unsigned int chsize = 0;

	//point where split occurs
	unsigned int nsplit = 0;

public:
	K getMin() { return cmin; };
	K getMax() { return cmax; };
	void setMin(K m) { cmin = m; };
	void setMax(K m) { cmax = m; };

	unsigned int getSpan() { return span; };
	void setSpan(unsigned int m) { span = m; };

	MMNode* getNext() { return nextNode; };
	MMNode* getPrev() { return prevNode; };
	void setNext(MMNode* m) { nextNode = m; };
	void setPrev(MMNode* m) { prevNode = m; };


	MMNode* getParent() { return parent; };
	void setParent(MMNode* m) { parent = m; };

	bool isLeaf() { return leaf; };
	void setLeaf(bool l) { leaf = l; };

	MMNode** getChildren() { return children; };
	MMNode* getChild(int index) {
		if (index >= 0 && index < N)
			return children[index];
		return nullptr;
	}
	void setChildren(MMNode** m) { children = m; };
	void setChild(int index, MMNode* m) {
		if (index >= 0 && index < N)
			children[index] = m;
	}

	MMKey** getKeys() { return keys; };
	MMKey* getKey(int index) {
		if (index >= 0 && index < N)
			return keys[index];
		return nullptr;
	}
	void setKeys(MMKey** m) { keys = m; };
	void setKey(int index, MMKey* m) {
		if (index >= 0 && index < N)
			keys[index] = m;
	}

	unsigned int getChildId() { return childId; };
	void setChildId(unsigned int m) { childId = m; };

	unsigned int getTempSpan() { return tempspan; };
	void setTempSpan(unsigned int m) { tempspan = m; };

	unsigned int getKeyCount() { return size; };
	void setKeyCount(unsigned int m) { size = m; };

	unsigned int getChildrenCount() { return chsize; };
	void setChildrenCount(unsigned int m) { chsize = m; };

	unsigned int getNSplit() { return nsplit; };
	void setNSplit(unsigned int m) { nsplit = m; };


	MinMaxNode(bool isLeaf) : leaf(isLeaf), nsplit(floor(N * 0.5)) {
		if (isLeaf) {
			keys = new MMKey * [N];
		}
		else {
			children = new MMNode * [N];
		}
	}

	~MinMaxNode() {

		delete[] keys;
		delete[] children;
	}

	void display(int depth = 0) {

		//cout << "=======================" << endl;
		cout << std::string(depth, '\t') << "Child = " << getMin() << "," << getMax() << " :: " << getSpan() << endl;

		for (int i = 0; i < getKeyCount(); i++) {
			cout << std::string(depth, '\t') << "\tKey[" << getKey(i)->data << "] = " << getKey(i)->key << endl;
		}

		for (int i = 0; i < getChildrenCount(); i++) {
			getChild(i)->display(depth + 1);
		}
		//cout << "=======================" << endl;
	}

	//merging will require identifying a node that can fit its sibling to the right
	MinMaxNode* merge(MinMaxNode* node) {

	}



	MinMaxNode* revsearch(K key, unsigned int count = 0) {
		int childCount = getChildrenCount();
		MMNode* child = nullptr;
		if (childCount > 0) {
			int i = 0;
			for (; i < childCount; i++) {
				child = getChild(i);
				if (key <= child->getMax())
					break;
				count += child->getSpan();
			}
			if (i >= childCount) i = childCount - 1;
			return child->revsearch(key, count);
		}

		setTempSpan(count + 1);
		//tempspan = (count + 1);
		return this;
	}

	MinMaxNode* search(K key, unsigned int count = 0) {
		int childCount = getChildrenCount();
		MMNode* child = nullptr;
		if (childCount > 0) {
			child = getChild(0);
			if (key < child->getMin()) {
				return child->search(key, count);
			}
			int i = childCount - 1;
			for (; i >= 0; i--) {
				child = getChild(i);
				if (key >= child->getMin())//&& key <= children[i]->cmax)
					break;
				count += child->getSpan();
			}
			if (i < 0) i = 0;
			return child->search(key, count);
		}

		MinMaxNode* root = this;
		while (root->getParent() != nullptr)
			root = root->getParent();

		setTempSpan(root->getSpan() - (count + getKeyCount()));
		return this;
	}

	MinMaxNode* insert(K key, V data) {
		MinMaxNode* node = nullptr;
		if (ASC) node = search(key);
		else node = revsearch(key);
		return node->insertLeaf(key, data);
	}




	MinMaxNode* insertLeaf(K key, V d) {
		unsigned int keyCount = getKeyCount();
		MMKey* kv = new MMKey(key, d);
		MMKey* temp = nullptr;
		//first kv or greater than entire list
		if (keyCount == 0 || key > getKey(keyCount - 1)->key) {
			setKey(keyCount, kv);
			setKeyCount(keyCount + 1);
			incrementSpan(1);
			updateMinMaxLeaf();
		}
		else {

			//find index where key is less, insert into index and shift to right
			int i = 0;
			if (ASC) {
				for (; i < keyCount; i++) {
					temp = getKey(i);
					if (key < temp->key) {
						break;
					}
				}
			}
			else {
				for (; i < keyCount; i++) {
					temp = getKey(i);
					if (key <= temp->key) {
						break;
					}
				}
			}

			mmShiftRight(keys, i, keyCount);
			setKey(i, kv);
			setKeyCount(keyCount + 1);
			incrementSpan(1);
			if (i == 0)
				updateMinMaxLeaf();
		}

		//reached capacity, split the node
		if (getKeyCount() == N) {
			return splitLeaf(); //new root?
		}
		else {
			// old one connected with left, is now connected with new right
			MinMaxNode* farRight = getNext();
			if (farRight != nullptr)
				farRight->setPrev(this);
		}
		return nullptr;
	}

	MinMaxNode* insertNode(MinMaxNode* node, int atIndex = -1) {
		if (atIndex == -1)
			return nullptr;

		node->setParent(this);

		unsigned int childCount = getChildrenCount();
		mmShiftRightNode(children, atIndex, childCount);
		setChild(atIndex, node);
		node->setChildId(atIndex);
		childCount += 1;
		setChildrenCount(childCount);
		//children[atIndex] = node;
		//node->childId = atIndex;
		//chsize++;
		updateMinMaxNode();

		//reached capacity, split the node
		if (childCount == N)
			return splitNode();

		return nullptr;
	}

	MinMaxNode* splitLeaf() {
		MinMaxNode* right = new MinMaxNode(true);
		unsigned int keyCount = getKeyCount();
		for (int i = nsplit, j = 0; i < keyCount; i++) {
			right->setKey(j++, getKey(i));
		}

		//remove count entirely from parent tree
		MinMaxNode* cur = this;

		// update sizes to match moved keys
		setKeyCount(nsplit);
		right->setKeyCount(N - nsplit);
		decrementSpan(N - nsplit);

		updateMinMaxLeaf();
		right->updateMinMaxLeaf();

		// old one connected with left, is now connected with new right
		MinMaxNode* farRight = getNext();
		if (farRight != nullptr)
			farRight->setPrev(right);

		// right is now connected in middle between left and far right
		right->setNext(farRight);
		right->setPrev(this);

		// left is connected to new right
		setNext(right);

		MinMaxNode* p = getParent();

		if (p == nullptr) {
			MinMaxNode* newRoot = new MinMaxNode(false);
			newRoot->insertNode(this, 0);
			newRoot->insertNode(right, 1);
			newRoot->incrementSpan(N - nsplit);
			right->incrementSpan(right->getKeyCount());

			return newRoot;
		}
		else {
			p->insertNode(right, getChildId() + 1);
			right->incrementSpan(right->getKeyCount());
		}
	}


	MinMaxNode* splitNode() {
		MinMaxNode* right = new MMNode(false);
		MinMaxNode* child = nullptr;
		unsigned int spanMove = 0;
		unsigned int childCount = getChildrenCount();
		int j = 0;
		for (int i = getNSplit(); i < childCount; i++) {
			child = getChild(i);
			child->setChildId(j);
			child->setParent(right);
			spanMove += child->getSpan();
			right->setChild(j, child);
			j++;
		}

		// update sizes to match moved keys
		setChildrenCount(nsplit);
		//chsize = nsplit;
		right->setChildrenCount(N - nsplit);

		decrementSpan(spanMove);

		updateMinMaxNode();
		right->updateMinMaxNode();


		MinMaxNode* p = getParent();

		if (p == nullptr) {
			MinMaxNode* newRoot = new MMNode(false);
			newRoot->insertNode(this, 0);
			newRoot->insertNode(right, 1);
			right->incrementSpan(spanMove);
			newRoot->incrementSpan(getSpan());
			return newRoot;
		}
		else {
			p->insertNode(right, getChildId() + 1);
			right->incrementSpan(spanMove);
		}
	}

	MinMaxNode* remove(K key, V d) {

		MinMaxNode* nearest = search(key);
		MMKey* kv = nullptr;

		int i = 0;
		unsigned int keyCount = nearest->getKeyCount();
		for (; i < keyCount; i++) {
			kv = nearest->getKey(i);
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
		nearest->setKeyCount(keyCount);
		//nearest->size--;
		nearest->decrementSpan(1);

		if (keyCount == 0) {
			//leaf has no children, update the linked list pointers for leaf nodes
			MinMaxNode* farRight = nearest->getNext();
			MinMaxNode* left = nearest->getPrev();

			if (farRight != nullptr) {
				if (left != nullptr) {
					farRight->setPrev(left);
					left->setNext(farRight);
				}
				else {
					farRight->setPrev(nullptr);
				}
			}
			else if (left != nullptr) {
				left->setNext(nullptr);
			}

			//remove this node from tree recursively incase parent is also becomes empty
			removeNode(nearest);
		}
		else {
			nearest->updateMinMaxLeaf();
		}
	}

	MinMaxNode* removeNode(MinMaxNode* node) {
		MinMaxNode* p = node->getParent();
		if (p == nullptr)
			return nullptr;

		unsigned int pCount = p->getChildrenCount();
		if (pCount > 1) {
			MinMaxNode** parentChildren = p->getChildren();
			mmShiftLeftNode(parentChildren, node->getChildId(), pCount);
		}
		//p->chsize--;
		pCount -= 1;
		p->setChildrenCount(pCount);
		delete node;

		if (pCount == 0 && p->getParent() != nullptr) {
			removeNode(p);
		}
		else {
			p->updateMinMaxNode();
		}
	}

	void updateMinMaxLeaf() {
		if (getKeyCount() <= 0) return;
		//from -infinite to +infinite
		setMin(getKey(0)->key);
		setMax(getKey(getKeyCount() - 1)->key);

		if (getParent() != nullptr)
			getParent()->updateMinMaxNode();
	}

	void updateMinMaxNode() {
		if (getChildrenCount() <= 0) return;
		//from -infinite to +infinite
		setMin(getChild(0)->getMin());
		setMax(getChild(getChildrenCount() - 1)->getMax());
		if (getParent() != nullptr)
			getParent()->updateMinMaxNode();
	}

	void incrementSpan(unsigned int count) {
		setSpan(getSpan() + count);
		//span += count;
		if (getParent() != nullptr)
			getParent()->incrementSpan(count);
	}

	void decrementSpan(unsigned int count) {
		if (count > getSpan()) setSpan(0);
		else setSpan(getSpan() - count);
		if (getParent() != nullptr)
			getParent()->decrementSpan(count);
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
			arr[idx]->setChildId(idx + 1);
			arr[idx + 1] = arr[idx];
		}
	}

	void mmShiftLeftNode(MinMaxNode** arr, int i, unsigned int size) {
		for (; i < size - 1; i++) {
			arr[i + 1]->setChildId(i);
			arr[i] = arr[i + 1];
		}
	}

};
