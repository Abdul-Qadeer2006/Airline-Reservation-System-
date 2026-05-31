// ============================================================
// AVLTree.h
// AVL Self-Balancing BST  —  Key: string, Value: int
// Used for O(log n) city-name -> index and hotel price lookup
// ============================================================
#ifndef AVLTREE_H
#define AVLTREE_H

#include <string>
#include <iostream>
using namespace std;

struct AVLNode {
    string   key;
    int      value;
    int      height;
    AVLNode* left;
    AVLNode* right;

    AVLNode(const string& k, int v)
        : key(k), value(v), height(1), left(nullptr), right(nullptr) {
    }
};

class AVLTree {
    AVLNode* root;

    int height(AVLNode* n) { return n ? n->height : 0; }
    int bf(AVLNode* n) { return n ? height(n->left) - height(n->right) : 0; }
    void updateH(AVLNode* n) {
        if (!n) return;
        int lh = height(n->left), rh = height(n->right);
        n->height = 1 + (lh > rh ? lh : rh);
    }

    AVLNode* rotR(AVLNode* y) {
        AVLNode* x = y->left, * T = x->right;
        x->right = y; y->left = T;
        updateH(y); updateH(x); return x;
    }
    AVLNode* rotL(AVLNode* x) {
        AVLNode* y = x->right, * T = y->left;
        y->left = x; x->right = T;
        updateH(x); updateH(y); return y;
    }
    AVLNode* balance(AVLNode* n) {
        updateH(n);
        int b = bf(n);
        if (b > 1) { if (bf(n->left) < 0) n->left = rotL(n->left);  return rotR(n); }
        if (b < -1) { if (bf(n->right) > 0) n->right = rotR(n->right); return rotL(n); }
        return n;
    }

    AVLNode* insert(AVLNode* n, const string& key, int val) {
        if (!n) return new AVLNode(key, val);
        if (key < n->key) n->left = insert(n->left, key, val);
        else if (key > n->key) n->right = insert(n->right, key, val);
        else                   n->value = val;
        return balance(n);
    }

    AVLNode* find(AVLNode* n, const string& key) const {
        if (!n) return nullptr;
        if (key < n->key) return find(n->left, key);
        else if (key > n->key) return find(n->right, key);
        else                   return n;
    }

    void destroy(AVLNode* n) {
        if (!n) return;
        destroy(n->left); destroy(n->right); delete n;
    }

public:
    AVLTree() : root(nullptr) {}
    ~AVLTree() { destroy(root); }

    void insert(const string& key, int val) { root = insert(root, key, val); }

    // Returns value, or -1 if not found
    int search(const string& key) const {
        AVLNode* n = find(root, key);
        return n ? n->value : -1;
    }

    bool contains(const string& key) const { return find(root, key) != nullptr; }
};

#endif
