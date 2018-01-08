//
// Created by maara on 4/19/17.
//
#include <iomanip>
#include <iostream>

#include "StatsAnalyzer.h"

using namespace std;

std::ostream& operator<<(std::ostream& out, const LoadEstimates& est) {
    out << "Loads{send=" << std::setprecision(2) << est.sendLoad << ", recv=" << std::setprecision(2) << est.recvLoad << ", net=" << std::setprecision(2) << est.netLoad << "}";
    return out;
}

StatsAnalyzer::StatsAnalyzer() {
}

StatsAnalyzer::~StatsAnalyzer() {
}

LoadEstimates StatsAnalyzer::analyze(StatsBuffer& buffer) {
    
    LoadEstimates ret;
    
    // - send load -------------------------------------------------------------
    StatsPair start;
    StatsPair now;
    start = buffer.buffer[buffer.len-1];
    now = buffer.buffer[0];
    
    // SEND overload check: Frames were dropped
    uint64_t dDropped = now.first.droppedFrames - start.first.droppedFrames;
    
    if (dDropped) {
        // Overload = no. of frames available / no. of frames processed
        ret.sendLoad = (double) (buffer.len+dDropped) / (double) buffer.len;
    } else {
        // Load = worst time spent processing frame / available time per frame
        double worst = 0.0;
        double second = 0.0;
        
        const double expected = buffer[0].first.nanoPerFrameExpected - buffer[1].first.nanoPerFrameExpected;
        
        for (uint64_t i = 0; i < buffer.len - 1; ++i) {
            uint64_t actual = buffer[i].first.nanoPerFrameActual - buffer[i+1].first.nanoPerFrameActual;
            double actualPerc = (double) actual / expected;
            
            uint64_t compress = buffer[i].first.compressMillis - buffer[i+1].first.compressMillis;
            if (compress > 0) compress -= 1;
            
            double compressPerc = (compress*1000000) / expected;
            
            double load = compressPerc > actualPerc ? compressPerc : actualPerc;
            
            if (load > worst) {
                second = worst;
                worst = load;
            } else if (load > second) {
                second = load;
            }
        }
        
        ret.sendLoad = second;
    }
    
    // - recv load -------------------------------------------------------------
    uint64_t expTime = buffer[0].second.nanoPerFrameExpected - buffer[buffer.len-1].second.nanoPerFrameExpected /
                buffer[0].second.isDisplayed - buffer[buffer.len-1].second.isDisplayed;
    // TODO: Verify UG reports correct times on non-displayed frames
    
    if (expTime <= 0) {
        std::cerr << "StatsAnalyzer: expTime is " << expTime << std::endl;
    }
    int delayedCnt = 0;
    int displayedCnt = 0;
    
    double worst = 0;
    double second = 0;
    
    // RECV overload check: processing frames takes longer than available time
    for (uint64_t i = 0; i < buffer.len - 1; i++) {
        double decTime = (buffer[i].second.nanoPerFrameDecompress - buffer[i+1].second.nanoPerFrameDecompress) / 1000.0;
        double actTime = (double) buffer[i].second.timestamp - buffer[i+1].second.timestamp;
        uint64_t displayed = buffer[i].second.isDisplayed - buffer[i+1].second.isDisplayed;
    
        if (displayed > 0) {
            ++displayedCnt;

            if (decTime >= expTime || actTime >= expTime) {
                ++delayedCnt;
                
                double load = decTime > actTime ? decTime : actTime / (double) expTime;
                if (load > worst) {
                    second = worst;
                    worst = load;
                } else if (load > second) {
                    second = load;
                }
            }
        }
    }
    
    // overload = longest time spent decoding a frame / time available per frame
    if (delayedCnt > 2) {
        std::cout << "  " << delayedCnt << " frames were delayed; second worst delay is " << second << std::endl;
        ret.recvLoad = second;
    }
    
    // - net load --------------------------------------------------------------
    uint64_t receivedBytes = now.second.receivedBytes - start.second.receivedBytes;
    uint64_t expectedBytes = now.second.expectedBytes - start.second.expectedBytes;
    
    // NET overload:
    if (receivedBytes < expectedBytes) {
        double worst = 0;
        double second = 0;

        for (uint64_t i = 0; i < buffer.len - 1; ++i) {
            uint64_t received = buffer[i].second.receivedBytes - buffer[i+1].second.receivedBytes;
            uint64_t expected = buffer[i].second.expectedBytes - buffer[i+1].second.expectedBytes;
            uint64_t corrupted = buffer[i].second.isCorrupted - buffer[i+1].second.isCorrupted;
            
            if (received < expected && corrupted > 0) {
                double loss = ((double) expected) / received;
                if (loss > worst) {
                    second = worst;
                    worst = loss;
                } else if (loss > second) {
                    second = loss;
                }
            }
        }
        
        // NET overload: if RECV is not overloaded, the largest data loss on any corrupted frame
        //   if RECV is overloaded, net losses may be caused by network buffer overflow
        std::cout << "recvLoad=" << ret.recvLoad << "; comparison=" << (ret.recvLoad == 0) << std::endl;
        ret.netLoad = ret.recvLoad == 0 ? second : 0;
    }
    
    return ret;
}
