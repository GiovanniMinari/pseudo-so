#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <string>
#include <vector>
#include "Process.h"
#include "FileSystemManager.h"

class InputParser {
public:
    std::vector<Process> readProcesses(const std::string& path);

    std::vector<std::vector<int>> readPageReferences(const std::string& path);

    void readFileSystemInput(
        const std::string& path,
        FileSystemManager& fileSystem
    );
};

#endif