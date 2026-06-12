#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <vector>
#include "Process.h"

class ResourceManager {
private:
    std::vector<int> printerOwner;
    std::vector<int> scannerOwner;
    std::vector<int> modemOwner;
    std::vector<int> sataOwner;

    bool allocateFromPool(std::vector<int>& pool, int amount, int pid);
    void releaseFromPool(std::vector<int>& pool, int pid);
    int countFree(const std::vector<int>& pool) const;

public:
    ResourceManager();

    void initialize();

    bool isRequestPossible(const Process& process) const;

    bool canAllocate(const Process& process) const;

    bool allocate(const Process& process);

    void release(const Process& process);
};

#endif