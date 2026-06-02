#include <iostream>
#include "Dispatcher.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Uso: ./dispatcher processes.txt files.txt string.txt\n";
        return 1;
    }

    Dispatcher dispatcher;

    dispatcher.load(argv[1], argv[2], argv[3]);
    dispatcher.run();
    dispatcher.printFinalReport();

    return 0;
}