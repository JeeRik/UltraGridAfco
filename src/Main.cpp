//
// Created by maara on 3/13/17.
//

#include <vector>
#include "Main.h"
#include "StatsCheck.h"
#include "Tools.h"

using namespace std;

vector<string> parseCommands(const string text) {
    return {text};
}

bool isSenderReply(const string &text) {
    
    if (text.size() == 0) {
        return false;
    }
    char c = text.c_str()[0];
    bool ret = c >= 48 && c < 58;
    return ret;
}

bool isReceiverReply(const string text) {
    return isSenderReply(text);
}

bool isSenderData(const string text) {
    if (! text.compare(0, 6, "stats ")) return true;
    
    return false;
}

bool isReceiverData(const string text) {
    return isSenderData(text);
}

bool isSenderEvent(const string text) {
    return ! (isSenderData(text) || isSenderReply(text));
}

bool isReceiverEvent(const string text) {
    return ! (isReceiverData(text) || isReceiverReply(text));
}

Main::Main(ComHandler &com) : com(com), statsBuffer(25), warnBuffer(5, 5, 2) {
    issueCommand("stats on");
}

InMessage Main::issueCommand(const string &command) {
    com.sendCommand(command);

    InMessage reply;

    while (! isSenderReply(reply.message)) {
        reply = com.getSenderMessage(1000000);
    }
    
    return reply;
}

InMessage Main::reset() {
    
    warnBuffer.reset();
    
    InMessage tmp;
            
    while (true) {
        tmp = com.getMessage(0);
        if (tmp == InMessage()) {
            return InMessage();
        }
        if (tmp.ch == Channel::CH_CTRL) {
            return tmp;
        }
    }
}

void Main::processData(InMessage msg) {
    
    // build data
    StatsPair* tuplePtr = parser.process(msg);
    
    if (tuplePtr == nullptr) {
        return;
    }

    StatsPair tuple = *tuplePtr;
    delete tuplePtr;
    
    // buffer data
    statsBuffer.push(tuple);
    
    if (! statsBuffer.isReady()) {
        return;
    }
    
    // check warnings
    bool succ = checkMatcher.check(statsBuffer.buffer);
    if (! succ) {
        StatsDiff diff{statsBuffer[1], statsBuffer[0]};
        std::cout << "Check failed: " << diff << std::endl;
    }

    bool warn = warnBuffer.push(!succ);
    
    if (! warn) {
        return;
    }
    
    // if enough warnings accumulate, analyze errors
    LoadEstimates estimates = analzyer.analyze(statsBuffer);
    
    std::cout << "Problem detected. Analysis results: " << estimates << std::endl;
    StatsDiff diff{statsBuffer[statsBuffer.len-1], statsBuffer[0]};
    std::cout << "  " << getTimeString() << " " << diff << std::endl;
    diff.printDetail(std::cout, "\n  ");
    std::cout << std::endl;
    std::cout << warnBuffer << std::endl;
    
    for (uint64_t i = statsBuffer.len - 1; i > 0; --i) {
        StatsDiff d{statsBuffer[i], statsBuffer[i-1]};
        std::cout << "    " << i << ": ";
        d.printDetail(std::cout);
        std::cout << std::endl;
    }
    
    warnBuffer.reset();
};

void Main::main() {
    
    InMessage msg;
    lastRecvTimestamp = getUnixTime();
    lastSendTimestamp = getUnixTime();
    
    while (true) {
        
        if (msg == InMessage()) msg = com.getMessage(11000000);
        
        long now = getUnixTime();
        if (lastSendTimestamp + CHANNEL_TIMEOUT < now) {
            std::cerr << "No message received from SEND for " << CHANNEL_TIMEOUT << "ms" << std::endl;
            lastSendTimestamp = now;
        }
        if (lastRecvTimestamp + CHANNEL_TIMEOUT < now) {
            std::cerr << "No message received from RECV for " << CHANNEL_TIMEOUT << "ms" << std::endl;
            lastRecvTimestamp = now;
        }
    
        switch (msg.ch) {
            
            case Channel::CH_NULL:
                msg = InMessage();
                continue;
            case Channel::CH_CTRL: {
                vector<string> commands;
                
                commands = parseCommands(msg.message);
                
                vector<string> replies(commands.size());
                
                for (string command : commands) {
                    InMessage reply = issueCommand(command);
                }
                
                com.reply("DUMMY COMMAND REPLY");
                
                msg = reset();
                continue;
            }
            case Channel::CH_SEND: {
                lastSendTimestamp = getUnixTime();
                if (isSenderReply(msg.message)) {
                    std::cerr << "Received unexpected sender reply: " << msg
                              << std::endl;
                    msg = reset();
                    continue;
                }
                if (isSenderEvent(msg.message)) {
                    std::cout << "Sender event: " << msg << std::endl;
                    msg = reset();
                    continue;
                }
                if (isSenderData(msg.message)) {
//                    std::cout << "Sender data: " << msg << std::endl;
                    
                    processData(msg);
                    
                    msg = InMessage();
                    continue;
                }
            }
            case Channel::CH_RECV: {
                lastRecvTimestamp = getUnixTime();
                if (isReceiverReply(msg.message)) {
                    std::cerr << "Received unexpected receiver reply: " << msg
                              << std::endl;
                    msg = reset();
                    continue;
                }
                if (isReceiverEvent(msg.message)) {
                    std::cout << "Receiver event: " << msg << std::endl;
                    msg = reset();
                    continue;
                }
                if (isReceiverData(msg.message)) {
//                    std::cout << "Receiver data: " << msg << std::endl;
                    
                    processData(msg);
                    msg = InMessage();
                    continue;
                }
            }
        }
        
        std::cout << "Unidentified message " << msg << std::endl;
        
        msg = InMessage();
        if (1 == 1) {
            continue;
        } else {
            break;
        }
    }

}

