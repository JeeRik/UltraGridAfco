//
// Created by maara on 3/20/17.
//

#ifndef SOCKETCOMMUNICATORCLION_UGADAPTER_H
#define SOCKETCOMMUNICATORCLION_UGADAPTER_H

#include <string>
#include <vector>
#include <map>
#include "ComHandler.h"

using namespace std;

std::ostream& operator<<(std::ostream& os, const pair<string,string> &c);
        
class UgAdapter {
private:
    ComHandler com;
    map<string, string> properties;
    map<string, string> transaction;
    bool isTransaction = false;
    
    static bool isSenderReply(const string &reply);
    
    const string passCommand(const string &command);

public:
    /**
     * @param com
     * @param command Command used to run the UltraGrid sender.
     */
    UgAdapter(ComHandler &com, const vector<string> argv);
    
    /**
     * Mark start of a new transaction. Any previous transaction is closed as
     * successfull.
     */
    void startTransaction();
    
    /**
     * Send a command to UltraGrid-sender.
     * @param command Command to send
     * @return reply received from UltraGrid
     */
    const string sendCommand(const string &command);
    
    /**
     * Roll back the current transaction. All commands properties set within the
     * cureent transaction are set to the state before transaction began.
     */
    void rollback();
    
    /**
     * Close the current transaction.
     */
    void closeTransaction();
    
    /**
     * Get value of the given property. Can not be used during an active
     * transaction.
     * @param key  property name
     * @return value configured under this key
     */
    const string getProperty(const string &key);
    
    /**
     * Execute commands as bulk in one transaction. If any command fails, the
     * transaction is rolled back. Transaction is is closed upon success.
     * @param commands Commands to send, in the order they are to be sent.
     * @return empty map on success, list of replies upon rollback
     */
    vector<string> sendTransaction(vector<string> commands);
    
    /**
     * Check whether the reply indicates success
     * @param reply reply to check
     * @return true if success
     */
    static bool isSuccessReply(const string &reply);
    
    pair<string, string> decodeCommand(const string &command);
    
    string encodeCommand(const pair<const string, const string> &command);
};

#endif //SOCKETCOMMUNICATORCLION_UGADAPTER_H
