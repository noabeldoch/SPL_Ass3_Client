//
// Created by noabeldoch on 28/12/2021.
//

#ifndef CLIENT_READFROMSOCKETTHREAD_H
#define CLIENT_READFROMSOCKETTHREAD_H

#include "connectionHandler.h"

class readFromSocketThread {
public:
    readFromSocketThread(ConnectionHandler &ch);
    void run();
private:
    ConnectionHandler &connectionHandler;
};


#endif //CLIENT_READFROMSOCKETTHREAD_H
