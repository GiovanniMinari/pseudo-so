#include "FileSystemManager.h"

#include <iostream>

FileSystemManager::FileSystemManager()
    : totalBlocks(0) {
}

void FileSystemManager::initializeDisk(int totalBlocks) {
    this->totalBlocks = totalBlocks;
    disk.assign(totalBlocks, "0");
    files.clear();
    operations.clear();
}

void FileSystemManager::addInitialFile(const std::string& name, int startBlock, int size) {
    DiskFile file;
    file.name = name;
    file.startBlock = startBlock;
    file.size = size;
    file.ownerPid = -1;

    files[name] = file;

    for (int i = 0; i < size; i++) {
        int block = startBlock + i;
        if (block >= 0 && block < static_cast<int>(disk.size())) {
            disk[block] = name;
        }
    }
}

void FileSystemManager::addOperation(const FileOperation& operation) {
    operations.push_back(operation);
}

int FileSystemManager::findFirstFit(int size) const {
    (void) size;
    return -1;
}

bool FileSystemManager::processExists(int pid, const std::vector<Process>& processes) const {
    for (std::size_t i = 0; i < processes.size(); i++) {
        if (processes[i].getPid() == pid) {
            return true;
        }
    }
    return false;
}

bool FileSystemManager::canDeleteFile(const Process& process, const DiskFile& file) const {
    (void) process;
    (void) file;
    return false;
}

void FileSystemManager::executeOperations(const std::vector<Process>& processes) {
    (void) processes;
}

bool FileSystemManager::createFile(
    int pid,
    const std::string& name,
    int size,
    const std::vector<Process>& processes
) {
    (void) pid;
    (void) name;
    (void) size;
    (void) processes;
    return false;
}

bool FileSystemManager::deleteFile(
    int pid,
    const std::string& name,
    const std::vector<Process>& processes
) {
    (void) pid;
    (void) name;
    (void) processes;
    return false;
}

void FileSystemManager::printDiskMap() const {
    std::cout << "Mapa de ocupacao do disco:\n";
    for (std::size_t i = 0; i < disk.size(); i++) {
        std::cout << disk[i];
        if (i + 1 < disk.size()) {
            std::cout << " ";
        }
    }
    std::cout << "\n";
}
