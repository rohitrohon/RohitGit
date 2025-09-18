#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <openssl/sha.h>

using namespace std;


#define CHUNK_SIZE 512*1024


// Tracker connection abstraction
class TrackerConnection {
public:
    TrackerConnection(const string &trackerFile, const string &clientIP, int clientPort);
    bool connectToTracker();              // Connect to primary or secondary tracker
    bool sendCommand(const string &cmd);  // Sends command with failover retry
    string receiveResponse() const;       // Reads tracker response
    void closeConnection() const;

private:
    string trackerFilePath;
    string primaryLine, secondaryLine;
    string currentTrackerIP;
    int currentTrackerPort;
    int trackerSock;
    string clientIP;
    int clientPort;

    bool tryConnect(const string &trackerLine);
    void sendRegistration(); // Resend IP:Port after reconnect
};


// Peer server
void startPeerListener(int clientPort);
void servePeerRequest(int peerSock);
void sendRequestedChunk(int sock, const string &req);


// File utils
string computeSHA1(const string &filePath);


// Client request processing
void processUserCommand(const string &input, TrackerConnection &tracker, int clientPort);
void parseSeederInfo(const string &info, const string &fileName);


// Helpers
inline string getLocalIPAddress() {
return "127.0.0.1"; // Simplified for local testing
}


#endif