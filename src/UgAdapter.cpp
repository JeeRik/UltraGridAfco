//
// Created by maara on 3/21/17.
//

#include <iostream>
#include <set>
#include "UgAdapter.h"

const map<string,string> UG_DEFAULT_PROPERTIES({
            pair<string,string>("_state","play"),
            pair<string,string>("compress", "none"),
            pair<string,string>("fec-video","none"),
            pair<string,string>("stats","off")});

const set<string> UG_MONITORED_COMMANDS({"pause", "play", "compress", "stats", "fec-video"});

std::ostream& operator<<(std::ostream& os, const pair<string,string> &c) {
    os << c.first << ":" << c.second;
    return os;
}

bool UgAdapter::isSenderReply(const string &text) {
    
    if (text.size() == 0) {
        return false;
    }
    char c = text.c_str()[0];
    bool ret = c >= 48 && c < 58;
    return ret;
}


UgAdapter::UgAdapter(ComHandler &com, const vector<string> argv) : com(com) {
    properties.insert(UG_DEFAULT_PROPERTIES.begin(), UG_DEFAULT_PROPERTIES.end());
    
    for (u_int i = 0; i < argv.size(); i++) {
        
        if (argv[i] == "-c") {
            properties["compress"] = argv[i+1];
            i++;
            continue;
        }
        
        if (argv[i] == "-f") {
            properties["fec-video"] = argv[i+1];
            i++;
            continue;
        }
        
        if (argv[i] == "--start-paused") {
            properties["_state"] = "pause";
            continue;
        }
    }
}

int commandCount = 0;
bool UgAdapter::isSuccessReply(const string &reply) {
    
    return ++commandCount < 3;
    
    return reply[0] == '2';
}

const string UgAdapter::passCommand(const string &command) {
    com.sendCommand(command);
    
    InMessage reply;
    
    while (! isSenderReply(reply.message)) {
        reply = com.getSenderMessage(1000000);
    }
    
    return reply.message;
}

pair<string,string> UgAdapter::decodeCommand(const string &command) {
    auto delim = command.find(' ');
    if (delim==string::npos) {
        delim=command.size();
    }
    
    string key = command.substr(0, delim);
    
    if (UG_MONITORED_COMMANDS.find(key) == UG_MONITORED_COMMANDS.end()) {
        std::cout << "Non-monitored command \"" << command << "\"" << std::endl;
        return pair<string,string>("", "");
    }
    
    if (delim >= command.length()-1) {
        string key = command.substr(0, delim);
        if (key == "play") {
            return pair<string,string>("_state", "play");
        }
        if (key == "pause") {
            return pair<string,string>("_state", "pause");
        }
        
        std::cerr << "Unknown command: \"" << command << "\"" << std::endl;
        return pair<string,string>("","");
    }
    
    string value = command.substr(delim+1, command.length());
    
    return pair<string,string>(key, value);
};

string UgAdapter::encodeCommand(const pair<const string, const string> &command) {
    if (properties.find(command.first) == properties.end()) {
        std::cerr << "Encoding unknown property [" << command.first << ", " << command.second << "]" << std::endl;
        return "";
    }
    
    if (command.first == "_state") {
        return command.second;
    }
    
    return command.first + " " + command.second;
}

void UgAdapter::startTransaction() {
    if (isTransaction) {
        std::cerr << "Starting new transaction while the previous was not closed. Closing previous transaction" << std::endl;
        closeTransaction();
    }
    transaction.clear();
    isTransaction = true;
}

const string UgAdapter::sendCommand(const string &command) {
    
    pair<string,string> parsed = decodeCommand(command);
    
    if (parsed == pair<string,string>("","")) {
        std::cout << "Issuing unknown command \"" << command << "\", sending without transaction" << std::endl;
        return passCommand(command);
    }
    
    if (! isTransaction) {
        std::cerr << "Cannot send monitored command without transaction: " << command << std::endl;
        return "";
    }
    
    string reply = passCommand(command);
    
    transaction[parsed.first] = parsed.second;
    
    return reply;
}

void UgAdapter::rollback() {
    
    for (pair<string,string> it : transaction) {
        std::cout << "Reverting command \"" << it.first << " " << it.second
                  << "\"" << std::endl;
        string orig = properties[it.first];
        std::cout << "  reverting to value \"" << orig << "\"" << std::endl;
        string command = it.first + " " + orig;
        std::cout << "  command: \"" << command << "\"" << std::endl;
        string reply = passCommand(command);
        std::cout << "  reply:  \"" << reply << "\"" << std::endl;
    
        if (!isSuccessReply(reply)) {
            std::cerr << "Rollback command failed with reply \"" << reply
                      << "\", ignoring failure (command: \"" << command << "\")"
                      << std::endl;
        }
    }
    
    isTransaction = false;
    transaction.clear();
}

void UgAdapter::closeTransaction() {
    isTransaction = false;
    properties.insert(transaction.begin(), transaction.end());
    transaction.clear();
}

const string UgAdapter::getProperty(const string &key) {
    return properties.at(key);
}

vector<string> UgAdapter::sendTransaction(const vector<string> commands) {
    
    vector<string> ret;
    
    if (isTransaction) {
        std::cerr << "Cannot send transaction: Another transaction in progress" << std::endl;
        return {};
    }
    
    startTransaction();
    
    for (string command : commands) {
        string reply = sendCommand(command);
        
        ret.insert(ret.end(), reply);
        
        if (! isSuccessReply(reply)) {
            std::cerr << "Command failed, aborting transaction. Command: \"" << command << "\", reply: \"" << reply << "\"" << std::endl;
            rollback();
            return ret;
        }
    }
    
    closeTransaction();
    return ret;
}

