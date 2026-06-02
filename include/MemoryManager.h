#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <vector>
#include "Process.h"

struct Frame {
    bool occupied;
    int pid;
    int pageNumber;
    int lastUsedTime;
    bool realTimeArea;
};

class MemoryManager {
private:
    std::vector<Frame> frames;
    int globalTimeCounter;

    int findFreeFrameForProcess(const Process& process) const;
    int findLRUFrameLocal(const Process& process) const;
    bool isPageLoaded(int pid, int pageNumber) const;
    int countFramesUsedByProcess(int pid) const;

public:
    MemoryManager();

    void initialize();

    void preloadProcess(Process& process);

    void accessPage(Process& process, int pageNumber);

    void printPageFaults(const std::vector<Process>& processes) const;
};

#endif