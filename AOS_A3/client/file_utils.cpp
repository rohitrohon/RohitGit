#include "client_handler.h"


string computeSHA1(const string &filePath) {
FILE *file = fopen(filePath.c_str(), "rb");
if (!file) return "";


SHA_CTX ctx;
SHA1_Init(&ctx);
unsigned char buffer[1024];
size_t bytesRead;


while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
SHA1_Update(&ctx, buffer, bytesRead);


fclose(file);


unsigned char hash[SHA_DIGEST_LENGTH];
SHA1_Final(hash, &ctx);


stringstream ss;
for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
ss << hex << setw(2) << setfill('0') << (int)hash[i];


return ss.str();
}


void sendRequestedChunk(int sock, const string &req) {
// Parse req: file_path chunk_no
string filePath;
int chunkNo;
stringstream ss(req);
ss >> filePath >> chunkNo;


ifstream fin(filePath, ios::binary);
if (!fin) return;


fin.seekg(chunkNo * CHUNK_SIZE, ios::beg);
vector<char> buffer(CHUNK_SIZE);
fin.read(buffer.data(), CHUNK_SIZE);
send(sock, buffer.data(), fin.gcount(), 0);
}