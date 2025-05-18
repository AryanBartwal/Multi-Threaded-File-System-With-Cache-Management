#include "fs/filesystem.hpp"
#include "common/logger.hpp"
#include <iostream>
#include <chrono>

using namespace mtfs::fs;
using namespace std::chrono;

// Helper function to measure operation time
template<typename Func>
double measureTime(Func&& func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count() / 1000.0; // Convert to milliseconds
}

int main() {
    try {
        std::cout << "Creating filesystem with cache...\n";
        auto fs = FileSystem::create("./test_cache_fs");

        // Create and write to a test file
        const std::string filename = "test.txt";
        const std::string content = "This is a test file content that we will read multiple times to demonstrate caching.";
        
        std::cout << "\nCreating and writing to file: " << filename << "\n";
        fs->createFile(filename);
        fs->writeFile(filename, content);

        // First read (should be from disk)
        std::cout << "\nFirst read (from disk):\n";
        double firstReadTime = measureTime([&]() {
            std::string data = fs->readFile(filename);
            std::cout << "Content: " << data << "\n";
        });
        std::cout << "Time taken: " << firstReadTime << " ms\n";
        std::cout << "Cache size: " << fs->getCacheSize() << "\n";

        // Second read (should be from cache)
        std::cout << "\nSecond read (from cache):\n";
        double secondReadTime = measureTime([&]() {
            std::string data = fs->readFile(filename);
            std::cout << "Content: " << data << "\n";
        });
        std::cout << "Time taken: " << secondReadTime << " ms\n";
        std::cout << "Cache size: " << fs->getCacheSize() << "\n";

        // Clear cache and read again
        std::cout << "\nClearing cache and reading again:\n";
        fs->clearCache();
        std::cout << "Cache size after clear: " << fs->getCacheSize() << "\n";
        
        double thirdReadTime = measureTime([&]() {
            std::string data = fs->readFile(filename);
            std::cout << "Content: " << data << "\n";
        });
        std::cout << "Time taken (after cache clear): " << thirdReadTime << " ms\n";
        std::cout << "Cache size: " << fs->getCacheSize() << "\n";

        // Update file content
        std::cout << "\nUpdating file content:\n";
        const std::string newContent = "This is updated content. The cache should be updated automatically.";
        fs->writeFile(filename, newContent);

        // Read updated content (should be from cache)
        std::cout << "\nReading updated content (from cache):\n";
        double fourthReadTime = measureTime([&]() {
            std::string data = fs->readFile(filename);
            std::cout << "Content: " << data << "\n";
        });
        std::cout << "Time taken: " << fourthReadTime << " ms\n";
        std::cout << "Cache size: " << fs->getCacheSize() << "\n";

        // Clean up
        std::cout << "\nCleaning up...\n";
        fs->deleteFile(filename);
        std::cout << "Final cache size: " << fs->getCacheSize() << "\n";

        std::cout << "\nTest completed successfully!\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
} 