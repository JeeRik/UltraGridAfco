//
// Created by maara on 5/10/17.
//

#ifndef SOCKETCOMMUNICATORCLION_RINGBUFFER_H
#define SOCKETCOMMUNICATORCLION_RINGBUFFER_H


#include <vector>

template <typename T> class RingBuffer {
    T* buffer;
    size_t size;
    size_t pos;
public:
    RingBuffer(size_t size);
    ~RingBuffer();
    
    void push(T&);
    bool isFull();
    T operator[](std::size_t);
};


#endif //SOCKETCOMMUNICATORCLION_RINGBUFFER_H
