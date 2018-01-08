/* 
 * File:   comHandler.h
 * Author: maara
 *
 * Created on January 31, 2017, 9:53 AM
 */
#ifndef COMHANDLER_H
#define	COMHANDLER_H

#include <iostream>
#include <vector>

#include "StringSocket.h"




enum class Channel {
    CH_NULL = 0, CH_SEND = 1, CH_RECV = 2, CH_CTRL = 3,
};

std::ostream& operator<<(std::ostream& os, const Channel& c);


class InMessage {
private:
public:
    Channel ch;
    std::string message;
    
    bool operator==(const InMessage &that);
    bool operator!=(const InMessage &that);
    
    InMessage();
    InMessage(Channel ch, const std::string message);
};

std::ostream& operator<<(std::ostream& os, const InMessage& message);

class ComHandler {
public:

    ComHandler(int ctrlPort, int sendPort, int recvPort, const std::string recvAddr);
    int init();
    /**
     * Pull the next message in any queue. Blocking if no message is available
     * @return A message from either queue
     */
    InMessage getMessage(long usec);
    /**
     * Pull the next message from the given channel
     * @return 
     */
    InMessage getSenderMessage(long usec);
    /**
     * Query whether there is a message available on any channel without blocking
     * @return true iff there is a message in any queue
     */
    bool hasNextMessage();
    /**
     * Query whether there is a message available on the given channel without blocking
     * @return true iff there is a message in the queue
     */
    bool hasNextMessage(Channel ch);
    /**
     * Send a reply to the control socket which the last received control
     * message came from. Assuming that every control message requires exactly
     * one reply, the messages are expected to be processed one-by-one. The
     * reply therefore is always addressed to the last
     * @param text
     */
    void reply(std::string text);
    /**
     * @param command
     * @return
     */
    void sendCommand(const std::string command);
    /**
     */
    ~ComHandler();

private:
    const int ctrlPort;
    const int sendPort;
    const int recvPort;
    const std::string recvAddr;

    int sendSocket;
    int recvSocket;
    int ctrlServerSocket;
    StringSocket sendBuffer;
    StringSocket recvBuffer;
    
    int lastCtrlSocketId = 0;

    /**
     * List of all sockets opened by this handler. The value range is
     * control socket server[0], sender socket[1], receiver socket[2] and
     * connected control sockets[id+3]. The control socket ids list shrinks
     * upon socket close.
     */
    std::vector<int> sockets;
    /**
     * List of all StringBuffers maintained by this handler. The value range is
     * sender socket[0], receiver socket[1] and connected control sockets[id+2].
     * The control socket ids list shrinks upon socket close.
     */
    std::vector<StringSocket> buffers;
};

#endif	/* COMHANDLER_H */

