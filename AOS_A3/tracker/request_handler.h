#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <sstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

// Global state declarations
extern unordered_map<string, string> userData;
extern unordered_map<string, bool> isLoggedIn;
extern unordered_map<string, pair<int,string>> userIpPort;

// Group management
extern vector<string> allGroups;
extern unordered_map<string, string> groupAdmin;  // group -> admin

// Group membership and requests
extern unordered_map<string, set<string>> groupMembers;  // group -> members
extern unordered_map<string, set<string>> pendingRequests;  // group -> pending users

// File sharing
extern unordered_map<string, vector<pair<string, string>>> groupData;  // group -> vector of (filename, owner)
extern unordered_map<string, vector<string>> groupFiles;  // group -> list of files
extern unordered_map<string, vector<pair<string, pair<string, string>>>> downloaders;  // filename -> vector of (username, (filepath, group))
extern unordered_map<string, pair<string, int>> noOfChunks;  // filename -> (group, chunks)
extern unordered_map<string, string> fileHash;  // filepath -> hash

// Mutexes
extern mutex userDataMutex;
extern mutex loginMutex;
extern mutex allGroupsMutex;
extern mutex groupMembersMutex;
extern mutex pendingReqMutex;
extern mutex groupDataMutex;
extern mutex groupFilesMutex;
extern mutex dwnldVecMutex;

// Helper to send response
inline void sendResponse(int sock, const string &msg) {
    send(sock, msg.c_str(), msg.size(), 0);
}

// Request handler entry point
void handleClientRequest(int clientSock, function<void()> saveState);

// Function declarations
void handleCreateUser(const vector<string> &tokens, int sock);
void handleLogin(const vector<string> &tokens, int sock, string &currentUser, int clientPort, const string &clientIP);
void handleLogout(const vector<string> &tokens, int sock, string &currentUser);

// Group operations
void handleCreateGroup(const vector<string> &tokens, int sock, const string &currentUser);
void handleJoinGroup(const vector<string> &tokens, int sock, const string &currentUser);
void handleLeaveGroup(const vector<string> &tokens, int sock, const string &currentUser);
void handleListGroups(const vector<string> &tokens, int sock, const string &currentUser);
void handleListRequests(const vector<string> &tokens, int sock, const string &currentUser);
void handleAcceptRequest(const vector<string> &tokens, int sock, const string &currentUser);

// File operations
void handleUploadFile(const vector<string> &tokens, int sock, const string &currentUser);
void handleListFiles(const vector<string> &tokens, int sock, const string &currentUser);
void handleDownloadFile(const vector<string> &tokens, int sock, const string &currentUser);
void handleStopShare(const vector<string> &tokens, int sock, const string &currentUser);
void handleShowDownloads(const vector<string> &tokens, int sock, const string &currentUser);

#endif
