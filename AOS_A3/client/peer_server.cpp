#include "client_handler.h"


void startPeerListener(int clientPort) {
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
sockaddr_in addr{};
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = INADDR_ANY;
addr.sin_port = htons(clientPort);


bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
listen(sockfd, 5);
cout << "[INFO] Peer listener running on port " << clientPort << endl;


while (true) {
sockaddr_in peerAddr{};
socklen_t len = sizeof(peerAddr);
int peerSock = accept(sockfd, (struct sockaddr *)&peerAddr, &len);
thread t(servePeerRequest, peerSock);
t.detach();
}
}


void servePeerRequest(int peerSock) {
char buffer[1024] = {0};
int n = read(peerSock, buffer, sizeof(buffer));
if (n > 0) {
string fileChunkReq(buffer);
sendRequestedChunk(peerSock, fileChunkReq);
}
close(peerSock);
}