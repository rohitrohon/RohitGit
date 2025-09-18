#include "request_handler.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>

// -------------------- Global State --------------------
unordered_map<string, string> users;                  // username -> password
unordered_map<string, vector<string>> groups;         // group -> members
unordered_map<string, unordered_map<string, vector<string>>> fileTable; 
// group -> file -> vector of seeders

string stateFile = "tracker_state.txt";

// -------------------- State Persistence --------------------
void saveStateToFile() {
    ofstream out(stateFile);
    if (!out) {
        cerr << "[ERROR] Could not save state to file." << endl;
        return;
    }

    // Save users
    out << "[USERS]\n";
    for (const auto& [username, password] : users) {
        out << username << ':' << password << '\n';
    }

    // Save groups
    out << "[GROUPS]\n";
    for (const auto& [group, members] : groups) {
        out << group << ':';
        for (size_t i = 0; i < members.size(); ++i) {
            if (i > 0) out << ',';
            out << members[i];
        }
        out << '\n';
    }

    // Save file table
    out << "[FILES]\n";
    for (const auto& [group, files] : fileTable) {
        for (const auto& [filename, seeders] : files) {
            out << group << ':' << filename << ':';
            for (size_t i = 0; i < seeders.size(); ++i) {
                if (i > 0) out << ',';
                out << seeders[i];
            }
            out << '\n';
        }
    }
    
    out.close();
    cout << "[STATE] Saved tracker state to disk." << endl;
}

void loadStateFromFile() {
    ifstream in(stateFile);
    if (!in) {
        cout << "[STATE] No previous state found. Starting fresh." << endl;
        return;
    }

    string line, section;
    while (getline(in, line)) {
        if (line.empty()) continue;
        
        if (line == "[USERS]") {
            section = "USERS";
        } else if (line == "[GROUPS]") {
            section = "GROUPS";
        } else if (line == "[FILES]") {
            section = "FILES";
        } else {
            if (section == "USERS") {
                size_t pos = line.find(':');
                if (pos != string::npos) {
                    users[line.substr(0, pos)] = line.substr(pos + 1);
                }
            } else if (section == "GROUPS") {
                size_t pos = line.find(':');
                if (pos != string::npos) {
                    string group = line.substr(0, pos);
                    string members = line.substr(pos + 1);
                    stringstream ss(members);
                    string member;
                    while (getline(ss, member, ',')) {
                        if (!member.empty()) {
                            groups[group].push_back(member);
                        }
                    }
                }
            } else if (section == "FILES") {
                size_t pos1 = line.find(':');
                size_t pos2 = line.find(':', pos1 + 1);
                if (pos1 != string::npos && pos2 != string::npos) {
                    string group = line.substr(0, pos1);
                    string filename = line.substr(pos1 + 1, pos2 - pos1 - 1);
                    string seeders = line.substr(pos2 + 1);
                    stringstream ss(seeders);
                    string seeder;
                    while (getline(ss, seeder, ',')) {
                        if (!seeder.empty()) {
                            fileTable[group][filename].push_back(seeder);
                        }
                    }
                }
            }
        }
    }
    
    in.close();
    cout << "[STATE] Loaded tracker state from disk." << endl;
}

// -------------------- Main Tracker Logic --------------------
int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: ./tracker <tracker_info.txt> <tracker_id>" << endl;
        return EXIT_FAILURE;
    }

    string trackerFile = argv[1];
    int trackerId = stoi(argv[2]);

    // Load tracker info (IP, port)
    ifstream fin(trackerFile);
    if (!fin) {
        cerr << "[ERROR] Could not open tracker info file." << endl;
        return EXIT_FAILURE;
    }

    string line;
    for (int i = 0; i < trackerId; ++i) getline(fin, line); // read correct tracker line
    fin.close();

    size_t sep = line.find(':');
    string ip = line.substr(0, sep);
    int port = stoi(line.substr(sep + 1));

    cout << "[INFO] Tracker starting on " << ip << ":" << port << endl;

    // Load persisted state
    loadStateFromFile();

    // Setup socket
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    bind(serverSock, (struct sockaddr *)&addr, sizeof(addr));
    listen(serverSock, 5);

    cout << "[INFO] Tracker is listening for connections..." << endl;

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);
        int clientSock = accept(serverSock, (struct sockaddr *)&clientAddr, &addrLen);

        thread t([clientSock]() {
            handleClientRequest(clientSock, saveStateToFile);  // pass save callback
        });
        t.detach();
    }

    close(serverSock);
    return EXIT_SUCCESS;
}