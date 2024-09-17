
template <typename K, typename V, unsigned int N, bool ASC>
class MinMaxKey {
public:
	K key;
	V data;
	class MinMaxKey(K k, V d) : key(k), data(d) {}
};


template <typename K, typename V, unsigned int N, bool ASC>
class MinMaxNodeSerializer {
public:

	typedef MinMaxNodeSerializer<K, V, N, ASC> MMNode;
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

	virtual ~MinMaxNodeSerializer() {
		delete[] keys;
		delete[] children;
	}

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
		if (index >= 0 && index <= getChildrenCount())
			return children[index];
		return nullptr;
	}
	void setChildren(MMNode** m) { children = m; };
	void setChild(int index, MMNode* m) {
		if (index >= 0 && index <= getChildrenCount())
			children[index] = m;
	}

	MMKey** getKeys() { return keys; };
	MMKey* getKey(int index) {
		if (index >= 0 && index <= getKeyCount())
			return keys[index];
		return nullptr;
	}
	void setKeys(MMKey** m) { keys = m; };
	void setKey(MMKey* m, int index) {
		if (index >= 0 && index <= getKeyCount())
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
};