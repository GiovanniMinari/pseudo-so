#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include <vector>
#include "Process.h"

class Scheduler {
private:
    std::queue<int> realTimeQueue;
    std::queue<int> userQueue1;
    std::queue<int> userQueue2;
    std::queue<int> userQueue3;

    int quantum;
    int agingLimit;

public:
    Scheduler();

    void addProcess(const Process& process);

    bool hasReadyProcess() const;

    int getNextProcessPid();

    void requeueUserProcess(Process& process);

    void applyAging(std::vector<Process>& processes);

    int getQuantum() const;
};

#endif