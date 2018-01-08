//
// Created by maara on 5/15/17.
//

#include <iostream>
#include "BlockWarnBuffer.h"

BlockWarnBuffer::BlockWarnBuffer(const int blockSize, const int blockCount,
                                 const int warnBlockCount) : blockSize(blockSize),
                                                             blockCount(blockCount),
                                                             warnBlockCount(warnBlockCount) {
    pos = blockSize*blockCount - 1;
    blocks = new bool[blockCount];
    reset();
}
BlockWarnBuffer::~BlockWarnBuffer() {
    delete blocks;
}

bool BlockWarnBuffer::querry() {
    if (resetLocation >= 0) {
        if (pos == resetLocation) {
            resetLocation = -1;
        }
        return false;
    }
    
    int cnt = 0;
    for (int i = 0; i < blockCount; i++) {
        cnt += blocks[i] ? 1 : 0;
    }
    
    return cnt >= warnBlockCount;
}

bool BlockWarnBuffer::push(const bool warning) {
    ++pos;
    
    if (pos >= blockCount * blockSize) {
        pos = pos % (blockCount * blockSize);
    }
    
    if (pos % blockSize == 0) {
        blocks[pos / blockSize] = false;
    }
    
    blocks[pos / blockSize] |= warning;
    
    return querry();
}

void BlockWarnBuffer::reset() {
    for (int i = 0; i < blockCount; i++) {
        blocks[i] = false;
    }
    resetLocation = pos;
    pos++;
}

std::ostream& operator<<(std::ostream& out, const BlockWarnBuffer& buffer) {
    out << "WarnBuffer[" << buffer.pos << "]{";
    
    for (int i = 0; i < buffer.blockCount; i++) {
        out << ( i == 0 ? "" : ",") << (buffer.blocks[i] ? "T" : "f");
    }
    out << "}";
    return out;
}
