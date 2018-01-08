//
// Created by maara on 5/2/17.
//

#include "StatsBuffer.h"

template class RingBuffer<StatsPair>;

StatsBuffer::StatsBuffer(size_t len) : len(len), buffer(len) {
    
}

void StatsBuffer::push(StatsPair newPair) {
    buffer.push(newPair);
}

bool StatsBuffer::isReady() {
    return buffer.isFull();
}

StatsPair StatsBuffer::operator[](int i) {
    return buffer[i];
}
