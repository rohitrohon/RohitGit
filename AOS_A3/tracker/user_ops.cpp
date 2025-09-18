#include "request_handler.h"
void handleCreateUser(const vector<string> &tokens, int sock) {
if (tokens.size() != 3) {
sendResponse(sock, "Invalid arguments for create_user");
return;
}
lock_guard<mutex> lock1(userDataMutex);
lock_guard<mutex> lock2(loginMutex);
if (userData.count(tokens[1])) {
sendResponse(sock, "User already exists");
return;
}
userData[tokens[1]] = tokens[2];
isLoggedIn[tokens[1]] = false;
sendResponse(sock, "Account created for user: " + tokens[1]);
}


void handleLogin(const vector<string> &tokens, int sock, string &currentUser, int clientPort, const string &clientIP) {
if (tokens.size() != 3) {
sendResponse(sock, "Invalid arguments for login");
return;
}
lock_guard<mutex> lock1(userDataMutex);
lock_guard<mutex> lock2(loginMutex);


if (!userData.count(tokens[1])) {
sendResponse(sock, "User does not exist");
} else if (isLoggedIn[tokens[1]]) {
sendResponse(sock, "User already logged in");
} else if (!currentUser.empty()) {
sendResponse(sock, "Already logged in as " + currentUser);
} else if (userData[tokens[1]] == tokens[2]) {
isLoggedIn[tokens[1]] = true;
currentUser = tokens[1];
userIpPort[currentUser] = {clientPort, clientIP};
sendResponse(sock, "Login successful");
} else {
sendResponse(sock, "Invalid password");
}
}


void handleLogout(const vector<string> &tokens, int sock, string &currentUser) {
if (currentUser.empty()) {
sendResponse(sock, "Not logged in");
return;
}
lock_guard<mutex> lock(loginMutex);
isLoggedIn[currentUser] = false;
userIpPort[currentUser] = {0, ""};
currentUser.clear();
sendResponse(sock, "Logged out successfully");
}