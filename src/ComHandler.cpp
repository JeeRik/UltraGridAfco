#include "SocketHandler.h"
#include "ComHandler.h"
#include "Tools.h"

using namespace std;

std::ostream& operator<<(std::ostream& os, const Channel& c) {
    switch (c) {
        case Channel::CH_NULL: os << "CH_NULL";
            break;
        case Channel::CH_CTRL: os << "CH_CTRL";
            break;
        case Channel::CH_SEND: os << "CH_SEND";
            break;
        case Channel::CH_RECV: os << "CH_RECV";
            break;
        default: os.setstate(std::ios_base::failbit);
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const InMessage& message) {
    return os << message.ch << ":\"" << message.message << "\"";
}

bool InMessage::operator==(const InMessage &that) {
    return ch == that.ch && message == that.message;
}

bool InMessage::operator!=(const InMessage &that) {
    return ! (this->operator==(that));
}

InMessage::InMessage() : InMessage(Channel::CH_NULL, "") {}

InMessage::InMessage(Channel ch, const std::string message) : ch(ch), message(ch==Channel::CH_NULL ? "" : message) {
    if (ch == Channel::CH_NULL && (message != "")) {
        std::cerr << "Creating non-empty NULL message not allowed!" << std::endl;
    }
};


ComHandler::ComHandler(int ctrlPort, int sendPort, int recvPort, const std::string recvAddr) :
        ctrlPort(ctrlPort),
        sendPort(sendPort),
        recvPort(recvPort),
        recvAddr(recvAddr),
        sendBuffer(-1),
        recvBuffer(-1)
{
}

int ComHandler::init() {
    // Start CTRL server
    ctrlServerSocket = SocketHandler::startServer(ctrlPort);
    sendSocket = SocketHandler::connectTo("localhost", sendPort);
    recvSocket = SocketHandler::connectTo(recvAddr, recvPort);
    if (ctrlServerSocket <=0 || sendSocket <= 0 || recvSocket <= 0) {
        std::cerr << "ComHandler failed to initialize communication, aborting" << std::endl;
        return -1;
    }
    
    
    sendBuffer = StringSocket(sendSocket);
    recvBuffer = StringSocket(recvSocket);
    
    sockets.push_back(ctrlServerSocket);
    sockets.push_back(sendSocket);
    sockets.push_back(recvSocket);
    
    buffers.push_back(sendBuffer);
    buffers.push_back(recvBuffer);
    
    return 0;
}

InMessage ComHandler::getMessage(long usec) {

    long deadline = getUnixTime() + usec;
    
    while (getUnixTime() < deadline) {
    
        int id = SocketHandler::waitForData(sockets, deadline - getUnixTime());
    
        if (id <= 0) {
            if (id < 0) {
//                std::cerr << "Waiting for data timed out" << std::endl;
                return InMessage();
            }
            
            std::cout << "A new connection detected at server, accepting" << std::endl;
            
            int socket = SocketHandler::accept(ctrlServerSocket, 0);
            
            if (socket < 0) {
                std::cerr << "Failed to create connection" << std::endl;
                continue;
            }
            
            StringSocket buffer(socket);
            
            buffers.push_back(buffer);
            sockets.push_back(socket);
            
            continue;
            
        } else {
            
            std::string text;
            
            if (! buffers[id-1].readLine(text)) {
                
                std::cout << "Data read failed" << std::endl;
                
                if (! buffers[id-1].isClosed()) {
                    continue;
                }
                
                std::cerr << "Sockets[" << id << "] closed" << std::endl;
                if (id == 1 || id == 2) {
                    std::cerr << (id == 1 ? "Sender" : "Receiver") << " socket closed, exiting" << std::endl;
                    exit(1);
                }
    
                buffers.erase(buffers.begin() + (id-1));
                sockets.erase(sockets.begin() + (id));
            }
            
            InMessage ret = InMessage( (id > 2 ? Channel::CH_CTRL : (id == 1 ? Channel::CH_SEND : Channel::CH_RECV)), text);
            
            if (id > 2) lastCtrlSocketId = id;
            
            return ret;
        }
    }
    
    return InMessage();
//    StringSocket statsBuffer = buffers[id-1];
//
//    string text;
//    if (!statsBuffer.readLine(text)) {
//        std::cerr << "Reading line from statsBuffer " << statsBuffer.socket << " failed" << std::endl;
//        return NULL;
//    }
//
//    return new InMessage(ch, text);
    return InMessage();
}

InMessage ComHandler::getSenderMessage(long usec) {
    
    long deadline = getUnixTime() + usec;
    
    while (getUnixTime() < deadline) {
        int id = SocketHandler::waitForData({sendSocket}, deadline - getUnixTime());

        if (id == 0) {
            string line;
            if (! sendBuffer.readLine(line)) {
                if (sendBuffer.isClosed()) {
                    std::cerr << "Failed to read from sender; exiting" << std::endl;
                    exit(1);
                }
                continue;
            }
            InMessage ret = InMessage(Channel::CH_SEND, line);
            return ret;
        } else {
            
        }
    }
    
    return InMessage();
}

bool ComHandler::hasNextMessage() {
    return 0;
}

bool ComHandler::hasNextMessage(Channel ch) {
    (void) ch;
    return 0;
}

void ComHandler::sendCommand(const std::string command) {
    sendBuffer.writeLine(command);
}

void ComHandler::reply(std::string text) {
    buffers[lastCtrlSocketId-1].writeLine(text);
}


ComHandler::~ComHandler() {
    for (int socket : sockets) {
        SocketHandler::closeSocket(socket);
    }
}
