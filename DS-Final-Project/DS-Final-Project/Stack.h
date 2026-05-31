
#ifndef STACK_H
#define STACK_H

template <typename T>
struct SNode {
    T      data;
    SNode* next;
    SNode(const T& d) : data(d), next(nullptr) {}
};

template <typename T>
class Stack {
    SNode<T>* topPtr;
    int       sz;
public:
    Stack() : topPtr(nullptr), sz(0) {}
    ~Stack() { while (!empty()) pop(); }

    void push(const T& val) {
        SNode<T>* n = new SNode<T>(val);
        n->next = topPtr; topPtr = n; sz++;
    }

    void pop() {
        if (!topPtr) return;
        SNode<T>* tmp = topPtr; topPtr = topPtr->next;
        delete tmp; sz--;
    }

    T& top() { return topPtr->data; }
    const T& top() const { return topPtr->data; }
    bool empty() const { return topPtr == nullptr; }
    int  size()  const { return sz; }
};

#endif
