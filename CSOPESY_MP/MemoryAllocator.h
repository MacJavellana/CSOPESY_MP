#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <sstream>

class IMemory {
public:
	virtual void* allocate(size_t size) = 0;
	virtual void deallocate(void* ptr, size_t size) = 0;
	virtual std::string printMemory() = 0;
};

class MemoryAllocator : public IMemory {
public:
    MemoryAllocator(size_t maxSize): maxSize(maxSize), usedMemorySize(0), memory(maxSize, '.') {
        for (size_t i = 0; i < maxSize; ++i) {
            memoryUsageMap[i] = false;
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

    void* allocate(size_t size) override { // Finds the first available block to send the process in
        for (size_t i = 0; i < maxSize - size + 1; ++i) {
            if (!memoryUsageMap[i] && isMemoryAvailable(i, size)) {
                allocateMemoryAt(i, size);
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

        for (size_t i = 0; i < memory.size(); ++i) {
            if (currentMemoryPosition == 0) {
                break;
            }
            if (memoryUsageMap[i]) {
                ss << currentMemoryPosition << "\n";
                ss << "P" << i + 1 << "\n";
                currentMemoryPosition -= 4096;
                ss << currentMemoryPosition << "\n\n";
            }
            else {
                ss << "\n";
                currentMemoryPosition -= 16;
                ss << currentMemoryPosition << "\n";
            }
        }
        ss << "----start---- = 0\n";
        return ss.str();
    }

private:
    size_t maxSize;
    size_t usedMemorySize;
    std::vector<char> memory;
    std::unordered_map<size_t, bool> memoryUsageMap;
    int allocationCount = 0;

    void clearMemoryUsage() {
        std::fill(memory.begin(), memory.end(), '.'); // Reset memory to unallocated
        memoryUsageMap.clear();                       // Clear existing entries
        for (size_t i = 0; i < maxSize; ++i) {
            memoryUsageMap[i] = false;
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

    void allocateMemoryAt(size_t index, size_t size) {
        for (size_t i = index; i < index + size; ++i) {
            memoryUsageMap[i] = true;
        }
        usedMemorySize += size;
        allocationCount++;
    }

    void deallocateMemoryAt(size_t index, size_t size) {
        for (size_t i = index; i < index + size; ++i) {
            memoryUsageMap[i] = false;
        }
        usedMemorySize -= size;
        allocationCount--;
    }
};