#include "client_handler.h"


void processUserCommand(const string &input, TrackerConnection &tracker, int clientPort) {
vector<string> tokens;
stringstream ss(input);
string token;
while (ss >> token) tokens.push_back(token);


if (tokens.empty()) return;


if (tokens[0] == "upload_file") {
string sha1 = computeSHA1(tokens[1]);
string command = input + " " + sha1;
tracker.sendCommand(command);
} else if (tokens[0] == "download_file") {
tracker.sendCommand(input);
string response = tracker.receiveResponse();
parseSeederInfo(response, tokens[2]);
} else {
tracker.sendCommand(input);
cout << tracker.receiveResponse() << endl;
}
}


void parseSeederInfo(const string &info, const string &fileName) {
// Expected format: seederCount-chunkCount-hash-ip1-port1-path1@ip2-port2-path2@
vector<string> parts;
stringstream ss(info);
string segment;
while (getline(ss, segment, '-')) parts.push_back(segment);


if (parts.size() < 4) {
cerr << "[ERROR] Invalid seeder info format." << endl;
return;
}


int seederCount = stoi(parts[0]);
int chunkCount = stoi(parts[1]);
string fileHash = parts[2];
string peersInfo = parts[3];


// Split peersInfo by '@'
vector<string> seeders;
stringstream ss2(peersInfo);
while (getline(ss2, segment, '@')) {
if (!segment.empty()) seeders.push_back(segment);
}


if (seeders.empty()) {
cerr << "[WARN] No active seeders found." << endl;
return;
}


// Open output file
ofstream out(fileName, ios::binary | ios::trunc);
if (!out) {
cerr << "[ERROR] Could not open file for writing: " << fileName << endl;
return;
}


// For each chunk, pick a seeder and request it
for (int chunk = 0; chunk < chunkCount; ++chunk) {
// Pick seeder in round-robin
string seeder = seeders[chunk % seeders.size()];
stringstream sss(seeder);
string portStr, ip, path;
getline(sss, portStr, '-');
getline(sss, ip, '-');
getline(sss, path, '-');


int seederPort = stoi(portStr);
int peerSock = socket(AF_INET, SOCK_STREAM, 0);
sockaddr_in peerAddr{};
peerAddr.sin_family = AF_INET;
peerAddr.sin_port = htons(seederPort);
inet_pton(AF_INET, ip.c_str(), &peerAddr.sin_addr);


if (connect(peerSock, (struct sockaddr *)&peerAddr, sizeof(peerAddr)) < 0) {
cerr << "[WARN] Could not connect to seeder " << ip << endl;
close(peerSock);
continue;
}


// Send request: file path + chunk number
string req = path + " " + to_string(chunk);
send(peerSock, req.c_str(), req.size(), 0);


vector<char> buffer(CHUNK_SIZE);
int bytesReceived = read(peerSock, buffer.data(), CHUNK_SIZE);
if (bytesReceived > 0) {
    out.write(buffer.data(), bytesReceived);

    // 🔽 Add progress bar here
    double progress = ((double)(chunk + 1) / chunkCount) * 100.0;
    int barWidth = 50;
    int pos = (progress / 100.0) * barWidth;

    cout << "[DOWNLOAD] [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << "#";
        else cout << "-";
    }
    cout << "] " << fixed << setprecision(1) << progress << "%\r";
    cout.flush();
}
cout << endl;


close(peerSock);
}


out.close();


// Compute hash of downloaded file
string downloadedHash = computeSHA1(fileName);
if (downloadedHash == fileHash) {
cout << "[SUCCESS] File downloaded and verified." << endl;
} else {
cerr << "[ERROR] Hash mismatch! File may be corrupted." << endl;
}
}