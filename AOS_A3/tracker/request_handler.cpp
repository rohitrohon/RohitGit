#include "request_handler.h"
#include <sstream>
#include <unistd.h>

void handleClientRequest(int clientSock, function<void()> saveState) {
    char buffer[2048] = {0};
    int n = read(clientSock, buffer, sizeof(buffer));
    if (n <= 0) {
        close(clientSock);
        return;
    }

    string request(buffer);
    stringstream ss(request);
    string command;
    ss >> command;

    vector<string> tokens;
    string token;
    tokens.push_back(command);
    while (ss >> token) tokens.push_back(token);

    // Each client thread maintains its own state
    static thread_local string currentUser;
    static thread_local string clientIP;
    static thread_local int clientPort = 0;

    if (command == "create_user") {
        handleCreateUser(tokens, clientSock);
        saveState();

    } else if (command == "login") {
        // Extract client IP and port from socket if not already set
        if (clientIP.empty()) {
            sockaddr_in addr;
            socklen_t len = sizeof(addr);
            getpeername(clientSock, (struct sockaddr *)&addr, &len);
            clientIP = inet_ntoa(addr.sin_addr);
            clientPort = ntohs(addr.sin_port);
        }

        handleLogin(tokens, clientSock, currentUser, clientPort, clientIP);
        saveState();

    } else if (command == "logout") {
        handleLogout(tokens, clientSock, currentUser);
        saveState();

    } else if (command == "create_group") {
        handleCreateGroup(tokens, clientSock);
        saveState();

    } else if (command == "join_group") {
        handleJoinGroup(tokens, clientSock);
        saveState();

    } else if (command == "leave_group") {
        handleLeaveGroup(tokens, clientSock, currentUser);
        saveState();

    } else if (command == "list_groups") {
        handleListGroups(clientSock);

    } else if (command == "list_requests") {
        handleListRequests(tokens, clientSock);

    } else if (command == "accept_request") {
        handleAcceptRequest(tokens, clientSock);
        saveState();

    } else if (command == "upload_file") {
        handleUploadFile(tokens, clientSock, currentUser);
        saveState();

    } else if (command == "list_files") {
        handleListFiles(tokens, clientSock);

    } else if (command == "download_file") {
        handleDownloadFile(tokens, clientSock);

    } else if (command == "stop_share") {
        handleStopShare(tokens, clientSock, currentUser);
        saveState();

    } else {
        sendResponse(clientSock, "[ERROR] Unknown command: " + command);
    }

    close(clientSock);
}