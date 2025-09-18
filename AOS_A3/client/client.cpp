#include "client_handler.h"
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: ./client <client_port>" << endl;
        return EXIT_FAILURE;
    }

    int clientPort = stoi(argv[1]);
    string clientIP = getLocalIPAddress();

    cout << "[INFO] Client starting on IP: " << clientIP << " Port: " << clientPort << endl;

    // Start peer listener in separate thread
    thread peerThread(startPeerListener, clientPort);

    // Connect to tracker with failover support
    TrackerConnection tracker("../client/tracker_info.txt", clientIP, clientPort);
    if (!tracker.connectToTracker()) {
        cerr << "[ERROR] Could not connect to any tracker." << endl;
        return EXIT_FAILURE;
    }

    // Process user commands in loop
    string input;
    while (true) {
        cout << "client> ";
        if (!getline(cin, input)) break;
        if (input == "quit") break;

        processUserCommand(input, tracker, clientPort);
    }

    cout << "[INFO] Shutting down client..." << endl;
    tracker.closeConnection();
    peerThread.detach(); // allow listener thread to close naturally
    return EXIT_SUCCESS;
}