#include <iostream>
#include <stdexcept>
#include "Dispatcher.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Uso: ./dispatcher processes.txt files.txt string.txt\n";
        return 1;
    }

    try {
        Dispatcher dispatcher;
        dispatcher.load(argv[1], argv[2], argv[3]);
        dispatcher.run();
    } catch (const std::exception& error) {
        std::cerr << "Erro: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
