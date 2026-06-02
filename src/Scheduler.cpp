#include "Scheduler.h"

Scheduler::Scheduler()
    : quantum(1), agingLimit(5) {
}

void Scheduler::addProcess(const Process& process) {
    int pid = process.getPid();

    if (process.isRealTime()) {
        realTimeQueue.push(pid);
    } else if (process.getPriority() == 1) {
        userQueue1.push(pid);
    } else if (process.getPriority() == 2) {
        userQueue2.push(pid);
    } else {
        userQueue3.push(pid);
    }
}

bool Scheduler::hasReadyProcess() const {
    return !realTimeQueue.empty()
        || !userQueue1.empty()
        || !userQueue2.empty()
        || !userQueue3.empty();
}

int Scheduler::getNextProcessPid() {
    int pid = -1;

    if (!realTimeQueue.empty()) {
        pid = realTimeQueue.front();
        realTimeQueue.pop();
    } else if (!userQueue1.empty()) {
        pid = userQueue1.front();
        userQueue1.pop();
    } else if (!userQueue2.empty()) {
        pid = userQueue2.front();
        userQueue2.pop();
    } else if (!userQueue3.empty()) {
        pid = userQueue3.front();
        userQueue3.pop();
    }

    return pid;
}

void Scheduler::requeueUserProcess(Process& process) {
    addProcess(process);
}

void Scheduler::applyAging(std::vector<Process>& processes) {
    (void) processes;
}

int Scheduler::getQuantum() const {
    return quantum;
}
