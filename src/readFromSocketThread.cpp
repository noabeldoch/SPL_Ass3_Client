//
// Created by noabeldoch on 28/12/2021.
//

#include "readFromSocketThread.h"

readFromSocketThread::readFromSocketThread(ConnectionHandler &ch): connectionHandler(ch) {}

void readFromSocketThread::run() {
    while (!connectionHandler.getServerApprovedLogout()) {
        std::string answer;
        char bytes[2];
        connectionHandler.getBytes(bytes,2);
        short firstOP = connectionHandler.bytesToShort(bytes,0);

        //Error
        if(firstOP==11) {
            connectionHandler.getBytes(bytes,2);
            short secondOP = connectionHandler.bytesToShort(bytes,0);
            answer = "ERROR " + std::to_string(secondOP) + "\n";

            //Server didn't approve logout
            if (secondOP==3) {
                connectionHandler.setWaitForServerLogoutApproval(false);
            }
        }

        //Notification
        else if(firstOP==9) {

            answer = "NOTIFICATION ";

            //Byte representing PM/Public
            char byte[1];
            connectionHandler.getBytes(byte,1);
            short action = (short) ((byte[0] & 0xff) << 8);
            if (action==1) {
                answer = answer + "Public ";
            }
            else {
                answer = answer + "PM ";
            }

            //Posting user
            connectionHandler.getBytes(byte,1);
            while (byte[0]!='\0') {
                answer = answer + byte;
                connectionHandler.getBytes(byte,1);
            }
            answer = answer + " ";

            //Content
            connectionHandler.getBytes(byte,1);
            while (byte[0]!='\0') {
                answer = answer + byte;
                connectionHandler.getBytes(byte,1);
            }
            answer = answer + "\n";
        }

        //ACK
        else {
            connectionHandler.getBytes(bytes,2);
            short secondOP = connectionHandler.bytesToShort(bytes,0);
            answer = "ACK " + std::to_string(secondOP) + " ";

            //Follow
            if (secondOP==4) {

                //Username
                char byte[1];
                connectionHandler.getBytes(byte,1);
                while (byte[0]!='\0') {
                    answer = answer + byte;
                    connectionHandler.getBytes(byte,1);
                }
                answer = answer + "\n";
            }

            //Stat, Logstat
            else if (secondOP==8 || secondOP==7) {

                //Age
                connectionHandler.getBytes(bytes,2);
                short age = connectionHandler.bytesToShort(bytes,0);
                answer = answer + std::to_string(age) + " ";

                //NumPosts
                connectionHandler.getBytes(bytes,2);
                short NumPosts = connectionHandler.bytesToShort(bytes,0);
                answer = answer + std::to_string(NumPosts) + " ";

                //NumFollowers
                connectionHandler.getBytes(bytes,2);
                short NumFollowers = connectionHandler.bytesToShort(bytes,0);
                answer = answer + std::to_string(NumFollowers) + " ";

                //NumFollowing
                connectionHandler.getBytes(bytes,2);
                short NumFollowing = connectionHandler.bytesToShort(bytes,0);
                answer = answer + std::to_string(NumFollowing) + "\n";
            }

            //Generic ACK
            else {
                answer.resize(answer.length()-1); // Cancel the " "
                answer = answer + "\n";

                //Server approved logout
                if (secondOP==3) {
                    connectionHandler.setServerApprovedLogout(true);
                }
            }
        }
        std::cout << answer << std::endl;
    }
}