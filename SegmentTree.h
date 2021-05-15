#ifndef SEGTREE_H
#define SEGTREE_H

// 0 based indexing
#define left(i) (2 * i + 1)
#define right(i) (2 * i + 2)
#define parent(i) ((i - 1) / 2)

// Vector STL
#include <vector>

template <class T>
class SegTree
{

public:
    // Constructor of Segment Tree
    SegTree(std::vector<T> data, T value, T (*combine)(T obj1, T obj2));
    SegTree(T ar[], int n, T value, T (*combine)(T obj1, T obj2));

    // Query in range [l, r]
    T query(int l, int r);

    // Update the element at index idx to val
    void update(int idx, T val);

private:
    // Root of the segment tree
    T *tree;

    // Function call to build the segment tree
    void buildTree(std::vector<T> data);

    // Size of the segment tree array
    int segTreeSize;

    // Const value for extra nodes
    T valueForExtraNodes;

    // Method to combine child nodes
    T(*combine)
    (T obj1, T obj2);

    // Function to calculate size of segment tree required
    int calculateSize(int n);

    // Function to query in a particular range
    T queryHelper(int l, int r, int st, int ed, int node);

    // Function to update the value at an index
    T update(int idx, T val);
};

template <class T>
SegTree<T>::SegTree(std::vector<T> data,
                            T value, T (*combine)(T obj1, T obj2))
{
    this->combine = combine;
    valueForExtraNodes = value;
    segTreeSize = calculateSize(data.size());
    buildTree(data);
}

template <class T>
SegTree<T>::SegTree(T ar[], int n,
                            T value, T (*combine)(T obj1, T obj2))
{
    this->combine = combine;
    valueForExtraNodes = value;
    segTreeSize = calculateSize(n);

    std::vector<T> data;
    for (int i = 0; i < n; i++)
        data.push_back(ar[i]);

    buildTree(data);
}

template <class T>
int SegTree<T>::calculateSize(int n)
{
    int pw2 = 1;
    while (pw2 < n)
    {
        pw2 = pw2 << 1;
    }
    return 2 * pw2 - 1;
}

template <class T>
T SegTree<T>::query(int l, int r)
{
    int st = 0, ed = segTreeSize / 2;
    return queryHelper(l, r, st, ed, 0);
}

template <class T>
T SegTree<T>::queryHelper(int l, int r, int st, int ed, int node)
{
    // Invalid range
    if ((r < st) || (l > ed) || (l > r))
        return valueForExtraNodes;
    if (st >= l && ed <= r)
        return tree[node];
    T leftVal = queryHelper(l, r, st, (st + ed) / 2, left(node));
    T rightVal = queryHelper(l, r, (st + ed) / 2 + 1, ed, right(node));
    return combine(leftVal, rightVal);
}

template <class T>
void SegTree<T>::buildTree(std::vector<T> data)
{
    int n = data.size();

    tree = new T[segTreeSize];

    int extraNodes = (segTreeSize / 2 + 1) - n;

    for (int i = segTreeSize - 1; i >= 0; i--)
    {
        if (extraNodes > 0)
        {
            tree[i] = valueForExtraNodes;
            extraNodes--;
        }
        else if (n > 0)
        {
            tree[i] = data[n - 1];
            n--;
        }
        else
        {
            tree[i] = combine(tree[left(i)], tree[right(i)]);
        }
    }
}

template <class T>
void SegTree<T>::update(int idx, T val)
{
    // Index of segTree
    int segTreeIdx = (segTreeSize / 2) + idx;

    // Update the value
    tree[segTreeIdx] = val;

    // Update values for parent nodes
    while (parent(segTreeIdx) >= 0)
    {
        segTreeIdx = parent(segTreeIdx);

        if (right(segTreeIdx) < segTreeSize)
            tree[segTreeIdx] = combine(tree[left(segTreeIdx)], tree[right(segTreeIdx)]);

        // Root Node
        if (segTreeIdx == 0)
            break;
    }
}

// SEGTREE_H
#endif
