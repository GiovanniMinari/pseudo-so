#include "Dispatcher.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>
#include "InputParser.h"

Dispatcher::Dispatcher()
    : currentTime(0) {
    memoryManager.initialize();
    resourceManager.initialize();
}

void Dispatcher::load(
    const std::string& processesFile,
    const std::string& filesFile,
    const std::string& stringsFile
) {
    InputParser parser;

    processes = parser.readProcesses(processesFile);
    parser.readFileSystemInput(filesFile, fileSystemManager);

    std::vector<std::vector<int> > references = parser.readPageReferences(stringsFile);

    int amountToAttach = static_cast<int>(std::min(processes.size(), references.size()));
    for (int i = 0; i < amountToAttach; i++) {
        processes[i].setPageReferences(references[i]);
    }

    if (references.size() != processes.size()) {
        std::cerr
            << "Aviso: quantidade de linhas em string.txt (" << references.size()
            << ") diferente da quantidade de processos (" << processes.size() << ").\n";
    }
}

void Dispatcher::run() {
    currentTime = 0;

    while (!allProcessesFinished()) {
        createArrivedProcesses();

        if (!scheduler.hasReadyProcess()) {
            int nextArrivalTime = std::numeric_limits<int>::max();

            for (std::size_t i = 0; i < processes.size(); i++) {
                if (processes[i].getState() == ProcessState::NEW
                    && processes[i].getArrivalTime() < nextArrivalTime) {
                    nextArrivalTime = processes[i].getArrivalTime();
                }
            }

            if (nextArrivalTime == std::numeric_limits<int>::max()) {
                std::cout
                    << "dispatcher => nenhum processo pronto. "
                    << "Se ainda houver processo bloqueado, ele esta aguardando recurso de E/S.\n";
                break;
            }

            if (nextArrivalTime > currentTime) {
                currentTime = nextArrivalTime;
            } else {
                currentTime++;
            }

            continue;
        }

        int pid = scheduler.getNextProcessPid();
        if (pid < 0 || pid >= static_cast<int>(processes.size())) {
            continue;
        }

        Process& process = processes[pid];
        if (process.isFinished()) {
            finishProcess(process);
            continue;
        }

        if (process.isRealTime()) {
            executeRealTimeProcess(process);
        } else {
            executeUserProcess(process);
        }
    }

    std::cout << "dispatcher => todos os processos terminaram.\n";
}

void Dispatcher::printFinalReport() const {
    memoryManager.printPageFaults(processes);
}

void Dispatcher::createArrivedProcesses() {
    for (std::size_t i = 0; i < processes.size(); i++) {
        Process& process = processes[i];

        bool canTryToEnterReadyQueue =
            (process.getState() == ProcessState::NEW
             || process.getState() == ProcessState::BLOCKED)
            && process.getArrivalTime() <= currentTime;

        if (!canTryToEnterReadyQueue) {
            continue;
        }

        if (resourceManager.allocate(process)) {
            memoryManager.preloadProcess(process);
            process.setState(ProcessState::READY);
            scheduler.addProcess(process);
        } else {
            if (process.getState() == ProcessState::NEW) {
                std::cout
                    << "dispatcher => P" << process.getPid()
                    << " BLOCKED: aguardando recurso de E/S.\n";
            }

            process.setState(ProcessState::BLOCKED);
        }
    }
}

bool Dispatcher::allProcessesFinished() const {
    for (std::size_t i = 0; i < processes.size(); i++) {
        if (!processes[i].isFinished()) {
            return false;
        }
    }
    return true;
}

void Dispatcher::executeRealTimeProcess(Process& process) {
    process.setState(ProcessState::RUNNING);

    printDispatcherInfo(process);
    std::cout << "process " << process.getPid() << " => \n";

    if (process.getExecutedInstructions() == 0) {
        std::cout << "P" << process.getPid() << " STARTED\n";
    }

    while (!process.isFinished()) {
        int pageNumber = process.getNextPageReference();
        int faultsBefore = process.getPageFaults();

        memoryManager.accessPage(process, pageNumber);
        process.executeOneInstruction();
        currentTime++;

        std::cout << "P" << process.getPid()
                  << " instruction " << process.getExecutedInstructions();

        if (pageNumber >= 0) {
            std::cout << " (page " << pageNumber << ")";
        }

        if (process.getPageFaults() > faultsBefore) {
            std::cout << " PAGE FAULT";
        }

        std::cout << "\n";
    }

    finishProcess(process);
    std::cout << "P" << process.getPid() << " return SIGINT\n";
}

void Dispatcher::executeUserProcess(Process& process) {
    process.setState(ProcessState::RUNNING);

    printDispatcherInfo(process);
    std::cout << "process " << process.getPid() << " => \n";

    if (process.getExecutedInstructions() == 0) {
        std::cout << "P" << process.getPid() << " STARTED\n";
    }

    int executedInThisQuantum = 0;
    while (!process.isFinished()
           && executedInThisQuantum < scheduler.getQuantum()) {
        int pageNumber = process.getNextPageReference();
        int faultsBefore = process.getPageFaults();

        memoryManager.accessPage(process, pageNumber);
        process.executeOneInstruction();
        executedInThisQuantum++;
        currentTime++;

        std::cout << "P" << process.getPid()
                  << " instruction " << process.getExecutedInstructions();

        if (pageNumber >= 0) {
            std::cout << " (page " << pageNumber << ")";
        }

        if (process.getPageFaults() > faultsBefore) {
            std::cout << " PAGE FAULT";
        }

        std::cout << "\n";
    }

    if (process.isFinished()) {
        finishProcess(process);
        std::cout << "P" << process.getPid() << " return SIGINT\n";
        return;
    }

    process.setState(ProcessState::READY);

    // Processos que chegaram durante o quantum entram na fila antes do processo atual voltar.
    createArrivedProcesses();
    scheduler.requeueUserProcess(process);
}

void Dispatcher::finishProcess(Process& process) {
    resourceManager.release(process);
    memoryManager.releaseProcess(process);
    process.setState(ProcessState::FINISHED);
}

void Dispatcher::printDispatcherInfo(const Process& process) const {
    ResourceRequest resources = process.getResources();

    std::cout << "dispatcher => \n";
    std::cout << " PID: " << process.getPid() << "\n";
    std::cout << " arrival: " << process.getArrivalTime() << "\n";
    std::cout << " frames: " << process.getWorkingSetSize() << "\n";
    std::cout << " priority: " << process.getPriority() << "\n";
    std::cout << " time: " << process.getRemainingTime() << "\n";
    std::cout << " printers: " << resources.printers << "\n";
    std::cout << " scanners: " << resources.scanners << "\n";
    std::cout << " modems: " << resources.modems << "\n";
    std::cout << " drives: " << resources.sataDrives << "\n";
}
