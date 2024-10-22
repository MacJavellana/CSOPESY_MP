#include "ConsoleManager.h"


int main() {
    // init
    ConsoleManager::initialize();
    ConsoleManager* conman = ConsoleManager::get();

    // start
    conman->start();

    return 0;
}