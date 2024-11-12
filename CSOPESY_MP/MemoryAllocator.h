#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <sstream>

class IMemory {
public:
	virtual void* allocate(size_t size, int processID) = 0;
	virtual void deallocate(void* ptr, size_t size) = 0;
	virtual std::string printMemory() = 0;
};

class MemoryAllocator : public IMemory {
public:
    MemoryAllocator(size_t maxSize): maxSize(maxSize), usedMemorySize(0), memory(maxSize, '.') {
        for (size_t i = 0; i < maxSize; ++i) {
            memoryUsageMap[i] = false;
            processMap[i] = -1;
        }
    }

    ~MemoryAllocator() {
        memory.clear();
    }

    int getAllocationCount() {
        return allocationCount;
    }

    size_t getFragmentedSpace() {
        return maxSize - usedMemorySize;
    }

    void* allocate(size_t size, int processID) override { // Finds the first available block to send the process in
        for (size_t i = 0; i < maxSize - size + 1; ++i) {
            if (!memoryUsageMap[i] && isMemoryAvailable(i, size)) {
                allocateMemoryAt(i, size, processID);
                return &memory[i];
            }
        }
        return nullptr; // Returns a nullptr if no block is found
    }

    void deallocate(void* ptr, size_t size) override {
        size_t index = static_cast<char*>(ptr) - &memory[0];
        if (memoryUsageMap[index]) {
            deallocateMemoryAt(index, size);
        }
    }
   
    std::string printMemory() override {
        std::ostringstream ss;
        ss << "----end---- = " << maxSize << "\n";

        size_t currentMemoryPosition = maxSize;
        int index = maxSize - 1;

        while (index > 0) {
            if (memoryUsageMap[index]) {
                ss << index + 1 << "\n";
                ss << "P" << processMap[index] << "\n";
                index -= 4095; //this is fixed. assumes that these processes is 4096 bytes
                ss << index << "\n\n";
            }
            index--;
        }
        ss << "----start---- = 0\n";
        return ss.str();
    }

private:
    size_t maxSize;
    size_t usedMemorySize;
    std::vector<char> memory;
    std::unordered_map<size_t, bool> memoryUsageMap;
    std::unordered_map<size_t, bool> processMap;
    int allocationCount = 0;

    void clearMemoryUsage() {
        std::fill(memory.begin(), memory.end(), '.'); // Reset memory to unallocated
        memoryUsageMap.clear();                       // Clear existing entries
        for (size_t i = 0; i < maxSize; ++i) {
            memoryUsageMap[i] = false;
            processMap[i] = -1;
        }
    }

    bool isMemoryAvailable(size_t index, size_t size) const {
        if (index + size > maxSize) return false;
        for (size_t i = index; i < index + size; ++i) {
            if (memoryUsageMap.find(i) != memoryUsageMap.end() && memoryUsageMap.at(i)) {
                return false;
            }
        }
        return true;
    }

    void allocateMemoryAt(size_t index, size_t size, int processID) {
        for (size_t i = index; i < index + size; ++i) {
            memoryUsageMap[i] = true;
            processMap[i] = processID;
        }
        usedMemorySize += size;
        allocationCount++;
    }

    void deallocateMemoryAt(size_t index, size_t size) {
        for (size_t i = index; i < index + size; ++i) {
            memoryUsageMap[i] = false;
            processMap[i] = -1;
        }
        usedMemorySize -= size;
        allocationCount--;
    }
};