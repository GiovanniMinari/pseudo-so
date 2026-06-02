#include "Dispatcher.h"

#include <algorithm>
#include <iostream>
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

    std::vector<std::vector<int>> references = parser.readPageReferences(stringsFile);

    int amountToAttach = std::min(processes.size(), references.size());
    for (int i = 0; i < amountToAttach; i++) {
        processes[i].setPageReferences(references[i]);
    }

    if (references.size() != processes.size()) {
        std::cerr
            << "Aviso: quantidade de linhas em string.txt (" << references.size()
            << ") diferente da quantidade de processos (" << processes.size() << ").\n";
    }

    std::cout << "dispatcher => processos carregados\n";
    for (std::size_t i = 0; i < processes.size(); i++) {
        printDispatcherInfo(processes[i]);
    }
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
    std::cout << " page references loaded: "
              << (process.hasNextPageReference() ? "yes" : "no") << "\n";
}

void Dispatcher::run() {
    std::cout << "dispatcher => fase 1: execucao ainda nao implementada.\n";
}

void Dispatcher::printFinalReport() const {
    std::cout << "dispatcher => fase 1: relatorio final ainda nao implementado.\n";
}

void Dispatcher::createArrivedProcesses() {
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
    (void) process;
}

void Dispatcher::executeUserProcess(Process& process) {
    (void) process;
}

void Dispatcher::finishProcess(Process& process) {
    process.setState(ProcessState::FINISHED);
}
