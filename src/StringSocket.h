/* 
 * File:   lineReadBuffer.h
 * Author: maara
 *
 * Created on February 14, 2017, 2:41 PM
 */

#ifndef LINEREADBUFFER_H
#define	LINEREADBUFFER_H

#include <string>
/**
 * Buffer for reading from a string input per-line. Reads chunks, returns
 * individual lines and buffers the rest. Assumes that every line is written
 * fully (e.g., if there is any data in the buffer, then there is a linebreak
 * as well or will arrive soon, hence may block until a '\n' is received).
 */
class StringSocket {
public:
    StringSocket(int socket);
    /**
     * Read line from the stream. May block if no line is available.
     * @param line An empty string to which write the line
     * @return true if a line was successfully red
     */
    bool readLine(std::string &line);
    /**
     * Check whether the buffer is empty. Assumes that only entire lines are 
     * transferred. Hence, returns true even on semi-received lines as the
     * rest of the line should be available in the socket itself.
     * Does not check whether there is any data avail
     * @return true if there is any data to be read, even in the socket
     */
    bool hasBuffered();
    /**
     * Check whether there is data available, either in the buffer or in the
     * socket itself.
     * @param usec microseconds that socket poll is allowed to sleep
     * @return true if any data is available
     */
    bool hasData(long usec);
    /**
     * @return true if the underlying socket is closed
     */
    bool isClosed();
    /**
     * Write the string into socket, adding a breakline to its end for
     * distinction on the receiving end.
     * @param line string to be written
     */
    void writeLine(std::string line);
private:
    int socket;
    std::string buffer;
    bool closed = false;
};


#endif	/* LINEREADBUFFER_H */

