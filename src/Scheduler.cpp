#include "Scheduler.h"

namespace {
    void clearQueue(std::queue<int>& queue) {
        std::queue<int> empty;
        queue.swap(empty);
    }
}

Scheduler::Scheduler()
    : quantum(1), agingLimit(5) {
}

void Scheduler::addProcess(const Process& process) {
    int pid = process.getPid();

    if (process.isRealTime()) {
        realTimeQueue.push(pid);
    } else if (process.getPriority() <= 1) {
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
    if (process.getPriority() == 1) {
        process.setPriority(2);
    } else if (process.getPriority() == 2) {
        process.setPriority(3);
    }

    process.resetWaitingTime();
    addProcess(process);
}

void Scheduler::applyAging(std::vector<Process>& processes) {
    // Aging apenas para processos de usuario prontos.
    // Processos de tempo real permanecem na fila FIFO de prioridade 0.
    for (std::size_t i = 0; i < processes.size(); i++) {
        Process& process = processes[i];

        if (process.isRealTime() || process.getState() != ProcessState::READY) {
            continue;
        }

        process.increaseWaitingTime();

        if (process.getWaitingTime() >= agingLimit && process.getPriority() > 1) {
            process.setPriority(process.getPriority() - 1);
            process.resetWaitingTime();
        }
    }

    // Como uma promocao muda a fila correta do processo, reconstruimos as filas
    // de usuario a partir dos processos READY. Isso evita PIDs duplicados.
    clearQueue(userQueue1);
    clearQueue(userQueue2);
    clearQueue(userQueue3);

    for (std::size_t i = 0; i < processes.size(); i++) {
        const Process& process = processes[i];

        if (!process.isRealTime() && process.getState() == ProcessState::READY) {
            addProcess(process);
        }
    }
}

int Scheduler::getQuantum() const {
    return quantum;
}
