#pragma once
#ifndef PAGINGALLOCATOR_H
#define PAGINGALLOCATOR_H

#include "IAllocator.h"
#include <queue>
#include <unordered_map>

class PagingAllocator : public IAllocator {
public:
    PagingAllocator(int maxMemory);
    ~PagingAllocator() = default;

    bool allocate(std::shared_ptr<Process> process) override;
    void deallocate(std::shared_ptr<Process> process) override;

    void printMem() override;
    void printProcesses() override;
    void vmstat() override;

private:
    std::queue<int> _freeFrameList;
    // procName > vector(page number)[frame]
    std::unordered_map <std::string, std::vector<int>> _pageTable;
    int _maxMemory;
    int _pagedIn = 0;
    int _pagedOut = 0;
};

#endif //!PAGINGALLOCATOR_H