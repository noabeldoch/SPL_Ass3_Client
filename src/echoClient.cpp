#include <stdlib.h>
#include <thread>
#include "../include/connectionHandler.h"
#include "../include/readFromSocketThread.h"

/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/

int main (int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);

    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }

    //Thread read from socket, start running upon creation
    readFromSocketThread readSocket(connectionHandler);
    std::thread socketThread(&readFromSocketThread::run, &readSocket);

    //Using 2 while loops in order to ensure that after the user tried to log out he won't be able to type anymore
    while(!connectionHandler.getServerApprovedLogout()) {

        while (!connectionHandler.getWaitForServerLogoutApproval()) {
            const short bufsize = 1024;
            char buf[bufsize];
            std::cin.getline(buf, bufsize); //Gets input from keyboard and stores in buf array, while the bufsize is the max size
            std::string line(buf);
            int len=line.length();
            if (!connectionHandler.sendLine(line)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            // connectionHandler.sendLine(line) appends '\n' to the message. Therefor we send len+1 bytes.
            std::cout << "Sent " << len+1 << " bytes to server" << std::endl; //It was given, check whether it should be deleted ??????????????
        }
    }

    return 0;
}