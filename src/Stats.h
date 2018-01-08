//
// Created by maara on 4/19/17.
//

#ifndef SOCKETCOMMUNICATORCLION_STATS_H
#define SOCKETCOMMUNICATORCLION_STATS_H

#include <ostream>

struct SendStatsLine {
    // stats SEND default bufferId 3882173 droppedFrames 0 nanoPerFrameActual 1151035696761 nanoPerFrameExpected 81570000000000 sendBytesTotal 1069154304 timestamp 1490088718475 compressMillis 6
    time_t receivedAt;
    uint64_t bufferId;
    uint64_t droppedFrames;
    uint64_t nanoPerFrameActual;
    uint64_t nanoPerFrameExpected;
    uint64_t sendBytesTotal;
    uint64_t timestamp;
    uint64_t compressMillis;
};

struct RecvStatsLine {
    // stats RECV bufferId 3882173 expectedPackets 774800 receivedPackets 774800 expectedBytes 1069154304 receivedBytes 1069154304 isCorrupted 0 isDisplayed 8157 timestamp 1490088718475 nanoPerFrameDecompress 953089 nanoPerFrameExpected -9223372036854775808 reportedFrames 8157
    time_t receivedAt;
    uint64_t bufferId;
    uint64_t expectedPackets;
    uint64_t receivedPackets;
    uint64_t expectedBytes;
    uint64_t receivedBytes;
    uint64_t isCorrupted;
    uint64_t isDisplayed;
    uint64_t timestamp;
    uint64_t nanoPerFrameDecompress;
    uint64_t nanoPerFrameExpected;
    uint64_t reportedFrames;
};

typedef std::pair<SendStatsLine, RecvStatsLine> StatsPair;
struct StatsDiff {
public:
    StatsPair start;
    StatsPair end;
    uint64_t length() const;
    uint64_t lostPackets() const;
    uint64_t lostBytes() const;
    uint64_t totalPackets() const;
    uint64_t totalBytes() const;
    uint64_t droppedFrames() const;
    uint64_t displayed() const;
    uint64_t corrupted() const;
    uint64_t timeDecompress() const;
    uint64_t timeCompress() const;
    uint64_t timeSend() const;
    uint64_t timeExpected() const;
    
    double timeDecompressRel() const;
    double timeCompressRel() const;
    double timeSendRel() const;
    double lostBytesRel() const;
    double lostPacketsRel() const;
    
    bool isOk() const;
    std::ostream& printDetail(std::ostream& out) const;
    std::ostream& printDetail(std::ostream& out, const std::string& separator) const;
};

std::ostream& operator<<(std::ostream& out, const SendStatsLine &line);
std::ostream& operator<<(std::ostream& out, const RecvStatsLine &line);
std::ostream& operator<<(std::ostream& out, const StatsPair &pair);
std::ostream& operator<<(std::ostream& out, const StatsDiff &pair);

#endif //SOCKETCOMMUNICATORCLION_STATS_H
