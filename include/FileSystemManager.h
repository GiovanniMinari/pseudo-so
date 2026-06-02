#ifndef FILE_SYSTEM_MANAGER_H
#define FILE_SYSTEM_MANAGER_H

#include <map>
#include <string>
#include <vector>
#include "Process.h"

enum class FileOperationType {
    CREATE,
    DELETE
};

struct DiskFile {
    std::string name;
    int startBlock;
    int size;
    int ownerPid;
};

struct FileOperation {
    int processPid;
    FileOperationType type;
    std::string fileName;
    int size;
};

class FileSystemManager {
private:
    int totalBlocks;
    std::vector<std::string> disk;
    std::map<std::string, DiskFile> files;
    std::vector<FileOperation> operations;

    int findFirstFit(int size) const;

    bool processExists(int pid, const std::vector<Process>& processes) const;

    bool canDeleteFile(const Process& process, const DiskFile& file) const;

public:
    FileSystemManager();

    void initializeDisk(int totalBlocks);

    void addInitialFile(const std::string& name, int startBlock, int size);

    void addOperation(const FileOperation& operation);

    void executeOperations(const std::vector<Process>& processes);

    bool createFile(
        int pid,
        const std::string& name,
        int size,
        const std::vector<Process>& processes
    );

    bool deleteFile(
        int pid,
        const std::string& name,
        const std::vector<Process>& processes
    );

    void printDiskMap() const;
};

#endif