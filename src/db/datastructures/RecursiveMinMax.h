#pragma once

#include <vector>
#include <algorithm>
#include <utility>
#include <chrono>
#include <cmath>
//#include "db/datastructures/SparseBTree.h"
//#include <functional>


/**
*
* Recursive MinMax
* Stores data using Linked List for sortedset and Skiplist tree 
* 
*/
template <typename K, typename V, unsigned int N, bool ASC>
class RecursiveMinMax {

public:

	class MMKey {
	public:
		K key;
		V value;

		class MMKey(K k, V v) : key(k), value(v) {

		}
	};

	class MMNode {
	public:
		K cmin = 0;
		K cmax = 0;

		//linked list nodes
		MMNode* nextNode = nullptr;
		MMNode* prevNode = nullptr;
		MMNode* headNode = nullptr;

		//minmax skiplist 
		MMNode* parent = nullptr;
		MMNode* root = nullptr;

		//0 = internal
		//1 = leaf
		bool leaf = true;
		MMNode** children;
		MMKey** data;


		unsigned int childId = -1;

		unsigned int tempspan = 0;
		unsigned int span = 0; //count of all previous 
		unsigned int size = 0;
		unsigned int chsize = 0;

		unsigned int nsplit = 0;
		
		


		MMNode(bool isLeaf) : leaf(isLeaf), nsplit( floor(N*0.5) ) {
			//if (isLeaf)
				data = new MMKey * [N];
			//else
				children = new MMNode * [N];
		}

		//merging will require identifying a node that can fit its sibling to the right
		MMNode* merge(MMNode *node) {

		}

		

		MMNode* search(K key, unsigned int count =0) {
			if (chsize > 0) {
				/*if (key >= children[size - 1]->cmax) {
					return children[size - 1]->insert(key, value);
				}*/
				int i;
				//for (i = chsize - 1; i >= 0; i--) {
				for (i = 0; i < chsize; i++) {
					
					if (key >= children[i]->cmin && key <= children[i]->cmax) {
						return children[i]->search(key, count);
					}
					count += children[i]->span;
				}
				if (key >= children[chsize - 1]->cmax)
					return children[chsize - 1]->search(key, count);
				if (key < children[0]->cmin)
					return children[0]->search(key, count);
			}

			tempspan = count;
			return this;
		}


		MMNode *insert(MMNode* node) {
			return insertNode(node);
		}

		MMNode *insert(K key, V value) {

			//if (!leaf) {
			MMNode* node = search(key);

			return node->insertLeaf(key, value);
		}

		

		

		void updateMinMaxLeaf() {
			if (size <= 0) return;
			//from -infinite to +infinite
			if (ASC) {
				cmin = data[0]->key;
				cmax = data[size - 1]->key;

				if( parent != nullptr )
					parent->updateMinMaxNode();
				
			}
			// from +infinite to -infinite
			else {
				cmax = data[0]->key;
				cmin = data[size - 1]->key;
			}
		}
		MMNode* splitLeaf() {

			MMNode* right = new MMNode(true);
			
			for (int i = nsplit, j=0; i < size; i++) {
				
				right->data[j++] = data[i];
			}

			//remove count entirely from parent tree
			MMNode* cur = this;
		
			// update sizes to match moved keys
			size = nsplit;
			right->size = N - nsplit;
			//right->parent = parent;

			//span -= N - nsplit;
			decrementSpan(N - nsplit);

			updateMinMaxLeaf();
			right->updateMinMaxLeaf();
			
			//right->incrementSpan(right->size);
			//incrementSpan(N - nsplit);

			// old one connected with left, is now connected with new right
			MMNode* farRight = nextNode;
			if (farRight != nullptr)
				farRight->prevNode = right;

			// right is now connected in middle between left and far right
			right->nextNode = farRight;
			right->prevNode = this;

			// left is connected to new right
			nextNode = right;

			if (parent == nullptr) {
				//decrementSpan(N - nsplit);

				MMNode* newRoot = new MMNode(false);
				newRoot->insertNode(this);
				newRoot->insertNode(right);

				//incrementSpan(N - nsplit);
				//newRoot->incrementSpan(right->span + span);
				newRoot->incrementSpan(N - nsplit);
				right->incrementSpan(right->size);
			
				return newRoot;
			}
			else {
				parent->insertNode(right);
				//right->span = right->size;
				right->incrementSpan(right->size);
			}
		}

		MMNode* insertLeaf(K key, V value) {
			//first kv or greater than entire list
			if (size == 0 || key >= data[size - 1]->key) {
				data[size] = new MMKey(key, value);
				size++;
				incrementSpan(1);
				updateMinMaxLeaf();
			}

			//find index where key is less, insert into index and shift to right
			int i = 0;
			for (; i < size; i++) {
				if (key < data[i]->key) {
					mmShiftRight<MMKey*>(data, i, size);
					data[i] = new MMKey(key, value);
					size++;
					incrementSpan(1);
					if (i == 0)
						updateMinMaxLeaf();
					break;
				}
			}

			//reached capacity, split the node
			if (size == N) {
				return splitLeaf(); //new root?
			}
			else {
				// old one connected with left, is now connected with new right
				MMNode* farRight = nextNode;
				if (farRight != nullptr)
					farRight->prevNode = this;

				// right is now connected in middle between left and far right
				//farRight->nextNode = farRight;
				//right->prevNode = left->node;

				// left is connected to new right
				//nextNode = right;
			}
			return nullptr;
		}

		MMNode* insertNode(MMNode* node) {

			node->parent = this;

			if (chsize == 0 || node->cmin > children[chsize-1]->cmax) {
				children[chsize] = node;
				
				node->childId = chsize++;
				//size++;
				updateMinMaxNode();
			}
			else {
			//find index where key is less, insert into index and shift to right
			for (int i = chsize -1; i >= 0; i--) {

				if ((i > 0 && node->cmin > children[i-1]->cmax) && node->cmin <= children[i]->cmax) {
					mmShiftRight<MMNode*>(children, i, chsize);
					children[i] = node;
					node->childId = i;
					chsize++;
					updateMinMaxNode();
					break;
				}

				//if (node->cmin > children[i]->cmin) {

				//	if (node->cmin > children[i]->cmax) {
				//		mmShiftRight<MMNode*>(children, i+1, chsize);
				//		children[i+1] = node;
				//		node->childId = i+1;
				//		
				//	}
				//	else {
				//		mmShiftRight<MMNode*>(children, i, chsize);
				//		children[i] = node;
				//		node->childId = i;
				//	}
				//	//size++;
				//	chsize++;
				//	updateMinMaxNode();
				//	break;
				//}
			}

			}

			//reached capacity, split the node
			if (chsize == N) {
				return splitNode();
			}
			return nullptr;

		}

		void updateMinMaxNode() {
			if (chsize <= 0) return;
			//from -infinite to +infinite
			if (ASC) {
				cmin = children[0]->cmin;
				cmax = children[chsize - 1]->cmax;

				//MMNode* cur = parent;
				//while (cur != nullptr) {
				if( parent != nullptr )
					parent->updateMinMaxNode();
					//cur = cur->parent;
				//}
			}
			// from +infinite to -infinite
			else {
				cmax = children[0]->cmin;
				cmin = children[chsize - 1]->cmax;
			}
		}

		MMNode* splitNode() {

			MMNode* right = new MMNode(false);
			unsigned int spanMove = 0;
			int j = 0;
			for (int i = nsplit; i < chsize; i++) {
				right->children[j] = children[i];
				right->children[j]->childId = j;
				right->children[j]->parent = right;
				spanMove += children[i]->span;
				j++;
			}

			// update sizes to match moved keys
			chsize = nsplit;
			right->chsize = N - nsplit;

			decrementSpan(spanMove);

			//right->parent = parent;
			updateMinMaxNode();
			right->updateMinMaxNode();

			//incrementSpan(size);
			//right->incrementSpan(size);

			if (parent == nullptr) {
				MMNode* newRoot = new MMNode(false);
				newRoot->insertNode(this);
				newRoot->insertNode(right);

				//newRoot->incrementSpan(right->span + span);

				right->incrementSpan(spanMove);
				newRoot->incrementSpan(span);
				//incrementSpan(size);
				//right->incrementSpan(right->size);
				//newRoot->updateMinMaxNode();
				return newRoot;
			}
			else {
				parent->insertNode(right);
				//parent->decrementSpan(spanMove);

				right->incrementSpan(spanMove);
				
				//right->incrementSpan(right->span);
				//right->incrementSpan(right->span);
			}
			
		}

		void incrementSpan(unsigned int count) {
			span += count;
			if (parent != nullptr)
				parent->incrementSpan(count);
		}

		void decrementSpan(unsigned int count) {
			if (count > span) span = 0;
			else span -= count;
			if (parent != nullptr)
				parent->decrementSpan(count);
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

	void display(MMNode *next, int depth=0) {

		if (next == nullptr) {
			next = root;
		}

			
			cout << std::string(depth, '\t') << "Child = " << next->cmin << "," << next->cmax << " :: " << next->span;
			if (next->nextNode != nullptr) {
				//cout << " :: " << next->nextNode->cmin;
			}
			if (next->parent != nullptr) {
				//cout  << " :: Parent = " << next->parent->cmin << "," << next->parent->cmax;
			}
			cout << endl;
			for (int i = 0; i < next->size; i++) {
				cout << std::string(depth, '\t') << "\tKey[" << i << "] = " << next->data[i]->key << endl;
			}
			for (int i = 0; i < next->chsize; i++) {
				//if (next->leaf) {
					//cout << std::string(depth, '\t') << "\tKey["<<i<<"] = " << next->data[i]->key << endl;
				//}
				//else {
				//if( i < next->chsize )
					display(next->children[i], depth+1);
				//}
					
				
			}

	}

	std::vector<std::tuple<unsigned int, K, V>> range(K key, unsigned int count, int offset = 0) {

		MMNode* nearest = root->search(key);
		std::vector<std::tuple<unsigned int, K, V>> ranks;
		ranks.reserve(count);
		if (nearest == nullptr) return ranks;

		int i = 0;
		for (; i < nearest->size; i++) {
			if (key < nearest->data[i]->key) {
				break;
			}
		}

		int absOffset = abs(offset);
		int startPos = i;
		MMNode* cur = nearest;
		while (absOffset > 0 && cur != nullptr) {

			if (offset > 0)
				startPos++;
			else if (offset < 0)
				startPos--;
			absOffset--;

			

			//start at end of previous node
			if (startPos < 0) {
				cur = cur->prevNode;
				if (cur != nullptr)
					startPos = cur->size - 1;
				else
					break;
			}

			//start at beggining of next node
			if (startPos >= cur->size) {
				cur = cur->nextNode;
				if (cur != nullptr)
					startPos = 0;
				else break;
			}
		}

		MMNode* node = cur;
		int rankPos = cur->tempspan + offset;
		while (node != nullptr && ranks.size() < count) {
			for (int i = startPos; i < node->size; i++) {
				K k = node->data[i]->key;
				V v = node->data[i]->value;
				if (i > 0) {
					K prevK = node->data[i - 1]->key;
					if (prevK != k) {
						rankPos++;
					}
				}
				else {
					rankPos++;
				}
				if (key > node->data[i]->key)//!(this->*sortFunc)(key, node->data[i]->key))
					continue;

				ranks.push_back({ rankPos, k, v });

				if (ranks.size() >= count) break;
			}
			node = node->nextNode;
			startPos = 0;
		}

		return ranks;

	}

	MMNode* search(K key) {
		
		MMNode* found = root->search(key);

		return found;
	}

	/*MMNode* searchBestNode(K key) {
		MMNode* found = root->searchBestNode(key);

		return found;
	}*/


	MMNode* insert(K key, V value) {
		//MMTreeNode* current = nodes.search(key);
		
		//First insert overall, cannot occur again
		
		MMNode *newRoot = root->insert(key, value);

		if (root != nullptr) {
			while (root->parent != nullptr) {
				root = root->parent;
			}
			//root = newRoot;
		}

		return root;
	}

	


	K mmClamp(K key, K cmin, K cmax) {
		return max(min(key, cmax), cmin);
	}

	


};