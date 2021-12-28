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
            std::cin.getline(buf,
                             bufsize); //Gets input from keyboard and stores in buf array, while the bufsize is the max size
            std::string line(buf);
            std::vector<std::string> input = connectionHandler.splitString(line);
            int outputLength;
            short op = connectionHandler.getOP(input[0]);

            //Logout, Stat
            if (op == 3 || op == 7) {
                if (op == 3) {
                    connectionHandler.setWaitForServerLogoutApproval(true);
                }
                char output[3];  //2 for opcode, 1 for ;
                connectionHandler.shortToBytes(op, output, 0);
                output[2] = '\n';
                connectionHandler.sendBytes(output, 3);
            }

            //Login
            else if (op == 1) {
                outputLength = input[1].size() + input[2].size() + 2 + 2 + 1 + 1; //username + password + 2(for op) + 2(for '0') + 1(for captcha) + 1(for ';')
                char output[outputLength];
                connectionHandler.shortToBytes(op, output, 0);
                connectionHandler.insertString(output, 2, input[1]); //username + '\0'
                connectionHandler.insertString(output, 2 + input[1].size() + 1, input[2]); //password + '\0'
                output[outputLength - 2] = (char) input[3][0]; //captcha
                output[outputLength - 1] = '\n';
                connectionHandler.sendBytes(output, outputLength);
            }

            //Follow / Unfollow
            else if (op == 4) {
                outputLength = input[2].size() + 2 + 1 + 1 + 1; //username + 2(opcode) + 1(follow/unfollow) + 1('\0') + 1(';')
                char output[outputLength];
                connectionHandler.shortToBytes(op, output, 0); //opcode
                if (input[1][0] == '1') { //unfollow
                    output[2] = '1';
                } else {
                    output[2] = '0'; //follow
                }
                connectionHandler.insertString(output, 3, input[2]); //username
                output[outputLength - 1] = '\n';
                connectionHandler.sendBytes(output, outputLength);
            }

            //Post, Stat
            else if (op == 5 || op == 8 || op==12) {
                outputLength = input[1].size() + 2 + 1 + 1; //string + 2(opcode) + 1('\0') + 1(';')
                char output[outputLength];
                connectionHandler.shortToBytes(op, output, 0); //opcode
                connectionHandler.insertString(output, 2, input[1]); //string + '\0'
                output[outputLength - 1] = '\n';
                connectionHandler.sendBytes(output, outputLength);
            }

            //PM, Register
            else if (op == 6 || op == 1) {
                outputLength = input[1].size() + input[2].size() + input[3].size() + 2 + 3 + 1; //username + string + string + 2(opcode) + 3('\0') + 1(';')
                char output[outputLength];
                connectionHandler.shortToBytes(op, output, 0); //opcode
                connectionHandler.insertString(output, 2, input[1]); //username + '\0'
                int nextPos = 2 + input[1].size() + 1;
                connectionHandler.insertString(output, nextPos, input[2]); //string + '\0'
                nextPos = 2 + input[1].size() + 1 + input[2].size() + 1;
                connectionHandler.insertString(output, nextPos, input[3]); //string + '\0'
                output[outputLength - 1] = '\n';
                connectionHandler.sendBytes(output, outputLength);
            }
        }
    }
    return 0;
}