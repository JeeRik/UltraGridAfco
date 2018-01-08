//
// Created by maara on 5/15/17.
//

#ifndef SOCKETCOMMUNICATORCLION_BLOCKWARNBUFFER_H
#define SOCKETCOMMUNICATORCLION_BLOCKWARNBUFFER_H


#include <ostream>

class BlockWarnBuffer {
    const int blockSize;
    const int blockCount;
    const int warnBlockCount;
    int pos; // Index of the last added warning
    bool *blocks;
    int resetLocation = 0;
public:
    BlockWarnBuffer(const int blockSize, const int blockCount, const int warnBlockCount = 5);
    bool push(const bool warning);
    bool querry();
    void reset();
    ~BlockWarnBuffer();
    
    friend std::ostream& operator<<(std::ostream& out, const BlockWarnBuffer& buffer);
};

std::ostream& operator<<(std::ostream& out, const BlockWarnBuffer& buffer);



#endif //SOCKETCOMMUNICATORCLION_BLOCKWARNBUFFER_H
