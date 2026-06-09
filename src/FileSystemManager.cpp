#include "FileSystemManager.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {
    const Process* findProcessByPid(int pid, const std::vector<Process>& processes) {
        for (std::size_t i = 0; i < processes.size(); i++) {
            if (processes[i].getPid() == pid) {
                return &processes[i];
            }
        }

        return 0;
    }

    std::string formatBlockInterval(int startBlock, int size) {
        std::ostringstream output;

        output << "blocos ";
        for (int i = 0; i < size; i++) {
            if (i > 0) {
                if (i == size - 1) {
                    output << " e ";
                } else {
                    output << ", ";
                }
            }

            output << (startBlock + i);
        }

        return output.str();
    }
}

FileSystemManager::FileSystemManager()
    : totalBlocks(0) {
}

void FileSystemManager::initializeDisk(int totalBlocks) {
    if (totalBlocks <= 0) {
        throw std::runtime_error("Quantidade total de blocos do disco deve ser positiva.");
    }

    this->totalBlocks = totalBlocks;
    disk.assign(totalBlocks, "0");
    files.clear();
    operations.clear();
}

void FileSystemManager::addInitialFile(const std::string& name, int startBlock, int size) {
    if (name.empty() || size <= 0 || startBlock < 0 || startBlock + size > totalBlocks) {
        throw std::runtime_error("Segmento inicial de arquivo invalido: " + name);
    }

    if (files.find(name) != files.end()) {
        throw std::runtime_error("Arquivo inicial duplicado: " + name);
    }

    for (int i = 0; i < size; i++) {
        int block = startBlock + i;
        if (disk[block] != "0") {
            throw std::runtime_error("Sobreposicao de blocos no arquivo inicial: " + name);
        }
    }

    DiskFile file;
    file.name = name;
    file.startBlock = startBlock;
    file.size = size;
    file.ownerPid = -1;

    files[name] = file;

    for (int i = 0; i < size; i++) {
        disk[startBlock + i] = name;
    }
}

void FileSystemManager::addOperation(const FileOperation& operation) {
    operations.push_back(operation);
}

int FileSystemManager::findFirstFit(int size) const {
    if (size <= 0 || size > totalBlocks) {
        return -1;
    }

    int freeSequenceStart = -1;
    int freeSequenceSize = 0;

    for (int i = 0; i < totalBlocks; i++) {
        bool blockIsFree = (disk[i] == "0");

        if (blockIsFree) {
            if (freeSequenceStart == -1) {
                freeSequenceStart = i;
            }

            freeSequenceSize++;

            if (freeSequenceSize == size) {
                return freeSequenceStart;
            }
        } else {
            freeSequenceStart = -1;
            freeSequenceSize = 0;
        }
    }

    return -1;
}

bool FileSystemManager::processExists(int pid, const std::vector<Process>& processes) const {
    return findProcessByPid(pid, processes) != 0;
}

bool FileSystemManager::canDeleteFile(const Process& process, const DiskFile& file) const {
    if (process.isRealTime()) {
        return true;
    }

    return file.ownerPid == process.getPid();
}

void FileSystemManager::executeOperations(const std::vector<Process>& processes) {
    std::cout << "Sistema de arquivos =>\n";

    for (std::size_t i = 0; i < operations.size(); i++) {
        const FileOperation& operation = operations[i];
        int operationNumber = static_cast<int>(i) + 1;

        const Process* process = findProcessByPid(operation.processPid, processes);
        if (process == 0) {
            std::cout << "Operacao " << operationNumber << " => Falha\n";
            std::cout << "O processo " << operation.processPid << " nao existe.\n";
            continue;
        }

        if (operation.type == FileOperationType::CREATE) {
            if (files.find(operation.fileName) != files.end()) {
                std::cout << "Operacao " << operationNumber << " => Falha\n";
                std::cout << "O processo " << operation.processPid
                          << " nao pode criar o arquivo " << operation.fileName
                          << " porque ele ja existe.\n";
                continue;
            }

            int firstBlock = findFirstFit(operation.size);
            if (firstBlock == -1) {
                std::cout << "Operacao " << operationNumber << " => Falha\n";
                std::cout << "O processo " << operation.processPid
                          << " nao pode criar o arquivo " << operation.fileName
                          << " (falta de espaco).\n";
                continue;
            }

            bool created = createFile(
                operation.processPid,
                operation.fileName,
                operation.size,
                processes
            );

            if (created) {
                std::cout << "Operacao " << operationNumber << " => Sucesso\n";
                std::cout << "O processo " << operation.processPid
                          << " criou o arquivo " << operation.fileName
                          << " (" << formatBlockInterval(firstBlock, operation.size) << ").\n";
            } else {
                std::cout << "Operacao " << operationNumber << " => Falha\n";
                std::cout << "O processo " << operation.processPid
                          << " nao pode criar o arquivo " << operation.fileName << ".\n";
            }
        } else {
            std::map<std::string, DiskFile>::const_iterator fileIt = files.find(operation.fileName);
            if (fileIt == files.end()) {
                std::cout << "Operacao " << operationNumber << " => Falha\n";
                std::cout << "O processo " << operation.processPid
                          << " nao pode deletar o arquivo " << operation.fileName
                          << " porque ele nao existe.\n";
                continue;
            }

            if (!canDeleteFile(*process, fileIt->second)) {
                std::cout << "Operacao " << operationNumber << " => Falha\n";
                std::cout << "O processo " << operation.processPid
                          << " nao pode deletar o arquivo " << operation.fileName
                          << " porque ele nao foi criado por esse processo.\n";
                continue;
            }

            bool deleted = deleteFile(operation.processPid, operation.fileName, processes);
            if (deleted) {
                std::cout << "Operacao " << operationNumber << " => Sucesso\n";
                std::cout << "O processo " << operation.processPid
                          << " deletou o arquivo " << operation.fileName << ".\n";
            } else {
                std::cout << "Operacao " << operationNumber << " => Falha\n";
                std::cout << "O processo " << operation.processPid
                          << " nao pode deletar o arquivo " << operation.fileName << ".\n";
            }
        }
    }

    printDiskMap();
}

bool FileSystemManager::createFile(
    int pid,
    const std::string& name,
    int size,
    const std::vector<Process>& processes
) {
    if (!processExists(pid, processes)) {
        return false;
    }

    if (size <= 0 || files.find(name) != files.end()) {
        return false;
    }

    int startBlock = findFirstFit(size);
    if (startBlock == -1) {
        return false;
    }

    DiskFile file;
    file.name = name;
    file.startBlock = startBlock;
    file.size = size;
    file.ownerPid = pid;

    files[name] = file;

    for (int i = 0; i < size; i++) {
        disk[startBlock + i] = name;
    }

    return true;
}

bool FileSystemManager::deleteFile(
    int pid,
    const std::string& name,
    const std::vector<Process>& processes
) {
    const Process* process = findProcessByPid(pid, processes);
    if (process == 0) {
        return false;
    }

    std::map<std::string, DiskFile>::iterator fileIt = files.find(name);
    if (fileIt == files.end()) {
        return false;
    }

    DiskFile file = fileIt->second;
    if (!canDeleteFile(*process, file)) {
        return false;
    }

    for (int i = 0; i < file.size; i++) {
        int block = file.startBlock + i;
        if (block >= 0 && block < static_cast<int>(disk.size())) {
            disk[block] = "0";
        }
    }

    files.erase(fileIt);
    return true;
}

void FileSystemManager::printDiskMap() const {
    std::cout << "Mapa de ocupacao do disco:\n";
    for (std::size_t i = 0; i < disk.size(); i++) {
        std::cout << disk[i];
        if (i + 1 < disk.size()) {
            std::cout << " ";
        }
    }
    std::cout << "\n";
}
