#ifndef PROCESS_H
#define PROCESS_H

#include <vector>

enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    FINISHED
};

struct ResourceRequest {
    int printers;
    int scanners;
    int modems;
    int sataDrives;
};

class Process {
private:
    int pid;
    int arrivalTime;
    int priority;
    int originalPriority;
    int processorTime;
    int remainingTime;
    int workingSetSize;
    ResourceRequest resources;
    std::vector<int> pageReferences;
    int currentPageIndex;
    ProcessState state;
    int pageFaults;
    int executedInstructions;
    int waitingTime;

public:
    Process();

    Process(
        int pid,
        int arrivalTime,
        int priority,
        int processorTime,
        int workingSetSize,
        ResourceRequest resources
    );

    int getPid() const;
    int getArrivalTime() const;
    int getPriority() const;
    int getOriginalPriority() const;
    int getRemainingTime() const;
    int getWorkingSetSize() const;
    int getPageFaults() const;
    int getExecutedInstructions() const;
    int getWaitingTime() const;
    const std::vector<int>& getPageReferences() const;

    ResourceRequest getResources() const;
    ProcessState getState() const;

    void setPriority(int newPriority);
    void setPageReferences(const std::vector<int>& references);
    void setState(ProcessState newState);

    bool isRealTime() const;
    bool isFinished() const;
    bool hasNextPageReference() const;

    int peekNextPageReference() const;
    int getNextPageReference();

    void executeOneInstruction();

    void incrementPageFaults();
    void resetPageFaults();
    void increaseWaitingTime();
    void resetWaitingTime();
};

#endif