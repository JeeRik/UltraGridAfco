//
// Created by maara on 3/14/17.
//

#ifndef SOCKETCOMMUNICATORCLION_MAIN_H
#define SOCKETCOMMUNICATORCLION_MAIN_H

#include "ComHandler.h"
#include "StatsParser.h"
#include "StatsAnalyzer.h"
#include "StatsBuffer.h"
#include "StatsCheck.h"
#include "BlockWarnBuffer.h"

class Main {
private:
    ComHandler com;
    StatsParser parser;
    StatsBuffer statsBuffer;
    StatsCheck::Matcher checkMatcher;
    BlockWarnBuffer warnBuffer;
    StatsAnalyzer analzyer;
    
    long lastSendTimestamp;
    long lastRecvTimestamp;
    const long CHANNEL_TIMEOUT = 10000;
    

    InMessage issueCommand(const std::string &command);
    InMessage reset();
    void processData(InMessage msg);
public:
    Main(ComHandler &com);
    void main();
};


#endif //SOCKETCOMMUNICATORCLION_MAIN_H
