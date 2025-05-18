#include "storage/block_manager.h"
#include "common/logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

using namespace mtfs::storage;
using namespace mtfs::common;

void printBlock(const std::vector<char>& data, size_t maxBytes = 64) {
    std::cout << "Block data (first " << maxBytes << " bytes): ";
    for (size_t i = 0; i < std::min(data.size(), maxBytes); ++i) {
        std::cout << (data[i] ? data[i] : '.');
    }
    std::cout << std::endl;
}

int main() {
    try {
        std::cout << "Initializing block manager...\n";
        BlockManager blockManager("./test_storage.bin");

        // Format storage
        std::cout << "\nFormatting storage...\n";
        blockManager.formatStorage();
        std::cout << "Free blocks: " << blockManager.getFreeBlocks() << "/" << blockManager.getTotalBlocks() << std::endl;

        // Allocate a block
        std::cout << "\nAllocating block...\n";
        int blockId = blockManager.allocateBlock();
        if (blockId < 0) {
            throw std::runtime_error("Failed to allocate block");
        }
        std::cout << "Allocated block ID: " << blockId << std::endl;
        std::cout << "Free blocks: " << blockManager.getFreeBlocks() << "/" << blockManager.getTotalBlocks() << std::endl;

        // Write data to block
        std::cout << "\nWriting data to block...\n";
        std::string testData = "Hello, Block Storage! This is a test message to verify block writing and reading functionality.";
        std::vector<char> writeData(testData.begin(), testData.end());
        if (!blockManager.writeBlock(blockId, writeData)) {
            throw std::runtime_error("Failed to write block");
        }

        // Read data back
        std::cout << "\nReading data from block...\n";
        std::vector<char> readData;
        if (!blockManager.readBlock(blockId, readData)) {
            throw std::runtime_error("Failed to read block");
        }
        printBlock(readData);

        // Verify data
        bool dataMatch = std::equal(writeData.begin(), writeData.end(), readData.begin());
        std::cout << "\nData verification: " << (dataMatch ? "PASSED" : "FAILED") << std::endl;
        assert(dataMatch && "Data verification failed");

        // Free block
        std::cout << "\nFreeing block...\n";
        if (!blockManager.freeBlock(blockId)) {
            throw std::runtime_error("Failed to free block");
        }
        std::cout << "Free blocks: " << blockManager.getFreeBlocks() << "/" << blockManager.getTotalBlocks() << std::endl;

        // Try to read freed block (should fail)
        std::cout << "\nTrying to read freed block (should fail)...\n";
        if (blockManager.readBlock(blockId, readData)) {
            throw std::runtime_error("Successfully read a freed block (unexpected)");
        }

        std::cout << "\nAll tests completed successfully!\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 