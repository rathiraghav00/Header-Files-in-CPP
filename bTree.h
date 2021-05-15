#pragma once

#include <utility>

#define NULL 0
#define SEARCH_KEY_NOT_FOUND 's'
#define REMOVE_KEY_NOT_FOUND 'r'

// Structure for representing nodes of a b tree
template <typename T>
struct BNode
{
    BNode<T> **child; // Array of pointers to children.
    T *key;           // Array of keys.
    unsigned size;    // Number of keys.
    bool leaf;        // Whether the node is a leaf.
};

typedef char BTREE_EXCEPTION;


template <typename T>
class BTree
{
public:

    // Constructor
    // First parameter is the minimum degree of the tree.
    // Second parameter is the tree's key-comparison function.
    // Third parameter is a function that prints keys.
    BTree(unsigned, bool (*)(T, T), void (*)(T) = NULL);

    // Destructor.
    ~BTree<T>();

    // Inserts a key into the tree.
    void insert(T);

    // Removes a key from the tree.
    T remove(T);

    // Function to find a key in the tree.
    // returnValue.first is the node the item is in.
    // returnValue.second is the correct index in that node's key array
    std::pair<BNode<T> *, unsigned> search(T);

    // Uses search but just returns the key rather than the whole node.
    T searchKey(T);

    // Prints the tree.
    void print();

private:
    // Used for initializing nodes.
    void initializeNode(BNode<T> *);

    // Recursive function called by destructor.
    void freeNode(BNode<T> *);

    // Finds the index of a key in a node.
    unsigned findIndex(BNode<T> *, T);

    // Inserts a key into a node.
    unsigned nodeInsert(BNode<T> *, T);

    // Deletes the key at a given index from a node.
    T nodeDelete(BNode<T> *, unsigned);

    // Function for splitting nodes that are too full.
    void splitChild(BNode<T> *, int);

    // Merges two children of a node at a given index into one child.
    char mergeChildren(BNode<T> *, unsigned);

    // Makes sure the child of a node at a specified index has >= minDegree items.
    char fixChildSize(BNode<T> *, unsigned);

    // Recursively prints a subtree.
    void printNode(BNode<T> *, unsigned);

    // Root node.
    BNode<T> *root;

    // Comparison function used for managing element placement.
    bool (*lessThan)(T, T);

    // Function used to print items in the tree.
    void (*printKey)(T);

    // Minimum degree of the tree.
    unsigned minDegree;
};

#include "bTree.cpp"
