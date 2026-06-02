#include "MemoryManager.h"

#include <iostream>

MemoryManager::MemoryManager()
    : globalTimeCounter(0) {
}

void MemoryManager::initialize() {
    frames.clear();

    for (int i = 0; i < 20; i++) {
        Frame frame;
        frame.occupied = false;
        frame.pid = -1;
        frame.pageNumber = -1;
        frame.lastUsedTime = 0;
        frame.realTimeArea = (i < 8);
        frames.push_back(frame);
    }

    globalTimeCounter = 0;
}

int MemoryManager::findFreeFrameForProcess(const Process& process) const {
    (void) process;
    return -1;
}

int MemoryManager::findLRUFrameLocal(const Process& process) const {
    (void) process;
    return -1;
}

bool MemoryManager::isPageLoaded(int pid, int pageNumber) const {
    (void) pid;
    (void) pageNumber;
    return false;
}

int MemoryManager::countFramesUsedByProcess(int pid) const {
    (void) pid;
    return 0;
}

void MemoryManager::preloadProcess(Process& process) {
    (void) process;
}

void MemoryManager::accessPage(Process& process, int pageNumber) {
    (void) process;
    (void) pageNumber;
}

void MemoryManager::printPageFaults(const std::vector<Process>& processes) const {
    std::cout << "Numero de Faltas de Paginas por processo:\n";
    for (std::size_t i = 0; i < processes.size(); i++) {
        std::cout << "P" << processes[i].getPid()
                  << " = " << processes[i].getPageFaults()
                  << " faltas de paginas\n";
    }
}
