#include <iostream>
#include <unistd.h>
#include <cstring>

#include "StringSocket.h"

StringSocket::StringSocket(int socket) : socket(socket) {
}

bool StringSocket::hasBuffered() {
    return buffer.length();
}

bool StringSocket::hasData(long usec) {
    if (hasBuffered() > 0) return true;

    struct timeval tv;
    fd_set readfds;

    tv.tv_sec = 0;
    tv.tv_usec = usec;

    FD_ZERO(&readfds);
    FD_SET(socket, &readfds);

    select(socket+1, &readfds, NULL, NULL, &tv);

    bool ret = FD_ISSET(socket, &readfds);
    return ret;
    
}
    
bool StringSocket::readLine(std::string &line) {
    line = "";
    
    if (closed) {
        std::cerr << "Reading data from a closed socket" << std::endl;
        return false;
    }
    
    while (true) {
        while (buffer.find('\n') == std::string::npos &&
               buffer.find('\n') == std::string::npos) {
        
            char buf[1025];
            int n = read(socket, buf, 1024);
            if (n == -1) { // handle errors
                line = buffer;
                buffer = "";
                closed = true;
                return false;
            }
            buf[n] = 0;
            buffer += buf;
        
            if (n == 0) {
                // Upon clean socket close, a line was already forwarded
                closed=true;
                return false;
            }
        }
    
        size_t posn = buffer.find('\n');
        size_t posr = buffer.find('\r');
    
        size_t pos = posn < posr ? posn : posr;
    
        if (pos == 0) {
            buffer.erase(buffer.begin());
            if (! hasData(0)) {
                return false;
            }
            continue;
        }
    
        line = buffer.substr(0, pos);
        buffer = buffer.substr(pos + 1);
        return true;
    }
}

bool StringSocket::isClosed() {
    return closed;
}

void StringSocket::writeLine(const std::string line) {
    if (line.length() == 0) {
        std::cout << "Sending empty string to socket " << socket << " ignored" << std::endl;
        return;
    }
    write(socket, line.c_str(), line.size());
    if (line[line.length() - 1] != '\n') {
        write(socket, "\n", 1);
    }
}
