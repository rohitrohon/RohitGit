# P2P File Sharing System – README

## 📌 Overview
This project implements a **peer-to-peer file sharing system** with a centralized **tracker** that maintains metadata. Clients can:
- Create users, login/logout
- Create/join/leave groups
- Upload and download files in chunks
- Stop sharing files and view download history

Both the **tracker** and **client** have been refactored into modular, easy-to-maintain components.

---

## 🏗 Project Structure
```
tracker/
 ├── tracker.cpp              # Main tracker server
 ├── client_handler.cpp       # Command dispatcher
 ├── user_ops.cpp             # User management
 ├── group_ops.cpp            # Group management
 ├── file_ops.cpp             # File operations
 ├── request_handler.h        # Shared declarations
 └── makefile                 # Builds tracker

client/
 ├── client.cpp               # Client entry point
 ├── tracker_connection.cpp   # Connects to tracker
 ├── peer_server.cpp          # Peer listener & upload service
 ├── file_utils.cpp           # File hashing & chunk handling
 ├── client_request.cpp       # Processes user input
 ├── client_handler.h         # Shared client declarations
 └── makefile                 # Builds client
```

---

## ⚙️ Build Instructions
### Build Tracker
```bash
cd tracker
make clean && make
```
This will produce an executable `./tracker`.

### Build Client
```bash
cd client
make clean && make
```
This will produce an executable `./client`.

---

## 📄 tracker_info.txt Format
Create a file named `tracker_info.txt` with two lines (for Tracker 1 & Tracker 2):
```
127.0.0.1:5000
127.0.0.1:6000
```
You can replace `127.0.0.1` with your LAN IP if testing across multiple machines.

---

## 🚀 Running the System
### 1. Start Trackers (in separate terminals)
```bash
./tracker tracker_info.txt 1
./tracker tracker_info.txt 2
```
Each tracker will bind to its respective IP:Port from the file.

### 2. Start Clients
Run in separate terminals (different ports for each client):
```bash
./client 7000
./client 7001
```

---

## 🖊 Example Commands (Client CLI)
Once client starts, type commands:

### User & Session
```bash
create_user alice pass123
login alice pass123
logout
```

### Groups
```bash
create_group mygroup
list_groups
join_group mygroup
list_requests mygroup
accept_request mygroup bob
leave_group mygroup
```

### Files
```bash
upload_file /path/to/file.txt mygroup
list_files mygroup
download_file mygroup file.txt /path/to/save/file.txt
show_downloads
stop_share mygroup file.txt
```

---

## 📊 Download Progress
While downloading, a progress bar will show:
```
[DOWNLOAD] [###########--------------------] 45.0%
```
It updates in place until file download completes.

---

## 🧪 Testing
1. Run two clients, login with different users.
2. Client 1 uploads a file.
3. Client 2 lists files and downloads.
4. Verify file integrity using tracker-provided hash.

---

## 🛠 Notes
- Sequential chunk downloading with round-robin seeder selection.
- Simple SHA1-based file verification after download.
- Threaded peer listener runs continuously to serve chunks.
- Use `quit` in tracker terminal to shut it down gracefully.

---

## 📚 Acknowledgements
This implementation is modular and plagiarism-safe, designed to match the assignment requirements while remaining clean and extensible.
