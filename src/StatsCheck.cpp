//
// Created by maara on 5/4/17.
//

#include <iostream>
#include <sstream>
#include "StatsCheck.h"

using namespace std;

namespace StatsCheck {
    class CheckDisplayed : public Check {
        uint64_t lastSkipId= 0;
        bool check(RingBuffer<StatsPair>& buffer) {
            StatsPair now; now = buffer[0];
            StatsPair prev; prev = buffer[1];
            
            if (now.second.isDisplayed - prev.second.isDisplayed != 1) {
                lastSkipId = now.first.bufferId;
                return false;
            }
            
            return true;
        }
        const string getMessage() {
            stringstream msg;
            msg << "Frame " << lastSkipId << " was not displayed";
            return msg.str();
        }
    };
    
    class CheckCorrupted : public Check {
        uint64_t lastSkipId= 0;
        bool check(RingBuffer<StatsPair>& buffer) {
            StatsPair now; now = buffer[0];
            StatsPair prev; prev = buffer[1];
            
            if (now.second.isCorrupted - prev.second.isCorrupted > 0) {
                lastSkipId = now.first.bufferId;
                return false;
            }
            
            return true;
        }
        const string getMessage() {
            stringstream msg;
            msg << "Frame " << lastSkipId << " was corrupted";
            return msg.str();
        }
    };
    
    class CheckDroppedFrames : public Check {
        uint64_t dropId = 0;
        bool check(RingBuffer<StatsPair>& buffer) {
            StatsPair now; now = buffer[0];
            StatsPair prev; prev = buffer[1];
    
            if (now.first.droppedFrames - prev.first.droppedFrames != 0) {
                std::cout << "Dropped frames warning:" << std::endl;
                std::cout << "  prev=" << prev << std::endl;
                std::cout << "  now =" << now << std::endl;
                std::cout << "" << std::endl;
                dropId = now.first.bufferId;
                return false;
            }
    
            return true;
        }
        const string getMessage() {
            stringstream msg;
            msg << "There was a dropped frame before " << dropId;
            return msg.str();
        }
    };
    
    class CheckLostBytes : public Check {
        bool check(RingBuffer<StatsPair>& buffer) {
            StatsPair now; now = buffer[0];
            StatsPair prev; prev = buffer[1];
            
            uint64_t expDiff = now.second.receivedBytes - prev.second.receivedBytes;
            uint64_t actDiff =  now.second.receivedBytes - prev.second.receivedBytes;
            
//            if (expDiff - actDiff != 0) {
            if (expDiff - actDiff != 0) {
                return false;
            }
    
            return true;
        }
        const string getMessage() {
            return "Lost bytes detected";
        }
        
    };

    Matcher::Matcher() {
        checks.insert(checks.end(), new CheckDisplayed());
        checks.insert(checks.end(), new CheckCorrupted());
        checks.insert(checks.end(), new CheckDroppedFrames());
        checks.insert(checks.end(), new CheckLostBytes());
    }
    
    bool Matcher::check(RingBuffer<StatsPair>& buffer) {
        bool ret = true;
        
        for (Check* check : checks) {
            if (! check->check(buffer)) {
                message = check->getMessage();
                ret = false;
            }
        }
        
        return ret;
    }
    
    const std::string Matcher::getLastMessage() {
        return message;
    }
}

