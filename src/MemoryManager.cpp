#include "MemoryManager.h"

#include <iostream>

namespace {
    bool frameBelongsToProcessArea(const Frame& frame, const Process& process) {
        if (process.isRealTime()) {
            return frame.realTimeArea;
        }

        return !frame.realTimeArea;
    }

    int minimumWorkingSetSize(const Process& process) {
        if (process.getWorkingSetSize() <= 0) {
            return 1;
        }

        return process.getWorkingSetSize();
    }
}

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
    int usedByProcess = countFramesUsedByProcess(process.getPid());
    int maxFramesForProcess = minimumWorkingSetSize(process);

    if (usedByProcess >= maxFramesForProcess) {
        return -1;
    }

    for (std::size_t i = 0; i < frames.size(); i++) {
        if (!frames[i].occupied && frameBelongsToProcessArea(frames[i], process)) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

int MemoryManager::findLRUFrameLocal(const Process& process) const {
    int lruIndex = -1;
    int oldestUsage = 0;

    for (std::size_t i = 0; i < frames.size(); i++) {
        if (!frames[i].occupied || frames[i].pid != process.getPid()) {
            continue;
        }

        if (lruIndex == -1 || frames[i].lastUsedTime < oldestUsage) {
            lruIndex = static_cast<int>(i);
            oldestUsage = frames[i].lastUsedTime;
        }
    }

    return lruIndex;
}

bool MemoryManager::isPageLoaded(int pid, int pageNumber) const {
    for (std::size_t i = 0; i < frames.size(); i++) {
        if (frames[i].occupied
            && frames[i].pid == pid
            && frames[i].pageNumber == pageNumber) {
            return true;
        }
    }

    return false;
}

int MemoryManager::countFramesUsedByProcess(int pid) const {
    int amount = 0;

    for (std::size_t i = 0; i < frames.size(); i++) {
        if (frames[i].occupied && frames[i].pid == pid) {
            amount++;
        }
    }

    return amount;
}

void MemoryManager::preloadProcess(Process& process) {
    if (!process.hasNextPageReference()) {
        return;
    }

    int pageNumber = process.peekNextPageReference();
    if (pageNumber < 0 || isPageLoaded(process.getPid(), pageNumber)) {
        return;
    }

    int frameIndex = findFreeFrameForProcess(process);
    if (frameIndex == -1) {
        frameIndex = findLRUFrameLocal(process);
    }

    if (frameIndex == -1) {
        std::cout << "dispatcher => P" << process.getPid()
                  << " sem frame livre para pre-carga de memoria.\n";
        return;
    }

    globalTimeCounter++;
    frames[frameIndex].occupied = true;
    frames[frameIndex].pid = process.getPid();
    frames[frameIndex].pageNumber = pageNumber;
    frames[frameIndex].lastUsedTime = globalTimeCounter;
}

void MemoryManager::accessPage(Process& process, int pageNumber) {
    if (pageNumber < 0) {
        return;
    }

    globalTimeCounter++;

    for (std::size_t i = 0; i < frames.size(); i++) {
        if (frames[i].occupied
            && frames[i].pid == process.getPid()
            && frames[i].pageNumber == pageNumber) {
            frames[i].lastUsedTime = globalTimeCounter;
            return;
        }
    }

    process.incrementPageFaults();

    int frameIndex = findFreeFrameForProcess(process);
    if (frameIndex == -1) {
        frameIndex = findLRUFrameLocal(process);
    }

    if (frameIndex == -1) {
        std::cout << "dispatcher => P" << process.getPid()
                  << " gerou page fault, mas nao ha frame local disponivel.\n";
        return;
    }

    frames[frameIndex].occupied = true;
    frames[frameIndex].pid = process.getPid();
    frames[frameIndex].pageNumber = pageNumber;
    frames[frameIndex].lastUsedTime = globalTimeCounter;
}

void MemoryManager::releaseProcess(const Process& process) {
    for (std::size_t i = 0; i < frames.size(); i++) {
        if (frames[i].occupied && frames[i].pid == process.getPid()) {
            frames[i].occupied = false;
            frames[i].pid = -1;
            frames[i].pageNumber = -1;
            frames[i].lastUsedTime = 0;
        }
    }
}

void MemoryManager::printPageFaults(const std::vector<Process>& processes) const {
    std::cout << "Numero de Faltas de Paginas por processo:\n";
    for (std::size_t i = 0; i < processes.size(); i++) {
        std::cout << "P" << processes[i].getPid()
                  << " = " << processes[i].getPageFaults()
                  << " faltas de paginas\n";
    }
}
