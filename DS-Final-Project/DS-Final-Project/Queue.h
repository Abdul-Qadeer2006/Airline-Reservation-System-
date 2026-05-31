// ============================================================
// Queue.h  —  Linked-list based Queue  (no STL)
// ============================================================
#ifndef QUEUE_H
#define QUEUE_H

template <typename T>
struct QNode {
    T      data;
    QNode* next;
    QNode(const T& d) : data(d), next(nullptr) {}
};

template <typename T>
class Queue {
    QNode<T>* frontPtr;
    QNode<T>* backPtr;
    int       sz;
public:
    Queue() : frontPtr(nullptr), backPtr(nullptr), sz(0) {}
    ~Queue() { while (!empty()) dequeue(); }

    void enqueue(const T& val) {
        QNode<T>* n = new QNode<T>(val);
        if (!backPtr) { frontPtr = backPtr = n; }
        else { backPtr->next = n; backPtr = n; }
        sz++;
    }

    void dequeue() {
        if (!frontPtr) return;
        QNode<T>* tmp = frontPtr;
        frontPtr = frontPtr->next;
        if (!frontPtr) backPtr = nullptr;
        delete tmp; sz--;
    }

    T& front() { return frontPtr->data; }
    const T& front() const { return frontPtr->data; }
    bool empty() const { return frontPtr == nullptr; }
    int  size()  const { return sz; }
};

#endif
