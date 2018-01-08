//
// Created by maara on 5/4/17.
//

#ifndef SOCKETCOMMUNICATORCLION_STATSCHECK_H
#define SOCKETCOMMUNICATORCLION_STATSCHECK_H


#include <vector>
#include <deque>
#include "Stats.h"
#include "RingBuffer.h"


namespace StatsCheck {
    
    class Check {
    public:
        virtual bool check(RingBuffer<StatsPair>&) = 0;
        virtual const std::string getMessage() = 0;
    };

    class Matcher {
        std::string message = "";
        std::vector<Check*> checks;
    public:
        Matcher();
        bool check(RingBuffer<StatsPair>&);
        const std::string getLastMessage();
    };
    
};


#endif //SOCKETCOMMUNICATORCLION_STATSCHECK_H
