CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread


SRC = client.cpp \
tracker_connection.cpp \
peer_server.cpp \
file_utils.cpp \
client_request.cpp


OBJ = $(SRC:.cpp=.o)
TARGET = client


all: $(TARGET)


$(TARGET): $(OBJ)
$(CXX) $(CXXFLAGS) -o $@ $(OBJ)


%.o: %.cpp client_handler.h
$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
rm -f $(OBJ) $(TARGET)


.PHONY: all clean