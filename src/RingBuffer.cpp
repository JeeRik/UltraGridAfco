//
// Created by maara on 5/10/17.
//

#include <iostream>
#include "RingBuffer.h"
#include "Stats.h"

template <typename T> RingBuffer<T>::RingBuffer(size_t size) : size(size), pos(0) {
    buffer = new T[size];
}

template <typename T> void RingBuffer<T>::push(T& t) {
    buffer[++pos % size] = t;
}

template <typename T> bool RingBuffer<T>::isFull() {
    return pos >= size;
}

template <typename T> T RingBuffer<T>::operator[](std::size_t index) {
    if (! isFull()) {
        std::cerr << "Cannot access elements of non-full RingBuffer" << std::endl;
        return buffer[0];
    }
    if (index >= size) {
        std::cerr << "Cannot access item at [" << index << "]of RingBuffer of size " << size << std::endl;
        return buffer[0];
    }
    
    return buffer[(pos - index) % size];
}

template <typename T> RingBuffer<T>::~RingBuffer() {
    delete buffer;
}

template class RingBuffer<StatsPair>;