//
// Created by maara on 4/19/17.
//

#ifndef SOCKETCOMMUNICATORCLION_STATSANALYZER_H
#define SOCKETCOMMUNICATORCLION_STATSANALYZER_H

#include <queue>

#include "StatsBuffer.h"


struct LoadEstimates {
    double sendLoad = 0.0;
    double recvLoad = 0.0;
    double netLoad = 0.0;
};

std::ostream& operator<<(std::ostream&, const LoadEstimates&);

class StatsAnalyzer {
public:
    StatsAnalyzer();
    ~StatsAnalyzer();
    LoadEstimates analyze(StatsBuffer& buffer);
};

#endif //SOCKETCOMMUNICATORCLION_STATSANALYZER_H
