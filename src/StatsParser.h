//
// Created by maara on 4/6/17.
//

#ifndef SOCKETCOMMUNICATORCLION_STATSPARSER_H
#define SOCKETCOMMUNICATORCLION_STATSPARSER_H

#include <cstdint>
#include <string>
#include <queue>
#include "ComHandler.h"
#include "Stats.h"

class StatsParser {
private:

    std::queue<SendStatsLine*> sBuffer;
    std::queue<RecvStatsLine*> rBuffer;

public:
    StatsPair*  process(const InMessage msg);
    SendStatsLine decodeSendStats(const std::string line);
    RecvStatsLine decodeRecvStats(const std::string line);
};

#endif //SOCKETCOMMUNICATORCLION_STATSPARSER_H
