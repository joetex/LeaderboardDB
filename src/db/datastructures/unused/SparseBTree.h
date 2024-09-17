#ifndef SPARSEBTREE_HPP
#define SPARSEBTREE_HPP

//int m = 128;
//int min = 63;

//#define INT_MIN –2147483648

    template <typename T>
    void sparseShiftRight(T* arr, int i, int& size) {
        for (int idx = size - 1; idx >= i; idx--) {
            arr[idx + 1] = arr[idx];
        }
        size++;
    }

    template <typename T>
    void sparseShiftLeft(T* arr, int i, int& size) {
        for (; i < size - 1; i++) {
            arr[i] = arr[i + 1];
        }
        size--;
    }

    template <typename T>
    class SparseNode {
    public:
        SparseNode<T>** children;	// array of child pointers
        T** data;       // array of data (has keys)

        int m;                  // tree degree
        int n;                  // current number of keys
        int mmin;                // min no keys = ceil(m / 2) - 1

        unsigned int dataSpan = 0;
        unsigned int childSpan = 0;
        bool leaf;              // true if no children
        SparseNode<T>* parent = nullptr;
        int childId = -1;

        SparseNode(int m, bool leaf = true)
            : m(m), mmin(ceil(m / 2.) - 1), leaf(leaf), n(0) {
            // allocate memory for arrays
            children = new SparseNode<T> *[m + 1];
            data = new T * [m];
        }

        SparseNode<T>* insert(T* element, T*& upshiftElement) {
            /*
                if overflow, return new created child and update reference for new parent
            */

            // find ordered position for element
            int i = 0;
            while (i < n && element->cmax > data[i]->cmax) {
                i++;
            }

            if (leaf) {
                // allocate space and assign
                sparseShiftRight<T*>(data, i, n);
                data[i] = element;
                data[i]->parent = this;
                data[i]->dataId = i;

                if (n == m) {
                    // overflow
                    return split(upshiftElement);
                }
            }
            else {
                // pass down to child
                SparseNode<T>* right = children[i]->insert(element, upshiftElement);
                //children[i]->parent = this;
                //children[i]->childId = i;

                if (right) {
                    // overflow in child, returned min and right branch

                    // set data
                    sparseShiftRight<T*>(data, i, n);
                    data[i] = upshiftElement;
                    data[i]->parent = this;
                    data[i]->dataId = i;
                    //data[i]->incrementParents(upshiftElement->size);

                    // add child
                    int _n = n;
                    sparseShiftRight<SparseNode<T>*>(children, i + 1, _n);
                    children[i + 1] = right;
                    right->parent = this;
                    right->childId = i + 1;

                    if (n == m) {
                        // too many elements
                        return split(upshiftElement);
                    }
                }
            }

            upshiftElement = nullptr;
            return nullptr;
        }

        SparseNode<T>* split(T*& upshiftElement) {
            SparseNode<T>* right = new SparseNode<T>(m, leaf);
            right->parent = parent;

            unsigned int leftSpan = dataSpan;
            unsigned int rightSpan = 0;

            upshiftElement = data[mmin];
            //upshiftElement->decrementParents(upshiftElement->size);
            data[mmin] = nullptr;

            //leftSpan -= upshiftElement->size;

            


            leftSpan -= rightSpan;
            //dataSpan = leftSpan;
            //right->dataSpan = rightSpan;


            // data
            for (int idx = mmin + 1; idx < n; idx++) {
                //remove from left side tree
                //data[idx]->decrementParents(data[idx]->size);

                //update references
                data[idx]->parent = right;
                data[idx]->dataId = idx;
                right->data[idx - mmin - 1] = data[idx];

                //add size to right side tree
                //data[idx]->incrementParents(data[idx]->size);
                data[idx] = nullptr;

                right->n++;
            }
            n -= right->n + 1;  // +1 to account for divisor

            


            //dataSpan = leftSpan;
            //right->dataSpan = rightSpan;

            return right;
        }

        T* search(int key) {
            int i = 0;
            if (n == 0) {
                return nullptr;
            }
            for (; i < n; i++) {
                
                if (key >= data[i]->cmin && key <= data[i]->cmax) {
                    return data[i];
                }
                if (!leaf && key < data[i]->cmin) {
                    return children[i]->search(key);
                }
            }

            // if key is greater than largest max
            //   return the node with highest max value
            if (leaf) {
                return data[i - 1];
            }
            return children[i]->search(key);
        }


        void traverse(int key, void(itemViewer)(T* data)) {
            for (int i = 0; i < n; i++) {
                if (key < data[i]->cmin) {
                    return;
                }
                if (!leaf) {
                    // has children
                    children[i]->traverse(itemViewer);
                }
                itemViewer(data[i]);
            }
            if (!leaf) {
                // get rightmost child
                children[n]->traverse(itemViewer);
            }
        }

        void traverse(void(itemViewer)(T* data)) {
            for (int i = 0; i < n; i++) {
                if (!leaf) {
                    // has children
                    children[i]->traverse(itemViewer);
                }
                itemViewer(data[i]);
            }
            if (!leaf) {
                // get rightmost child
                children[n]->traverse(itemViewer);
            }
        }

        /*
            return vals
            -1: child not found
            0: successful deletion
            1: cannot delete because violated minimum condition
        */
        int erase(int cmin, int cmax) {
            // find location
            bool found = false;
            int i;
            for (i = 0; i < n && cmax >= data[i]->cmax; i++) {
                if (cmin >= data[i]->cmin && cmax == data[i]->cmax) {
                    // found element
                    found = true;
                    break;
                }
            }

            if (found) {
                // key in data

                if (leaf) {
                    // remove item
                    data[i] = nullptr;
                    sparseShiftLeft<T*>(data, i, n);
                }
                else {
                    // left child: children[i], right child: children[i + 1]

                    // substitute from left
                    int _n = children[i]->n;
                    if (_n > children[i]->mmin) {
                        T* repl = getPredecessor(i);
                        // recursively delete key
                        children[i]->erase(repl->cmin, repl->cmax);
                        // borrow largest key from left
                        data[i] = repl;
                        return 0;
                    }

                    // substitute from right
                    _n = children[i + 1]->n;
                    if (_n > children[i]->mmin) {
                        T* repl = getSuccessor(i + 1);
                        // recusively delete
                        children[i + 1]->erase(repl->cmin, repl->cmax);
                        // borrow smallest key from right
                        data[i] = repl;
                        return 0;
                    }

                    // both children have min no. keys
                    data[i] = nullptr;
                    sparseShiftLeft<T*>(data, i, n);
                    mergeChildren(i);
                }

                // return if violates min
                return n >= mmin ? 0 : 1;
            }
            else {
                // wasn't found
                if (leaf) {
                    // no more children to search
                    return -1;
                }

                // goto child
                int res = children[i]->erase(cmin, cmax);
                if (res < 1) {
                    // successful or not found
                    return res;
                }
                else {
                    // deletion in child violates minimum condition
                    /*
                        - look to siblings, if either have more than min, borrow
                        - else, merge with sibling and parent
                    */

                    bool hasLeft = i > 0;
                    bool hasRight = i < n + 1; // + 1 because the data was already removed


                    if (hasLeft && children[i - 1]->n > children[i]->mmin) {
                        // has left sibling with more than min keys
                        borrow(i, true);
                        return 0;
                    }

                    if (hasRight && children[i + 1]->n > children[i]->mmin) {
                        // has right sibling with more than min keys
                        borrow(i, false);
                        return 0;
                    }

                    // both siblings have min, merge with sibling and parent
                    if (hasLeft) {
                        // parent is data[i - 1]

                        // insert into target child
                        sparseShiftRight<T*>(children[i]->data, 0, children[i]->n);
                        children[i]->data[0] = data[i - 1];

                        mergeChildren(i - 1);
                        sparseShiftLeft<T*>(data, i - 1, n);

                        // return if violates min
                        return n >= mmin ? 0 : 1;
                    }
                    if (hasRight) {
                        // parent is data[i]

                        // insert into right sibling
                        sparseShiftRight<T*>(children[i + 1]->data, 0, children[i + 1]->n);
                        children[i + 1]->data[0] = data[i];

                        mergeChildren(i);
                        sparseShiftLeft<T*>(data, i, n);

                        // return if violates min
                        return n >= mmin ? 0 : 1;
                    }
                }

                return -1;
            }
        }

    private:
        

        T* getPredecessor(int idx) {
            SparseNode<T>* current = children[idx];
            while (!current->leaf) {
                // get last child
                current = current->children[current->n];
            }

            return current->data[current->n - 1];
        }

        T* getSuccessor(int idx) {
            SparseNode<T>* current = children[idx];
            while (!current->leaf) {
                // get first child
                current = current->children[0];
            }

            return current->data[0];
        }

        void mergeChildren(int leftIdx) {
            SparseNode<T>* child = children[leftIdx];
            SparseNode<T>* sibling = children[leftIdx + 1];

            int ogN = child->n;
            if (ogN == child->mmin - 1) {
                ogN++;
            }

            // data
            for (int i = 0; i < sibling->n; i++) {
                child->data[child->n++] = sibling->data[i];
            }

            // children
            if (!child->leaf) {
                for (int i = 0; i < sibling->n; i++) {
                    child->children[ogN + i] = sibling->children[i];
                }
            }

            // left shift children
            int _n = n + 1;
            sparseShiftLeft<SparseNode<T>*>(children, leftIdx + 1, _n);
        }

        void borrow(int base, bool toLeft) {
            bool isLeaf = children[base]->leaf;

            /*
                if to left, parent is data[base - 1]
                if to right, parent is data[base]
            */

            if (toLeft) {
                // shift data over
                sparseShiftRight<T*>(children[base]->data, 0, children[base]->n);
                // insert parent element
                children[base]->data[0] = data[base - 1];

                // substitute greatest key from left sibling
                int _n = children[base - 1]->n;
                data[base - 1] = children[base - 1]->data[_n - 1];

                // update sibling
                children[base - 1]->data[_n - 1] = nullptr;
                children[base - 1]->n--;

                if (!isLeaf) {
                    // bring over last child from left
                    int noChildren = children[base]->n;
                    sparseShiftRight<SparseNode<T>*>(children[base]->children, 0, noChildren);

                    children[base]->children[0] = children[base - 1]->children[_n];
                    children[base - 1]->children[_n] = nullptr;
                }
            }
            else {
                // insert parent element
                children[base]->data[children[base]->n] = data[base];
                children[base]->n++;

                // substitute smallest key from right sibling
                int _n = children[base + 1]->n;
                data[base] = children[base + 1]->data[0];

                // update sibling
                sparseShiftLeft<T*>(children[base + 1]->data, 0, children[base + 1]->n);
                children[base + 1]->data[children[base + 1]->n] = nullptr; // remove last key (extra from shift)

                if (!isLeaf) {
                    // bring over first child from right
                    int noChildren = children[base]->n;

                    children[base]->children[noChildren] = children[base + 1]->children[0];
                    _n++;
                    sparseShiftLeft<SparseNode<T>*>(children[base + 1]->children, 0, _n);
                    children[base + 1]->children[_n] = nullptr; // remove last child (extra from shift)
                }
            }
        }
    };

    template <typename T>
    class SparseBTree {
    public:
        int m;			// degree of tree

        SparseNode<T>* root;	// root SparseNode of tree

        SparseBTree(int m)
            : m(m) {
            // allocate for root
            root = new SparseNode<T>(m, true);
            root->mmin = 1;
        }

        void traverse(int key, void(itemViewer)(T* data)) {
            root->traverse(key, itemViewer);
        }

        void insert(std::vector<T*> elements) {
            for (T* element : elements) {
                insert(element);
            }
        }

        void insert(T* element) {
            T* upshiftElement = nullptr;
            SparseNode<T>* right = root->insert(element, upshiftElement);

            if (right) {
                // need to create new root
                SparseNode<T>* newRoot = new SparseNode<T>(m, false);

                

                // set children
                newRoot->children[0] = root;
                newRoot->children[1] = right;
                root->parent = newRoot;
                right->parent = newRoot;
                root->childId = 0;
                right->childId = 1;

                // set data
                newRoot->data[0] = upshiftElement;
                newRoot->n = 1;
                //upshiftElement->incrementParents(upshiftElement->size);

                newRoot->dataSpan = root->dataSpan + right->dataSpan;


                // reset root in class
                root = newRoot;
            }
        }

        T* operator[](int key) {
            return root->search(key);
        }

        T* search(int key) {
            return root->search(key);
        }

        bool erase(int cmin, int cmax) {
            int res = root->erase(cmin, cmax);

            if (root->n == 0) {
                // no more keys, tree shrinks
                if (!root->leaf) {
                    root = root->children[0];
                }
            }

            return res == 0;
        }

        void clear() {
            clearNode(root);
        }

    private:
        void clearNode(SparseNode<T>* n) {
            for (int i = 0; i < n->n; i++) {
                if (!n->leaf) {
                    clearNode(n->children[i]);
                    n->children[i] = nullptr;
                }
                n->data[i] = nullptr;
            }
            if (!n->leaf) {
                clearNode(n->children[n->n]);
                n->children[n->n] = nullptr;
            }
            n->children = nullptr;
            n->data = nullptr;
        }
    };

#endif //SPARSEBTREE_HPP