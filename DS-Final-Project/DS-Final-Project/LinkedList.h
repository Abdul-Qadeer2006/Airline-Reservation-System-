// ============================================================
// LinkedList.h
// Generic singly-linked list — no STL
// ============================================================
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
using namespace std;

template <typename T>
struct LLNode {
    T       data;
    LLNode* next;
    LLNode(const T& d) : data(d), next(nullptr) {}
};

template <typename T>
class LinkedList {
public:
    LLNode<T>* head;
    int        sz;

    LinkedList() : head(nullptr), sz(0) {}

    ~LinkedList() { clear(); }

    // Deep copy
    LinkedList(const LinkedList& o) : head(nullptr), sz(0) {
        LLNode<T>* cur = o.head;
        while (cur) { pushBack(cur->data); cur = cur->next; }
    }
    LinkedList& operator=(const LinkedList& o) {
        if (this != &o) { clear(); LLNode<T>* c = o.head; while (c) { pushBack(c->data); c = c->next; } }
        return *this;
    }

    void pushFront(const T& val) {
        LLNode<T>* n = new LLNode<T>(val);
        n->next = head; head = n; sz++;
    }

    void pushBack(const T& val) {
        LLNode<T>* n = new LLNode<T>(val);
        if (!head) { head = n; sz++; return; }
        LLNode<T>* cur = head;
        while (cur->next) cur = cur->next;
        cur->next = n; sz++;
    }

    // Insert keeping the list sorted (ascending) via comparator lambda/functor
    // comparator(a,b) returns true if a < b
    template<typename Comp>
    void insertSorted(const T& val, Comp comp) {
        LLNode<T>* n = new LLNode<T>(val);
        if (!head || comp(val, head->data)) {
            n->next = head; head = n; sz++; return;
        }
        LLNode<T>* cur = head;
        while (cur->next && !comp(val, cur->next->data)) cur = cur->next;
        n->next = cur->next; cur->next = n; sz++;
    }

    void popFront() {
        if (!head) return;
        LLNode<T>* tmp = head; head = head->next;
        delete tmp; sz--;
    }

    bool empty()  const { return head == nullptr; }
    int  size()   const { return sz; }

    T& front() { return head->data; }
    const T& front() const { return head->data; }

    void clear() {
        while (head) { LLNode<T>* tmp = head; head = head->next; delete tmp; }
        sz = 0;
    }

    // Check existence with equality operator
    bool contains(const T& val) const {
        LLNode<T>* cur = head;
        while (cur) { if (cur->data == val) return true; cur = cur->next; }
        return false;
    }
};

#endif
#pragma once
