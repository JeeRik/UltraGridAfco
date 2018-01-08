//
// Created by maara on 4/6/17.
//
#include <sstream>
#include <iostream>
#include "StatsParser.h"

using namespace std;

SendStatsLine StatsParser::decodeSendStats(const string line) {
    
    istringstream iss(line);
    string tmp;
    uint64_t bufferId;
    uint64_t droppedFrames;
    uint64_t nanoPerFrameActual;
    uint64_t nanoPerFrameExpected;
    uint64_t sendBytesTotal;
    uint64_t timestamp;
    uint64_t compressMillis;

    iss >> tmp; // stats
    iss >> tmp; // SEND
    iss >> tmp; // default
    iss >> tmp; // bufferId
    if ("bufferId" != tmp) std::cerr << "Corrupted SEND line: " << line << std::endl;
    iss >> tmp;
    bufferId = stoull(tmp);
    iss >> tmp >> tmp;
    droppedFrames = stoull(tmp);
    iss >> tmp >> tmp;
    nanoPerFrameActual = stoull(tmp);
    iss >> tmp >> tmp;
    nanoPerFrameExpected = stoull(tmp);
    iss >> tmp >> tmp;
    sendBytesTotal = stoull(tmp);
    iss >> tmp >> tmp;
    timestamp = stoull(tmp);
    iss >> tmp >> tmp;
    compressMillis = stoull(tmp);
    
    return SendStatsLine{time(nullptr), bufferId, droppedFrames, nanoPerFrameActual, nanoPerFrameExpected, sendBytesTotal, timestamp, compressMillis};
}

RecvStatsLine StatsParser::decodeRecvStats(const string line) {
    istringstream iss(line);
    string tmp;
    vector<uint64_t> data;
    
    iss >> tmp >> tmp;
    if ("RECV" != tmp) std::cerr << "Corrupted RECV line: " << line << std::endl;
    
    while (iss) {
        iss >> tmp >> tmp;
        data.insert(data.end(), stoull(tmp));
    }
    
    return RecvStatsLine{time(nullptr), data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10]};
}

StatsPair * StatsParser::process(
        const InMessage message) {
    if (message.ch == Channel::CH_SEND) {
        SendStatsLine sLine = decodeSendStats(message.message);
    
        // If there is nothing to match against, add new stats record
        if (rBuffer.empty()) {
            sBuffer.push(new SendStatsLine(sLine));
            return nullptr;
        }
        
        // Discard all lines with lower id (no match will ever arrive)
        RecvStatsLine *rLine = NULL;
        do {
            if (rLine != NULL) {
                rBuffer.pop();
                delete rLine;
            }
            rLine = rBuffer.front();
        } while (rLine->bufferId < sLine.bufferId && rBuffer.size() > 1);
        
        // The ID is higher than anything in the matching statsBuffer; Empty the other statsBuffer and add this line
        if (rLine->bufferId < sLine.bufferId) {
            
            rBuffer.pop();
            delete rLine;
            sBuffer.push(new SendStatsLine(sLine));
            return nullptr;
        }
    
        // There is no matching line in the other statsBuffer. Discard this line
        if (rLine->bufferId > sLine.bufferId) {
            return nullptr;
        }
    
        // The other line matches, create and forward a tuple
        if (rLine->bufferId == sLine.bufferId) {
            rBuffer.pop();
            RecvStatsLine rLineVal = *rLine;
            delete rLine;
            return new StatsPair (sLine, rLineVal);
        }
        
        std::cerr << "Wait... what?" << std::endl;
    }
    
    if (message.ch == Channel::CH_RECV) {
        RecvStatsLine rLine = decodeRecvStats(message.message);
    
        // If there is nothing to match against, add new stats record
        if (sBuffer.empty()) {
            rBuffer.push(new RecvStatsLine(rLine));
            return nullptr;
        }
    
        // Discard all lines with lower id (no match will ever arrive)
        SendStatsLine *sLine = NULL;
        do {
            if (sLine != NULL) {
                sBuffer.pop();
                delete sLine;
            }
            sLine = sBuffer.front();
        } while (sLine->bufferId < rLine.bufferId && sBuffer.size() > 1);
    
        // The ID is higher than anything in the matching statsBuffer; Empty the other statsBuffer and add this line
        if (sLine->bufferId < rLine.bufferId) {
        
            sBuffer.pop();
            delete sLine;
            rBuffer.push(new RecvStatsLine(rLine));
            return nullptr;
        }
    
        // There is no matching line in the other statsBuffer. Discard this line
        if (sLine->bufferId > rLine.bufferId) {
            return nullptr;
        }
    
        // The other line matches, create and forward a tuple
        if (sLine->bufferId == rLine.bufferId) {
            sBuffer.pop();
            SendStatsLine sLineVal = *sLine;
            delete sLine;
            return new StatsPair (sLineVal, rLine);
        }
    
        std::cerr << "Wait... what?" << std::endl;
    }
    
    std::cout << "Unrecognized message: " << message << std::endl;
    return nullptr;
}

