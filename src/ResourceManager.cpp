#include "ResourceManager.h"

ResourceManager::ResourceManager() {
    initialize();
}

void ResourceManager::initialize() {
    printerOwner.assign(2, -1);
    scannerOwner.assign(1, -1);
    modemOwner.assign(1, -1);
    sataOwner.assign(2, -1);
}

bool ResourceManager::allocateFromPool(std::vector<int>& pool, int amount, int pid) {
    int allocated = 0;

    for (std::size_t i = 0; i < pool.size() && allocated < amount; i++) {
        if (pool[i] == -1) {
            pool[i] = pid;
            allocated++;
        }
    }

    return allocated == amount;
}

void ResourceManager::releaseFromPool(std::vector<int>& pool, int pid) {
    for (std::size_t i = 0; i < pool.size(); i++) {
        if (pool[i] == pid) {
            pool[i] = -1;
        }
    }
}

int ResourceManager::countFree(const std::vector<int>& pool) const {
    int freeAmount = 0;

    for (std::size_t i = 0; i < pool.size(); i++) {
        if (pool[i] == -1) {
            freeAmount++;
        }
    }

    return freeAmount;
}

bool ResourceManager::canAllocate(const Process& process) const {
    // A especificacao diz que processos de tempo real nao precisam de I/O.
    // Portanto, nesta simulacao eles nao disputam scanner, impressora, modem ou SATA.
    if (process.isRealTime()) {
        return true;
    }

    ResourceRequest request = process.getResources();

    return request.printers <= countFree(printerOwner)
        && request.scanners <= countFree(scannerOwner)
        && request.modems <= countFree(modemOwner)
        && request.sataDrives <= countFree(sataOwner);
}

bool ResourceManager::allocate(const Process& process) {
    if (!canAllocate(process)) {
        return false;
    }

    // Processos de tempo real ignoram E/S.
    if (process.isRealTime()) {
        return true;
    }

    ResourceRequest request = process.getResources();
    int pid = process.getPid();

    allocateFromPool(printerOwner, request.printers, pid);
    allocateFromPool(scannerOwner, request.scanners, pid);
    allocateFromPool(modemOwner, request.modems, pid);
    allocateFromPool(sataOwner, request.sataDrives, pid);

    return true;
}

void ResourceManager::release(const Process& process) {
    int pid = process.getPid();

    releaseFromPool(printerOwner, pid);
    releaseFromPool(scannerOwner, pid);
    releaseFromPool(modemOwner, pid);
    releaseFromPool(sataOwner, pid);
}
