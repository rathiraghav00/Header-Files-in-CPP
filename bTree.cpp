#pragma once

#include <stdlib.h>
#include <utility>
#include <stdio.h>

using namespace std;

#define NEW_ROOT 2
#define MODIFIED_NOT_ROOT 1
#define NOT_MODIFIED 0

template <typename T>
BTree<T>::BTree(unsigned t, bool (*compare)(T, T), void (*printK)(T))
{
    minDegree = t;
    lessThan = compare;
    root = (BNode<T> *)malloc(sizeof(BNode<T>));
    initializeNode(root);
    root->leaf = true;
    printKey = printK;
}


template <typename T>
BTree<T>::~BTree<T>()
{
    freeNode(root);
}

template <typename T>
void BTree<T>::insert(T k)
{

    // Grow upwards if the root is full.
    if (root->size == 2 * minDegree - 1)
    {
        BNode<T> *newRoot = (BNode<T> *)malloc(sizeof(BNode<T>));
        initializeNode(newRoot);
        newRoot->leaf = false;
        newRoot->child[0] = root;
        root = newRoot;
        splitChild(newRoot, 0);
    }

    // Work down the tree.
    BNode<T> *curr = root;
    while (!curr->leaf)
    {

        // Find the proper child to go to.
        int index = curr->size - 1;
        while (index >= 0 && lessThan(k, curr->key[index]))
        {
            index--;
        }
        index++;

        // Split child if full.
        if (curr->child[index]->size == 2 * minDegree - 1)
        {
            splitChild(curr, index);
            if (lessThan(curr->key[index], k))
            {
                index++;
            }
        }
        curr = curr->child[index];
    }

    nodeInsert(curr, k);
}


template <typename T>
T BTree<T>::remove(T k)
{
    BNode<T> *curr = root;
    while (true)
    {
        unsigned i = findIndex(curr, k);

        // If the item to be deleted has been found.
        if (i < curr->size && !(lessThan(curr->key[i], k) || lessThan(k, curr->key[i])))
        {
            T toReturn = curr->key[i];

            // If at a leaf, just delete it.
            if (curr->leaf)
            {
                nodeDelete(curr, i);
            }

            // Otherwise replace with predecessor/successor or merge children.
            else
            {
                BNode<T> *leftKid = curr->child[i];
                BNode<T> *rightKid = curr->child[i + 1];

                // Replace with predecessor.
                if (leftKid->size >= minDegree)
                {
                    while (!(leftKid->leaf))
                    {
                        fixChildSize(leftKid, leftKid->size);
                        leftKid = leftKid->child[leftKid->size];
                    }
                    curr->key[i] = nodeDelete(leftKid, leftKid->size - 1);
                }

                // Replace with successor
                else if (rightKid->size >= minDegree)
                {
                    while (!(rightKid->leaf))
                    {
                        fixChildSize(rightKid, 0);
                        rightKid = rightKid->child[0];
                    }
                    curr->key[i] = nodeDelete(rightKid, 0);
                }

                // Merge children and move down the tree.
                else
                {
                    mergeChildren(curr, i);
                    curr = leftKid;
                    continue;
                }
            }
            return toReturn;
        }

        // If the item has not been found, move down the tree.
        else
        {

            // If at a leaf, then the item isn't present.
            if (curr->leaf)
            {
                throw(BTREE_EXCEPTION) REMOVE_KEY_NOT_FOUND;
            }

            // Adjust curr and move down the tree.
            char result = fixChildSize(curr, i);
            if (result == NEW_ROOT)
            {
                curr = root;
            }
            else
            {
                curr = curr->child[findIndex(curr, k)];
            }
        }
    }
}


template <typename T>
pair<BNode<T> *, unsigned> BTree<T>::search(T k)
{

    // Start at root.
    BNode<T> *x = root;

    // Work down the tree.
    while (true)
    {

        // Find the proper index in the current node's array.
        unsigned i = findIndex(x, k);

        // Found it!
        if (i < x->size && !(lessThan(k, x->key[i]) || lessThan(x->key[i], k)))
        {
            return pair<BNode<T> *, unsigned>(x, i);
        }

        // Hit the bottom of the tree.
        else if (x->leaf)
        {
            return pair<BNode<T> *, unsigned>(NULL, 0);
        }

        // Keep going.
        else
        {
            x = x->child[i];
        }
    }
}


template <typename T>
T BTree<T>::searchKey(T k)
{
    pair<BNode<T> *, unsigned> node = search(k);
    if (node.first == NULL)
    {
        throw(BTREE_EXCEPTION) SEARCH_KEY_NOT_FOUND;
    }
    return node.first->key[node.second];
}


template <typename T>
void BTree<T>::print()
{
    if (printKey != NULL && root != NULL)
    {
        printf("\n");
        printNode(root, 0);
        printf("\n");
    }
}


template <typename T>
void BTree<T>::initializeNode(BNode<T> *x)
{
    x->size = 0;
    x->key = (T *)malloc((2 * minDegree - 1) * sizeof(T));
    x->child = (BNode<T> **)malloc(2 * minDegree * sizeof(BNode<T> *));
}

template <typename T>
void BTree<T>::freeNode(BNode<T> *x)
{
    if (!x->leaf)
    {
        for (unsigned i = 0; i <= x->size; i++)
        {
            freeNode(x->child[i]);
        }
    }
    free(x->child);
    free(x->key);
    free(x);
}


template <typename T>
unsigned BTree<T>::findIndex(BNode<T> *x, T k)
{
    unsigned i = 0;
    while (i < x->size && lessThan(x->key[i], k))
    {
        i++;
    }
    return i;
}


template <typename T>
unsigned BTree<T>::nodeInsert(BNode<T> *x, T k)
{
    int index;

    // Make room for k.
    for (index = x->size; index > 0 && lessThan(k, x->key[index - 1]); index--)
    {
        x->key[index] = x->key[index - 1];
        x->child[index + 1] = x->child[index];
    }

    // Insert k.
    x->child[index + 1] = x->child[index];
    x->key[index] = k;
    x->size++;

    return index;
}


template <typename T>
T BTree<T>::nodeDelete(BNode<T> *x, unsigned index)
{

    T toReturn = x->key[index];

    x->size--;
    while (index < x->size)
    {
        x->key[index] = x->key[index + 1];
        x->child[index + 1] = x->child[index + 2];
        index++;
    }
    return toReturn;
}


template <typename T>
void BTree<T>::splitChild(BNode<T> *x, int i)
{

    // z is the new node and y is the node to split.
    BNode<T> *toSplit = x->child[i];
    BNode<T> *newNode = (BNode<T> *)malloc(sizeof(BNode<T>));
    ;
    initializeNode(newNode);
    newNode->leaf = toSplit->leaf;
    newNode->size = minDegree - 1;

    // Copy the second half of y's keys and children into z.
    for (unsigned j = 0; j < minDegree - 1; j++)
    {
        newNode->key[j] = toSplit->key[j + minDegree];
    }
    if (!toSplit->leaf)
    {
        for (unsigned j = 0; j < minDegree; j++)
        {
            newNode->child[j] = toSplit->child[j + minDegree];
        }
    }
    toSplit->size = minDegree - 1;

    nodeInsert(x, toSplit->key[minDegree - 1]);
    x->child[i + 1] = newNode;
}


template <typename T>
char BTree<T>::mergeChildren(BNode<T> *parent, unsigned i)
{

    BNode<T> *leftKid = parent->child[i];
    BNode<T> *rightKid = parent->child[i + 1];

    // Move item from parent to left child.
    leftKid->key[leftKid->size] = nodeDelete(parent, i);
    unsigned j = ++(leftKid->size);

    // Move everything from rightKid into leftKid
    for (unsigned k = 0; k < rightKid->size; k++)
    {
        leftKid->key[j + k] = rightKid->key[k];
        leftKid->child[j + k] = rightKid->child[k];
    }
    leftKid->size += rightKid->size;
    leftKid->child[leftKid->size] = rightKid->child[rightKid->size];

    // Free the memory used by rightChild
    free(rightKid->child);
    free(rightKid->key);
    free(rightKid);

    // If parent is empty, than it must have been the root.
    if (parent->size == 0)
    {
        root = leftKid;
        free(parent->child);
        free(parent->key);
        free(parent);
        return NEW_ROOT;
    }

    return MODIFIED_NOT_ROOT;
}


template <typename T>
char BTree<T>::fixChildSize(BNode<T> *parent, unsigned index)
{
    BNode<T> *kid = parent->child[index];

    // If things need fixed.
    if (kid->size < minDegree)
    {

        // Borrow from left sibling if possible.
        if (index != 0 && parent->child[index - 1]->size >= minDegree)
        {
            BNode<T> *leftKid = parent->child[index - 1];

            for (unsigned i = nodeInsert(kid, parent->key[index - 1]); i != 0; i--)
            {
                kid->child[i] = kid->child[i - 1];
            }
            kid->child[0] = leftKid->child[leftKid->size];
            parent->key[index - 1] = nodeDelete(leftKid, leftKid->size - 1);
        }

        // Borrow from right sibling if possible
        else if (index != parent->size && parent->child[index + 1]->size >= minDegree)
        {
            BNode<T> *rightKid = parent->child[index + 1];
            // Move curr->key[i] into kid->key
            nodeInsert(kid, parent->key[index]);
            kid->child[kid->size] = rightKid->child[0];
            rightKid->child[0] = rightKid->child[1];
            // Move rightKid->key[0] into curr->key
            parent->key[index] = nodeDelete(rightKid, 0);
        }

        // If borrowing is not possible, then merge.
        else if (index != 0)
        {
            return mergeChildren(parent, index - 1);
        }
        else
        {
            return mergeChildren(parent, index);
        }
        return MODIFIED_NOT_ROOT;
    }

    // If things don't need fixed.
    return NOT_MODIFIED;
}

template <typename T>
void BTree<T>::printNode(BNode<T> *node, unsigned tab)
{

    // Indent
    for (unsigned i = 0; i < tab; i++)
    {
        printf("\t");
    }

    // Print the current node.
    for (unsigned i = 0; i < node->size; i++)
    {
        printKey(node->key[i]);
        printf(" ");
    }
    printf("\n");

    // Print all child nodes.
    if (!node->leaf)
    {
        tab++;
        for (unsigned i = 0; i <= node->size; i++)
        {
            printNode(node->child[i], tab);
        }
    }
}
