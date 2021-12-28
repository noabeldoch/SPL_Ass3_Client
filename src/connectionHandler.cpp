#include "../include/connectionHandler.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using namespace std;

ConnectionHandler::ConnectionHandler(string host, short port):
host_(host), port_(port), io_service_(), socket_(io_service_), serverApprovedLogout(false), waitForServerLogoutApproval(false){}

ConnectionHandler::~ConnectionHandler() {
    close();
}

bool ConnectionHandler::connect() {
    std::cout << "Starting connect to "
              << host_ << ":" << port_ << std::endl;
    try {
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
        boost::system::error_code error;
        socket_.connect(endpoint, error);
        if (error)
            throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
            tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);
        }
        if(error)
            throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
            tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
        if(error)
            throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, '\n');
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, '\n');
}

bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character. 
    // Notice that the null character is not appended to the frame string.
    try {
        do{
            getBytes(&ch, 1);
            frame.append(1, ch);
        }while (delimiter != ch);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
    bool result=sendBytes(frame.c_str(),frame.length());
    if(!result) return false;
    return sendBytes(&delimiter,1);
}

// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}

bool ConnectionHandler::getServerApprovedLogout() {
    return serverApprovedLogout;
}

void ConnectionHandler::setServerApprovedLogout(bool answer) {
    serverApprovedLogout=answer;
}

bool ConnectionHandler::getWaitForServerLogoutApproval() {
    return waitForServerLogoutApproval;
}

void ConnectionHandler::setWaitForServerLogoutApproval(bool answer){
    waitForServerLogoutApproval=answer;
}

vector<string> ConnectionHandler::splitString(string &s){
    string delimiter = " ";
    vector<string> ans;
    size_t position = 0;
    string current;
    // find() returns the first instance of the delimiter, if it doesn't exist return npos
    while ((position = s.find(delimiter)) != std::string::npos) {
        current = s.substr(0, position);
        ans.push_back(current);
        s.erase(0, position + delimiter.length());
    }
    //In case find() returned npos - push the last string
    ans.push_back(s);
    return ans;
}

short ConnectionHandler::getOP(basic_string<char> &basicString){
    if (basicString == "REGISTER") return 1;
    if (basicString == "LOGIN") return 2;
    if (basicString == "LOGOUT") return 3;
    if (basicString == "FOLLOW") return 4;
    if (basicString == "POST") return 5;
    if (basicString == "PM") return 6;
    if (basicString == "LOGSTAT") return 7;
    if (basicString == "STAT") return 8;
    if (basicString == "NOTIFICATION") return 9;
    if (basicString == "ACK") return 10;
    if (basicString == "ERROR") return 11;
    if (basicString == "BLOCK") return 12;

    return 0;
}

void ConnectionHandler::shortToBytes(short num, char *bytesArr, int from) {
    bytesArr[from] = ((num >> 8) & 0xFF);
    bytesArr[from + 1] = (num & 0xFF);
}

void ConnectionHandler::insertString(char *bytes, int from, string &s) {
    for (size_t i = 0; i < s.size(); ++i) {
        bytes[i + from] = (char) s[i];
    }
    bytes[from + s.size()] = '\0';
}

short ConnectionHandler::bytesToShort(char *bytesarray, int from){
    short result = (short) ((bytesarray[from] & 0xff) << 8);
    result += (short) (bytesarray[from + 1] & 0xff);
    return result;
}

