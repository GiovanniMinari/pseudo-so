#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string>
#include <vector>

#include "Process.h"
#include "Scheduler.h"
#include "MemoryManager.h"
#include "ResourceManager.h"
#include "FileSystemManager.h"

class Dispatcher {
private:
    std::vector<Process> processes;

    Scheduler scheduler;
    MemoryManager memoryManager;
    ResourceManager resourceManager;
    FileSystemManager fileSystemManager;

    int currentTime;

    void createArrivedProcesses();

    bool allProcessesFinished() const;

    void executeRealTimeProcess(Process& process);

    void executeUserProcess(Process& process);

    void finishProcess(Process& process);

    void printDispatcherInfo(const Process& process) const;

public:
    Dispatcher();

    void load(
        const std::string& processesFile,
        const std::string& filesFile,
        const std::string& stringsFile
    );

    void run();

    void printFinalReport();
};

#endif