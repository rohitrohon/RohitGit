#include "request_handler.h"

void handleCreateGroup(const vector<string> &tokens, int sock, const string &currentUser) {
    if (tokens.size() != 2) return sendResponse(sock, "Invalid arguments for create_group");
    if (currentUser.empty()) return sendResponse(sock, "Login required to create group");

    lock_guard<mutex> lock1(allGroupsMutex);
    if (find(allGroups.begin(), allGroups.end(), tokens[1]) != allGroups.end()) {
        sendResponse(sock, "Group already exists");
        return;
    }

    allGroups.push_back(tokens[1]);
    groupAdmin[tokens[1]] = currentUser;
    groupMembers[tokens[1]].insert(currentUser);
    sendResponse(sock, "Group created successfully");
}

void handleListGroups(const vector<string> &tokens, int sock, const string &currentUser) {
    if (currentUser.empty()) return sendResponse(sock, "Login required to list groups");
    if (allGroups.empty()) return sendResponse(sock, "No groups exist");

    string result = "Existing groups:\n";
    for (const auto &grp : allGroups)
        result += grp + "\n";

    sendResponse(sock, result);
}

void handleJoinGroup(const vector<string> &tokens, int sock, const string &currentUser) {
    if (tokens.size() != 2) return sendResponse(sock, "Invalid arguments for join_group");
    if (currentUser.empty()) return sendResponse(sock, "Login required");

    lock_guard<mutex> lock(pendingReqMutex);
    if (find(allGroups.begin(), allGroups.end(), tokens[1]) == allGroups.end()) {
        sendResponse(sock, "Group does not exist");
    } else if (groupMembers[tokens[1]].count(currentUser)) {
        sendResponse(sock, "Already a member of group");
    } else if (pendingRequests[tokens[1]].count(currentUser)) {
        sendResponse(sock, "Join request already sent");
    } else {
        pendingRequests[tokens[1]].insert(currentUser);
        sendResponse(sock, "Join request sent for group " + tokens[1]);
    }
}

void handleListRequests(const vector<string> &tokens, int sock, const string &currentUser) {
    if (tokens.size() != 2) return sendResponse(sock, "Invalid arguments for list_requests");
    if (currentUser.empty()) return sendResponse(sock, "Login required");
    if (groupAdmin[tokens[1]] != currentUser) return sendResponse(sock, "You are not admin");

    string result = "Pending requests:\n";
    for (const auto &req : pendingRequests[tokens[1]])
        result += req + "\n";

    sendResponse(sock, result);
}

void handleAcceptRequest(const vector<string> &tokens, int sock, const string &currentUser) {
    if (tokens.size() != 3) return sendResponse(sock, "Invalid arguments for accept_request");
    if (currentUser.empty()) return sendResponse(sock, "Login required");
    if (groupAdmin[tokens[1]] != currentUser) return sendResponse(sock, "You are not admin");

    lock_guard<mutex> lock1(groupMembersMutex);
    lock_guard<mutex> lock2(pendingReqMutex);

    if (!pendingRequests[tokens[1]].count(tokens[2])) {
        sendResponse(sock, "No request from user " + tokens[2]);
        return;
    }
    groupMembers[tokens[1]].insert(tokens[2]);
    pendingRequests[tokens[1]].erase(tokens[2]);
    sendResponse(sock, "Request accepted");
}

void handleLeaveGroup(const vector<string> &tokens, int sock, const string &currentUser) {
    if (tokens.size() != 2) return sendResponse(sock, "Invalid arguments for leave_group");
    if (currentUser.empty()) return sendResponse(sock, "Login required");

    string groupId = tokens[1];
    if (find(allGroups.begin(), allGroups.end(), groupId) == allGroups.end()) {
        sendResponse(sock, "Group does not exist");
        return;
    }
    if (!groupMembers[groupId].count(currentUser)) {
        sendResponse(sock, "Not a member of group");
        return;
    }

    lock_guard<mutex> lock1(groupMembersMutex);
    groupMembers[groupId].erase(currentUser);
    if (groupMembers[groupId].empty()) {
        allGroups.erase(remove(allGroups.begin(), allGroups.end(), groupId), allGroups.end());
        groupAdmin.erase(groupId);
        pendingRequests.erase(groupId);
    }
    sendResponse(sock, "Removed from group " + groupId);
}