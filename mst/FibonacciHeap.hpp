/*
 * FibonacciHeap.hpp
 *
 *  Created on: Oct 17, 2013
 *  Author: Sagar
 *  Algorithm: http://www.cse.yorku.ca/~aaw/Jason/FibonacciHeapAlgorithm.html
 */
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

/*
 * Class to hold F-heap node. Used by F-heap class
 */
class FHeapNode {
	int nodeKey;
	uint nodeData;

	// Number of children
	uint degree;
	// Used in the decreaseKey algorithm
	bool mark;

	// Pointers for circular doubly linked list
	FHeapNode* previous;
	FHeapNode* next;

	// Pointer to the first child
	FHeapNode* child;
	FHeapNode* parent;

	FHeapNode() {}
	FHeapNode(uint v, int c):
		nodeKey(c),
		nodeData(v),
		degree(0),
		mark(false),
		child(NULL),
		parent(NULL) {
		previous = next = this;
	}

	bool isSingle() const {
		return (this == this->next);
	}

	// Inserts a new node after this node
	void insert(FHeapNode* other) {
		if (!other)
			return;

		this->next->previous = other->previous;
		other->previous->next = this->next;

		this->next = other;
		other->previous = this;
	}

	// Remove this node from the list of nodes
	void remove() {
		this->previous->next = this->next;
		this->next->previous = this->previous;
		this->next = this->previous = this;
	}

	// Add a child - increase degree
	void addChild(FHeapNode* other) {
		if (!child)
			child = other;
		else
			child->insert(other);
		other->parent = this;
		other->mark = false;
		degree++;
	}

	// Remove Child - decrease degree
	void removeChild(FHeapNode* other) {
		if (other->parent!=this) {
			cout << "F-heap Error: Trying to remove a child from a non-parent" << endl;
			exit(EXIT_FAILURE);
		}
		if (other->isSingle()) {
			if (child != other) {
				cout << "F-heap Error: Trying to remove a non-child" << endl;
				exit(EXIT_FAILURE);
			}
			child = NULL;
		} else {
			if (child == other)
				child = other->next;
			other->remove(); // from list of children
		}
		other->parent=NULL;
		other->mark = false;
		degree--;
	}


	friend ostream& operator<< (ostream& out, const FHeapNode& n) {
		return (out << n.nodeData << ":" << n.nodeKey);
	}

	// For debug purpose
	void printTree() const {
		cout << nodeData << ":" << nodeKey << ":" << degree << ":" << mark;
		if (child) {
			cout << "(";
			const FHeapNode* n=child;
			do {
				if (n==this) {
					cout << "F-heap Error: Illegal pointer - node is child of itself" << endl;
					exit(EXIT_FAILURE);
				}
				n->printTree();
				cout << " ";
				n = n->next;
			} while (n!=child);
			cout << ")";
		}
	}

	//For debug purpose
	void printAll() const {
		const FHeapNode* n=this;
		do {
			n->printTree();
			cout << " ";
			n = n->next;
		} while (n!=this);
		cout << endl;
	}
public:
	// Functions to return key and data
	int key() const { return nodeKey; }
	uint data() const { return nodeData; }

	friend class FHeap;
};


/*
 * F-Heap class. Supports insert, removeMin and decreaseKey
 */
class FHeap {
	typedef FHeapNode* fNode;
	// A circular double linked list of nodes
	fNode rootWithMinKey;
	// Total number of elements in heap
	uint count;
	// Maximum degree of a root in the  circular d-list
	uint maxDegree;

protected:
	// Insert a node into the HeapNode structure 1<->2<->3<->4
	fNode insertNode(fNode newNode) {
#ifdef LOG_ON
		cout << "insert " << (*newNode) << endl;
#endif
		if (!rootWithMinKey) {
			// Insert the first node to the heap
			rootWithMinKey = newNode;
		} else {
			// Insert the root of new tree to the list of roots 1->2->3->4...
			rootWithMinKey->insert(newNode);
			if (newNode->key() < rootWithMinKey->key())
				rootWithMinKey = newNode;
		}
		return newNode;
	}

public:

	FHeap():
		rootWithMinKey(NULL), count(0), maxDegree(0) {}

	~FHeap() {}

	bool empty() const {return count==0;}

	// For debug purpose
	void printRoots() const {
		cout << "maxDegree = " << maxDegree << "  count= " << count << "  roots= ";
		if (rootWithMinKey)
			rootWithMinKey->printAll();
		else
			cout << endl;
	}

	// Gives the pointer to minimum element
	fNode minimum() const {
		if (!rootWithMinKey) {
			cout << "F-heap Error: No min element" << endl;
			exit(EXIT_FAILURE);
		}
		return rootWithMinKey;
	}

	// Insert an element into the heap
	fNode insert (uint data, int key) {
#ifdef LOG_ON
		cout << "insert " << data << ":" << nodeKey << endl;
#endif
		count++;
		return insertNode(new FHeapNode(data,key));
	}


	/*
	 * removeMin of F-Heap
	 * Algorithm part: Fibonacci-Heap-Extract-Min, CONSOLIDATE
	 */
	void removeMinimum() {
		if (!rootWithMinKey) {
			cout << "F-heap Error: trying to remove from an empty heap" << endl;
			exit(EXIT_FAILURE);
		}

#ifdef LOG_ON
		cout << "In removeMinimum" << endl;
#endif
		count--;

		// Make all children of root new roots
		if (rootWithMinKey->child) {
#ifdef LOG_ON
			cout << " Root's children: ";
			rootWithMinKey->child->printAll(cout);
#endif
			fNode c = rootWithMinKey->child;
			do {
				c->parent = NULL;
				c = c->next;
			} while (c != rootWithMinKey->child);

			// Removed all children
			rootWithMinKey->child = NULL;
			rootWithMinKey->insert(c);
		}
#ifdef LOG_ON
		cout << " Roots after inserting children: ";
		printRoots(cout);
#endif


		// Handle the case where we delete the last key
		if (rootWithMinKey->next == rootWithMinKey) {
#ifdef LOG_ON
			cout << "Removed the last key" << endl;
#endif
			if (count!=0) {
				cout << "F-heap Error: Internal error: should have 0 keys" << endl;
				exit(EXIT_FAILURE);
			}
			rootWithMinKey = NULL;
			return;
		}

		// Merge roots with the same degree - CONSOLIDATE
		// Make room for a new degree
		vector<fNode> degreeRoots (maxDegree+1);
		fill (degreeRoots.begin(), degreeRoots.end(), (fNode)NULL);
		maxDegree = 0;
		fNode currentPointer = rootWithMinKey->next;
		uint currentDegree;
		do {
			currentDegree = currentPointer->degree;
#ifdef LOG_ON
			cout << "  roots starting from currentPointer: ";
			currentPointer->printAll(cout);
			cout << "  checking root " << *currentPointer << " with degree " << currentDegree << endl;
#endif

			fNode current = currentPointer;
			currentPointer = currentPointer->next;
			while (degreeRoots[currentDegree]) {
				// Merge the two roots with the same degree
				fNode other = degreeRoots[currentDegree];
				if (current->key() > other->key())
					swap(other,current);
				// Now current->key() <= other->key() so, make other a child of current
				// remove from list of roots
				other->remove();
				current->addChild(other);
#ifdef LOG_ON
				cout << "  Added " << *other << " as child of " << *current << endl;
#endif
				degreeRoots[currentDegree]=NULL;
				currentDegree++;
				if (currentDegree >= degreeRoots.size())
					degreeRoots.push_back((fNode)NULL);
			}
			// keep the current root as the first of its degree in the degrees vector
			degreeRoots[currentDegree] = current;
		} while (currentPointer != rootWithMinKey);

		// Remove the current root, and calculate the new rootWithMinKey
		delete rootWithMinKey;
		rootWithMinKey = NULL;

		uint newMaxDegree = 0;
		for (uint d = 0; d < degreeRoots.size(); d++) {
#ifdef LOG_ON
			cout << "Degree " << d << ": ";
#endif
			if (degreeRoots[d]) {
#ifdef LOG_ON
				cout << " " << *degreeRoots[d] << endl;
#endif
				degreeRoots[d]->next = degreeRoots[d]->previous = degreeRoots[d];
				insertNode(degreeRoots[d]);
				if (d > newMaxDegree)
					newMaxDegree = d;
			} else {
#ifdef LOG_ON
				cout << "No node" << endl;
#endif
			}
		}
		maxDegree = newMaxDegree;
	}

	void decreaseKey(fNode node, int newKey) {
		if (newKey >= node->nodeKey) {
			cout << "Trying to decrease key to a greater key" << endl;
			exit(EXIT_FAILURE);
		}

#ifdef LOG_ON
		cout << "Decreasing key of " << *node << " to " << newKey << endl;
#endif
		// Update the key and possibly the min key
		node->nodeKey = newKey;

		// Check that min pointer always points to right value
		fNode parent = node->parent;

		// Root node - just make sure the minimum is correct
		if (!parent) {
			if (newKey < rootWithMinKey->key())
				rootWithMinKey = node;
			return;
		} else if (parent->key() <= newKey) {
			return;
		}

		// Cascading cut
		while(true){
			parent->removeChild(node);
			insertNode(node);
#ifdef LOG_ON
				cout << "Removed " << *node << " as child of " << *parent << endl;
				cout << "Roots after remove: ";
				rootWithMinKey->printAll(cout);
#endif
			// Parent is a root - nothing more to do
			if (!parent->parent) {
				break;
			} else if (!parent->mark) {
				// Parent is not a root and is not marked - just mark it
				parent->mark = true;
				break;
			} else {
				node = parent;
				parent = parent->parent;
				continue;
			}
		}
	}

};



