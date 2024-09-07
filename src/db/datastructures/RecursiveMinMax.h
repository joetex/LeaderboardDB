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
			decrementSpan(N - nsplit);

			updateMinMaxLeaf();
			right->updateMinMaxLeaf();

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
				MMNode* newRoot = new MMNode(false);
				newRoot->insertNode(this);
				newRoot->insertNode(right);
				newRoot->incrementSpan(N - nsplit);
				right->incrementSpan(right->size);
			
				return newRoot;
			}
			else {
				parent->insertNode(right);
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
			else {

				//find index where key is less, insert into index and shift to right
				int i = 0;
				bool done = false;
				int ilog = log(N/2) / log(2);

				//skip through the list in intervals of log2(size)
				//backtrack once when the key is found and loop forward again to find the actual placement
				for (; i < size; i += ilog) {
					if (key < data[i]->key) {
						i -= ilog;
						break;
					}
					if (i + ilog >= size) {
						ilog = 1;
					}
				}

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
			}
			return nullptr;
		}

		MMNode* insertNode(MMNode* node) {

			node->parent = this;

			if (chsize == 0 || node->cmin > children[chsize-1]->cmax) {
				children[chsize] = node;
				node->childId = chsize++;
				updateMinMaxNode();
			}
			else {
				//find index where key is less, insert into index and shift to right
				for (int i = chsize -1; i >= 0; i--) {

					if ((i > 0 && node->cmin > children[i-1]->cmax) && node->cmin <= children[i]->cmax) {
						mmShiftRight<MMNode*>(children, i, chsize);
						children[i] = node;
						//node->childId = i;
						chsize++;
						updateMinMaxNode();
						break;
					}
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
				if( parent != nullptr )
					parent->updateMinMaxNode();
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
				//right->children[j]->childId = j;
				right->children[j]->parent = right;
				spanMove += children[i]->span;
				j++;
			}

			// update sizes to match moved keys
			chsize = nsplit;
			right->chsize = N - nsplit;

			decrementSpan(spanMove);

			updateMinMaxNode();
			right->updateMinMaxNode();

			if (parent == nullptr) {
				MMNode* newRoot = new MMNode(false);
				newRoot->insertNode(this);
				newRoot->insertNode(right);
				right->incrementSpan(spanMove);
				newRoot->incrementSpan(span);
				return newRoot;
			}
			else {
				parent->insertNode(right);
				right->incrementSpan(spanMove);
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

	int display(MMNode *next = nullptr, int maxDepth = -1, int depth=0) {

		if (next == nullptr) {
			next = root;
		}

		if (maxDepth <= -1 || depth <= maxDepth)
			cout << std::string(depth, '\t') << "Child = " << next->cmin << "," << next->cmax << " :: " << next->span << endl;

		if (next->nextNode != nullptr) {
			//cout << " :: " << next->nextNode->cmin;
		}
		if (next->parent != nullptr) {
			//cout  << " :: Parent = " << next->parent->cmin << "," << next->parent->cmax;
		}
		for (int i = 0; i < next->size; i++) {
			cout << std::string(depth, '\t') << "\tKey[" << i << "] = " << next->data[i]->key << endl;
		}
		int highestDepth = 0;
		for (int i = 0; i < next->chsize; i++) {
			//if (next->leaf) {
				//cout << std::string(depth, '\t') << "\tKey["<<i<<"] = " << next->data[i]->key << endl;
			//}
			//else {
			//if( i < next->chsize )
				int d = display(next->children[i], maxDepth, depth+1);
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

		MMNode* nearest = root->search(key);
		std::vector<std::tuple<unsigned int, K, V>> ranks;
		ranks.reserve(count);
		if (nearest == nullptr) return ranks;

		int i = nearest->size-1;
		for (; i >= 0; i--) {
			if (key >= nearest->data[i]->key)
				break;
		}

		unsigned int maxCount = root->span;

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
				if (cur->prevNode != nullptr) {
					cur = cur->prevNode;
					startPos = cur->size - 1;
				}
				else {
					startPos = 0;
					break;
				}
			}

			//start at beggining of next node
			if (startPos >= cur->size) {
				if (cur->nextNode != nullptr) {
					cur = cur->nextNode;
					startPos = 0;
				}
				else {
					startPos = cur->size - 1;
					break;
				}
			}
		}

		int rankPos = (maxCount - (nearest->tempspan + (i + 1))) + offset;
		offsetKey = cur->data[startPos]->key;


		while (cur != nullptr && ranks.size() < count) {
			for (int i = startPos; i >= 0; i--) {
				K k = cur->data[i]->key;
				V v = cur->data[i]->value;
				if (i < cur->size-1) {
					K prevK = cur->data[i + 1]->key;
					if (prevK != k) {
						rankPos++;
					}
				}
				else {
					rankPos++;
				}
				if (cur->data[i]->key > offsetKey)//!(this->*sortFunc)(key, node->data[i]->key))
					continue;

				ranks.push_back({ rankPos, k, v });

				if (ranks.size() >= count) break;
			}
			cur = cur->prevNode;
			if( cur != nullptr )
				startPos = cur->size - 1;
		}

		return ranks;

	}

	// pull the rankings in ASC order, so rank 1 is lowest number 
	std::vector<std::tuple<unsigned int, K, V>> range(K key, unsigned int count, int offset = 0) {

		MMNode* nearest = root->search(key);
		std::vector<std::tuple<unsigned int, K, V>> ranks;
		ranks.reserve(count);
		if (nearest == nullptr) return ranks;

		int i = 0;
		for (; i < nearest->size; i++) {
			if (key < nearest->data[i]->key)
				break;
		}

		K offsetKey = key;
		int absOffset = abs(offset);
		int startPos = i - 1;
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
					startPos = cur->size - 1;
				}
				else {
					startPos = 0;
					break;
				}
			}

			//start at beggining of next node
			if (startPos >= cur->size) {
				if (cur->nextNode != nullptr) {
					cur = cur->nextNode;
					startPos = 0;
				}
				else {
					startPos = cur->size - 1;
					break;
				}
			}
		}

		int rankPos = nearest->tempspan + (i-1) + offset;
		offsetKey = cur->data[startPos]->key;
	

		while (cur != nullptr && ranks.size() < count) {
			for (int i = startPos; i < cur->size; i++) {
				K k = cur->data[i]->key;
				V v = cur->data[i]->value;
				if (i > 0) {
					K prevK = cur->data[i - 1]->key;
					if (prevK != k) {
						rankPos++;
					}
				}
				else {
					rankPos++;
				}
				if (offsetKey > cur->data[i]->key)//!(this->*sortFunc)(key, node->data[i]->key))
					continue;

				ranks.push_back({ rankPos, k, v });

				if (ranks.size() >= count) break;
			}
			cur = cur->nextNode;
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