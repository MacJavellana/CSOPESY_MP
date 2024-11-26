#pragma once
#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <string>
#include <unordered_map>
#include "Process.h"
#include <memory>
#include "IAllocator.h"

class MemoryManager {
public:
    MemoryManager(int maxMemory, int minPage, int maxPage);
    ~MemoryManager() = default;

    bool allocate(std::shared_ptr<Process> process);
    void deallocate(std::shared_ptr<Process> process);

    void printMem(int qq);
    void vmstat();

    IAllocator* getAllocator() {
        return this->_allocator;
    }

private:
    IAllocator* _allocator;
};

#endif // !MEMORYMANAGER_H