//
// Created by maara on 5/2/17.
//

#ifndef SOCKETCOMMUNICATORCLION_STATSBUFFER_H
#define SOCKETCOMMUNICATORCLION_STATSBUFFER_H


#include <deque>
#include "Stats.h"
#include "RingBuffer.h"

class StatsBuffer {
public:
    const u_int len;
    RingBuffer<StatsPair> buffer;
    
    StatsBuffer(size_t len);
    void push(StatsPair newPair);
    bool isReady();
    StatsPair operator[](int i);
};


#endif //SOCKETCOMMUNICATORCLION_STATSBUFFER_H
