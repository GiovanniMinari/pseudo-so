CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Iinclude

SRC = src/main.cpp \
      src/Process.cpp \
      src/Scheduler.cpp \
      src/MemoryManager.cpp \
      src/ResourceManager.cpp \
      src/FileSystemManager.cpp \
      src/InputParser.cpp \
      src/Dispatcher.cpp \
      src/Utils.cpp

OUT = dispatcher

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)