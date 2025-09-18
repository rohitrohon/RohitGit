#include "request_handler.h"

void handleUploadFile(const vector<string> &tokens, int sock, const string &currentUser) {
    if (tokens.size() != 5) return sendResponse(sock, "Invalid arguments for upload_file");
    if (currentUser.empty()) return sendResponse(sock, "Login required");

    string groupId = tokens[2];
    if (find(allGroups.begin(), allGroups.end(), groupId) == allGroups.end())
        return sendResponse(sock, "Group does not exist");
    if (!groupMembers[groupId].count(currentUser))
        return sendResponse(sock, "You are not a member of this group");

    lock_guard<mutex> lock1(groupDataMutex);
    lock_guard<mutex> lock2(groupFilesMutex);

    pair<string,string> fileOwnerPair = {tokens[1], currentUser};
    auto &vec = groupData[groupId];
    if (find(vec.begin(), vec.end(), fileOwnerPair) != vec.end())
        return sendResponse(sock, "You have already uploaded this file");

    // Register file
    vec.push_back(fileOwnerPair);

    // Extract filename from path
    size_t pos = tokens[1].find_last_of("/");
    string fileName = (pos != string::npos) ? tokens[1].substr(pos+1) : tokens[1];
    groupFiles[groupId].push_back(fileName);
    downloaders[fileName].push_back({currentUser,{tokens[1],groupId}});
    noOfChunks[fileName] = {groupId, stoi(tokens[4])};
    fileHash[tokens[1]] = tokens[3];

    sendResponse(sock, "File uploaded successfully");
}

void handleListFiles(const vector<string> &tokens, int sock, const string &currentUser) {
    if (tokens.size() != 2) return sendResponse(sock, "Invalid arguments for list_files");
    string groupId = tokens[1];
    if (find(allGroups.begin(), allGroups.end(), groupId) == allGroups.end())
        return sendResponse(sock, "Group does not exist");
    if (!groupMembers[groupId].count(currentUser))
        return sendResponse(sock, "You are not a member of this group");

    string result = "Files in group:\n";
    for (const auto &file : groupFiles[groupId])
        result += file + "\n";

    sendResponse(sock, result);
}

static string buildSeederInfo(const vector<string> &tokens) {
    string message;
    int chunkCount = noOfChunks[tokens[2]].second;
    int seederCount = 0;

    for (const auto &entry : downloaders[tokens[2]]) {
        if (isLoggedIn[entry.first]) {
            seederCount++;
            message += to_string(userIpPort[entry.first].first) + "-" +
                       userIpPort[entry.first].second + "-" +
                       entry.second.first + "@";
        }
    }

    return to_string(seederCount) + "-" + to_string(chunkCount) + "-" + tokens[3] + "-" + message;
}

void handleDownloadFile(const vector<string> &tokens, int sock, const string &currentUser) {
    if (tokens.size() != 4) return sendResponse(sock, "Invalid arguments for download_file");
    string groupId = tokens[1];
    if (find(allGroups.begin(), allGroups.end(), groupId) == allGroups.end())
        return sendResponse(sock, "Group does not exist");
    if (!groupMembers[groupId].count(currentUser))
        return sendResponse(sock, "You are not a member of this group");
    if (find(groupFiles[groupId].begin(), groupFiles[groupId].end(), tokens[2]) == groupFiles[groupId].end())
        return sendResponse(sock, "File not found in group");

    // Send seeder info
    string msg = buildSeederInfo(tokens);
    sendResponse(sock, msg);

    // Track download progress
    lock_guard<mutex> lock(dwnldVecMutex);
    downloadStatus.push_back({"[D]", groupId, tokens[2]});

    // Read back hash from client (verify integrity)
    char buffer[1024] = {0};
    int valread = read(sock, buffer, sizeof(buffer));
    string clientHash(buffer);

    string response = "no";
    for (const auto &entry : groupData[groupId]) {
        if (entry.first.find(tokens[2]) != string::npos && fileHash[entry.first] == clientHash) {
            response = "yes";
            break;
        }
    }

    sendResponse(sock, response);

    // Update status
    if (response == "yes") {
        downloadStatus.erase(remove_if(downloadStatus.begin(), downloadStatus.end(),
                                       [&](const DownloadStat &d) {
                                           return d.progress == "[D]" && d.groupId == groupId && d.fileName == tokens[2];
                                       }),
                             downloadStatus.end());
        downloadStatus.push_back({"[C]", groupId, tokens[2]});
        groupData[groupId].push_back({tokens[3], currentUser});
        downloaders[tokens[2]].push_back({currentUser,{tokens[3],groupId}});
    } else {
        downloadStatus.erase(remove_if(downloadStatus.begin(), downloadStatus.end(),
                                       [&](const DownloadStat &d) {
                                           return d.progress == "[D]" && d.groupId == groupId && d.fileName == tokens[2];
                                       }),
                             downloadStatus.end());
    }
}

void handleShowDownloads(const vector<string> &tokens, int sock, const string &currentUser) {
    string result;
    for (const auto &d : downloadStatus)
        result += d.progress + " " + d.groupId + " " + d.fileName + "\n";

    if (result.empty()) result = "No downloads yet";
    sendResponse(sock, result);
}

void handleStopShare(const vector<string> &tokens, int sock, const string &currentUser) {
    if (tokens.size() != 3) return sendResponse(sock, "Invalid arguments for stop_share");
    string fileName = tokens[2];

    lock_guard<mutex> lock(dlListMutex);
    auto &list = downloaders[fileName];
    list.erase(remove_if(list.begin(), list.end(),
                         [&](const auto &p) { return p.first == currentUser; }),
               list.end());

    sendResponse(sock, "Stopped sharing " + fileName);
}
