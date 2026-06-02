#include "Process.h"

Process::Process()
    : pid(-1),
      arrivalTime(0),
      priority(0),
      originalPriority(0),
      processorTime(0),
      remainingTime(0),
      workingSetSize(0),
      resources{0, 0, 0, 0},
      currentPageIndex(0),
      state(ProcessState::NEW),
      pageFaults(0),
      executedInstructions(0),
      waitingTime(0) {
}

Process::Process(
    int pid,
    int arrivalTime,
    int priority,
    int processorTime,
    int workingSetSize,
    ResourceRequest resources
)
    : pid(pid),
      arrivalTime(arrivalTime),
      priority(priority),
      originalPriority(priority),
      processorTime(processorTime),
      remainingTime(processorTime),
      workingSetSize(workingSetSize),
      resources(resources),
      currentPageIndex(0),
      state(ProcessState::NEW),
      pageFaults(0),
      executedInstructions(0),
      waitingTime(0) {
}

int Process::getPid() const {
    return pid;
}

int Process::getArrivalTime() const {
    return arrivalTime;
}

int Process::getPriority() const {
    return priority;
}

int Process::getOriginalPriority() const {
    return originalPriority;
}

int Process::getRemainingTime() const {
    return remainingTime;
}

int Process::getWorkingSetSize() const {
    return workingSetSize;
}

int Process::getPageFaults() const {
    return pageFaults;
}

int Process::getExecutedInstructions() const {
    return executedInstructions;
}

int Process::getWaitingTime() const {
    return waitingTime;
}

ResourceRequest Process::getResources() const {
    return resources;
}

ProcessState Process::getState() const {
    return state;
}

void Process::setPriority(int newPriority) {
    priority = newPriority;
}

void Process::setPageReferences(const std::vector<int>& references) {
    pageReferences = references;
    currentPageIndex = 0;
}

void Process::setState(ProcessState newState) {
    state = newState;
}

bool Process::isRealTime() const {
    return priority == 0;
}

bool Process::isFinished() const {
    return state == ProcessState::FINISHED || remainingTime <= 0;
}

bool Process::hasNextPageReference() const {
    return currentPageIndex < static_cast<int>(pageReferences.size());
}

int Process::getNextPageReference() {
    if (!hasNextPageReference()) {
        return -1;
    }

    int page = pageReferences[currentPageIndex];
    currentPageIndex++;
    return page;
}

void Process::executeOneInstruction() {
    if (remainingTime > 0) {
        remainingTime--;
        executedInstructions++;
    }

    if (remainingTime == 0) {
        state = ProcessState::FINISHED;
    }
}

void Process::incrementPageFaults() {
    pageFaults++;
}

void Process::increaseWaitingTime() {
    waitingTime++;
}

void Process::resetWaitingTime() {
    waitingTime = 0;
}
