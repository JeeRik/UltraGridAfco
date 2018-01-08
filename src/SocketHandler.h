#ifndef SOCKETHANDLER_H
#define	SOCKETHANDLER_H

#include <string>
#include <vector>

class SocketHandler {
    public:
        /**
         * Connect client to a server
         * @param address where the server is located
         * @param port port the server is listening at
         * @return socket number
         */
        static int connectTo(const std::string address, int port);
        /**
         * Start a server socket on the specified port. Upon return, the socket
         * already queues incoming connection requests.
         * @param port port at which the server should listen for 
         * incoming connections
         * @return socket where the server listens for connections
         */
        static int startServer(int port);
        /**
         * Accept a connection that is pending on the given server socket
         * @param server server socket
         * @param usec how long accept() can wait for incoming connection
         * @return socket where the new channel was opened. -1 on failure or timeout
         */
        static int accept(int server, long usec);
        /**
         * Close the socket, either server or an opened channel
         * @param socket the socket to close
         * @return TBA
         */
        static int closeSocket(int socket);
        /**
         * Wait for data to be available on either of the sockets. Shortcut for
         * select().
         * @param sockets List of sockets to wait for
         * @param usec Max time to wait for the data
         * @return list-ID of the socket where data is available. -1 on timeout, -2 on other error
         */
        static int waitForData(const std::vector<int> &sockets, int usec);
};

#endif	/* SOCKETHANDLER_H */

