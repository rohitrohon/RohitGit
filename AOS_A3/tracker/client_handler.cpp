#include "request_handler.h"
using namespace std;


// Dispatch client commands to appropriate handler module
void processClientCommand(const string &input, int clientSock, string &clientUser, const string &clientIP, int clientPort) {
vector<string> tokens;
stringstream ss(input);
string token;
while (ss >> token) tokens.push_back(token);


if (tokens.empty()) return;


if (tokens[0] == "create_user") {
handleCreateUser(tokens, clientSock);
} else if (tokens[0] == "login") {
handleLogin(tokens, clientSock, clientUser, clientPort, clientIP);
} else if (tokens[0] == "logout") {
handleLogout(tokens, clientSock, clientUser);
} else if (tokens[0] == "create_group") {
handleCreateGroup(tokens, clientSock, clientUser);
} else if (tokens[0] == "list_groups") {
handleListGroups(tokens, clientSock, clientUser);
} else if (tokens[0] == "join_group") {
handleJoinGroup(tokens, clientSock, clientUser);
} else if (tokens[0] == "leave_group") {
handleLeaveGroup(tokens, clientSock, clientUser);
} else if (tokens[0] == "list_requests") {
handleListRequests(tokens, clientSock, clientUser);
} else if (tokens[0] == "accept_request") {
handleAcceptRequest(tokens, clientSock, clientUser);
} else if (tokens[0] == "upload_file") {
handleUploadFile(tokens, clientSock, clientUser);
} else if (tokens[0] == "list_files") {
handleListFiles(tokens, clientSock, clientUser);
} else if (tokens[0] == "download_file") {
handleDownloadFile(tokens, clientSock, clientUser);
} else if (tokens[0] == "show_downloads") {
handleShowDownloads(tokens, clientSock, clientUser);
} else if (tokens[0] == "stop_share") {
handleStopShare(tokens, clientSock, clientUser);
} else {
sendResponse(clientSock, "Invalid command.");
}
}