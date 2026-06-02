#include "InputParser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
    std::string trim(const std::string& text) {
        const std::string spaces = " \t\r\n";
        std::size_t start = text.find_first_not_of(spaces);

        if (start == std::string::npos) {
            return "";
        }

        std::size_t end = text.find_last_not_of(spaces);
        return text.substr(start, end - start + 1);
    }

    std::vector<std::string> splitByComma(const std::string& line) {
        std::vector<std::string> parts;
        std::stringstream stream(line);
        std::string item;

        while (std::getline(stream, item, ',')) {
            parts.push_back(trim(item));
        }

        return parts;
    }

    bool isBlankLine(const std::string& line) {
        return trim(line).empty();
    }

    int toInt(const std::string& value, const std::string& context) {
        try {
            return std::stoi(trim(value));
        } catch (const std::exception&) {
            throw std::runtime_error("Valor inteiro invalido em " + context + ": '" + value + "'");
        }
    }
}

std::vector<Process> InputParser::readProcesses(const std::string& path) {
    std::ifstream file(path.c_str());

    if (!file.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir o arquivo de processos: " + path);
    }

    std::vector<Process> processes;
    std::string line;
    int pid = 0;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;

        if (isBlankLine(line)) {
            continue;
        }

        std::vector<std::string> fields = splitByComma(line);

        if (fields.size() != 8) {
            throw std::runtime_error(
                "Linha " + std::to_string(lineNumber) +
                " do arquivo de processos deveria ter 8 campos."
            );
        }

        int arrivalTime = toInt(fields[0], "processes.txt linha " + std::to_string(lineNumber));
        int priority = toInt(fields[1], "processes.txt linha " + std::to_string(lineNumber));
        int processorTime = toInt(fields[2], "processes.txt linha " + std::to_string(lineNumber));
        int workingSetSize = toInt(fields[3], "processes.txt linha " + std::to_string(lineNumber));

        ResourceRequest resources;
        resources.printers = toInt(fields[4], "processes.txt linha " + std::to_string(lineNumber));
        resources.scanners = toInt(fields[5], "processes.txt linha " + std::to_string(lineNumber));
        resources.modems = toInt(fields[6], "processes.txt linha " + std::to_string(lineNumber));
        resources.sataDrives = toInt(fields[7], "processes.txt linha " + std::to_string(lineNumber));

        processes.push_back(Process(pid, arrivalTime, priority, processorTime, workingSetSize, resources));
        pid++;
    }

    return processes;
}

std::vector<std::vector<int>> InputParser::readPageReferences(const std::string& path) {
    std::ifstream file(path.c_str());

    if (!file.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir o arquivo de strings de referencia: " + path);
    }

    std::vector<std::vector<int>> allReferences;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;

        if (isBlankLine(line)) {
            continue;
        }

        std::vector<std::string> fields = splitByComma(line);
        std::vector<int> references;

        for (std::size_t i = 0; i < fields.size(); i++) {
            references.push_back(toInt(fields[i], "string.txt linha " + std::to_string(lineNumber)));
        }

        allReferences.push_back(references);
    }

    return allReferences;
}

void InputParser::readFileSystemInput(
    const std::string& path,
    FileSystemManager& fileSystem
) {
    std::ifstream file(path.c_str());

    if (!file.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir o arquivo do sistema de arquivos: " + path);
    }

    std::string line;
    int lineNumber = 0;

    if (!std::getline(file, line)) {
        throw std::runtime_error("Arquivo de sistema de arquivos vazio: " + path);
    }
    lineNumber++;
    int totalBlocks = toInt(line, "files.txt linha 1");
    fileSystem.initializeDisk(totalBlocks);

    if (!std::getline(file, line)) {
        throw std::runtime_error("Arquivo de sistema de arquivos sem quantidade de segmentos ocupados.");
    }
    lineNumber++;
    int occupiedSegments = toInt(line, "files.txt linha 2");

    for (int i = 0; i < occupiedSegments; i++) {
        if (!std::getline(file, line)) {
            throw std::runtime_error("Faltam linhas de segmentos ocupados no arquivo files.txt.");
        }
        lineNumber++;

        if (isBlankLine(line)) {
            i--;
            continue;
        }

        std::vector<std::string> fields = splitByComma(line);

        if (fields.size() != 3) {
            throw std::runtime_error(
                "Linha " + std::to_string(lineNumber) +
                " do files.txt deveria ter 3 campos de segmento inicial."
            );
        }

        std::string fileName = fields[0];
        int startBlock = toInt(fields[1], "files.txt linha " + std::to_string(lineNumber));
        int size = toInt(fields[2], "files.txt linha " + std::to_string(lineNumber));

        fileSystem.addInitialFile(fileName, startBlock, size);
    }

    while (std::getline(file, line)) {
        lineNumber++;

        if (isBlankLine(line)) {
            continue;
        }

        std::vector<std::string> fields = splitByComma(line);

        if (fields.size() != 3 && fields.size() != 4) {
            throw std::runtime_error(
                "Linha " + std::to_string(lineNumber) +
                " do files.txt deveria ter 3 campos para delete ou 4 para create."
            );
        }

        FileOperation operation;
        operation.processPid = toInt(fields[0], "files.txt linha " + std::to_string(lineNumber));

        int operationCode = toInt(fields[1], "files.txt linha " + std::to_string(lineNumber));
        if (operationCode == 0) {
            operation.type = FileOperationType::CREATE;
        } else if (operationCode == 1) {
            operation.type = FileOperationType::DELETE;
        } else {
            throw std::runtime_error(
                "Codigo de operacao invalido na linha " + std::to_string(lineNumber) + " do files.txt."
            );
        }

        operation.fileName = fields[2];
        operation.size = 0;

        if (operation.type == FileOperationType::CREATE) {
            if (fields.size() != 4) {
                throw std::runtime_error(
                    "Operacao create sem tamanho na linha " + std::to_string(lineNumber) + " do files.txt."
                );
            }
            operation.size = toInt(fields[3], "files.txt linha " + std::to_string(lineNumber));
        }

        fileSystem.addOperation(operation);
    }
}
