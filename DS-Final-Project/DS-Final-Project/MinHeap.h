// ============================================================
// MinHeap.h
// Array-based Min-Heap used as Priority Queue in Dijkstra
// No STL — manual heap operations
// ============================================================
#ifndef MINHEAP_H
#define MINHEAP_H

#include <iostream>
using namespace std;

// T must support:  T.key  (numeric, lower = higher priority)
// and operator< on key field
template <typename T>
class MinHeap {
    T* arr;
    int sz;
    int cap;

    void resize() {
        cap *= 2;
        T* newArr = new T[cap];
        for (int i = 0; i < sz; i++) newArr[i] = arr[i];
        delete[] arr;
        arr = newArr;
    }

    void heapifyUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (arr[i].key < arr[parent].key) {
                T tmp = arr[i]; arr[i] = arr[parent]; arr[parent] = tmp;
                i = parent;
            }
            else break;
        }
    }

    void heapifyDown(int i) {
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int smallest = i;
            if (left < sz && arr[left].key < arr[smallest].key) smallest = left;
            if (right < sz && arr[right].key < arr[smallest].key) smallest = right;
            if (smallest != i) {
                T tmp = arr[i]; arr[i] = arr[smallest]; arr[smallest] = tmp;
                i = smallest;
            }
            else break;
        }
    }

public:
    MinHeap(int initialCap = 64) : sz(0), cap(initialCap) {
        arr = new T[cap];
    }
    ~MinHeap() { delete[] arr; }

    void insert(const T& val) {
        if (sz == cap) resize();
        arr[sz++] = val;
        heapifyUp(sz - 1);
    }

    T extractMin() {
        T min = arr[0];
        arr[0] = arr[--sz];
        heapifyDown(0);
        return min;
    }

    const T& peekMin() const { return arr[0]; }
    bool empty() const { return sz == 0; }
    int  size()  const { return sz; }
};

#endif
