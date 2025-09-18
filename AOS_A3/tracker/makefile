CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -D_GLIBCXX_USE_CXX11_ABI=1

SRC = tracker.cpp \
      client_handler.cpp \
      user_ops.cpp \
      group_ops.cpp \
      file_ops.cpp

OBJ = $(SRC:.cpp=.o)
TARGET = tracker

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

%.o: %.cpp request_handler.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
