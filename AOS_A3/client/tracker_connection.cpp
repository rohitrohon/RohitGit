#include "client_handler.h"

TrackerConnection::TrackerConnection(const string &trackerFile, const string &ip, int port)
    : trackerFilePath(trackerFile), clientIP(ip), clientPort(port), trackerSock(-1) {}

bool TrackerConnection::tryConnect(const string &trackerLine) {
    size_t sep = trackerLine.find(':');
    if (sep == string::npos) return false;

    string ip = trackerLine.substr(0, sep);
    int port = stoi(trackerLine.substr(sep + 1));

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) return false;

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
        trackerSock = sock;
        currentTrackerIP = ip;
        currentTrackerPort = port;
        cout << "[INFO] Connected to tracker: " << ip << ":" << port << endl;
        sendRegistration();
        return true;
    }

    close(sock);
    return false;
}

void TrackerConnection::sendRegistration() {
    string ipPortStr = clientIP + ":" + to_string(clientPort);
    send(trackerSock, ipPortStr.c_str(), ipPortStr.size(), 0);
}

bool TrackerConnection::connectToTracker() {
    ifstream fin(trackerFilePath);
    if (!fin) return false;

    getline(fin, primaryLine);
    getline(fin, secondaryLine);
    fin.close();

    // Try primary first, then secondary
    if (!primaryLine.empty() && tryConnect(primaryLine)) return true;
    if (!secondaryLine.empty() && tryConnect(secondaryLine)) return true;

    cerr << "[ERROR] Could not connect to any tracker." << endl;
    return false;
}

bool TrackerConnection::sendCommand(const string &cmd) {
    if (trackerSock < 0) return false;

    int result = send(trackerSock, cmd.c_str(), cmd.size(), 0);
    if (result < 0) {
        cerr << "[WARN] Lost connection to tracker. Attempting failover..." << endl;
        close(trackerSock);

        // Try to reconnect (prefer secondary if primary was used)
        bool connected = false;
        if (currentTrackerIP == primaryLine.substr(0, primaryLine.find(':'))) {
            connected = tryConnect(secondaryLine);
        } else {
            connected = tryConnect(primaryLine);
        }

        if (connected) {
            cout << "[INFO] Reconnected to alternate tracker successfully." << endl;
            return send(trackerSock, cmd.c_str(), cmd.size(), 0) >= 0;
        } else {
            cerr << "[ERROR] All trackers unreachable!" << endl;
            return false;
        }
    }
    return true;
}

string TrackerConnection::receiveResponse() const {
    char buffer[2048] = {0};
    int n = read(trackerSock, buffer, sizeof(buffer));
    return (n > 0) ? string(buffer, n) : "";
}

void TrackerConnection::closeConnection() const {
    if (trackerSock >= 0) close(trackerSock);
}
