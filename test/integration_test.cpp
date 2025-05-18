#include "fs/filesystem.hpp"
#include "storage/block_manager.h"
#include "cache/lru_cache.h"
#include "common/logger.hpp"
#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <chrono>
#include <windows.h>

using namespace mtfs::fs;
using namespace mtfs::storage;
using namespace mtfs::cache;
using namespace std::chrono;

// Helper function to measure execution time
template<typename Func>
double measureTime(Func&& func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count() / 1000.0; // Convert to milliseconds
}

// Thread function for concurrent testing
DWORD WINAPI threadFunction(LPVOID param) {
    auto fs = static_cast<FileSystem*>(param);
    try {
        static LONG counter = 0;
        LONG threadNum = InterlockedIncrement(&counter) - 1;
        
        std::string threadFile = "thread_" + std::to_string(threadNum) + ".txt";
        fs->createFile(threadFile);
        fs->writeFile(threadFile, "Thread " + std::to_string(threadNum) + " data");
        std::string content = fs->readFile(threadFile);
        return content.empty() ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Thread error: " << e.what() << "\n";
        return 0;
    }
}

void runBasicTest() {
    std::cout << "\n=== Running Basic Functionality Test ===\n";
    
    // Initialize components
    const std::string testRoot = "./test_fs";
    auto fs = FileSystem::create(testRoot);
    
    try {
        // Test 1: File Creation and Writing
        std::cout << "\nTest 1: File Creation and Writing\n";
        const std::string testFile = "test.txt";
        const std::string content = "Hello, this is a test file content!";
        
        bool created = fs->createFile(testFile);
        assert(created && "File creation failed");
        std::cout << "File created successfully\n";
        
        bool written = fs->writeFile(testFile, content);
        assert(written && "File write failed");
        std::cout << "Content written successfully\n";
        
        // Test 2: Cache Performance
        std::cout << "\nTest 2: Cache Performance\n";
        std::string readContent;
        
        // First read (from disk)
        double firstReadTime = measureTime([&]() {
            readContent = fs->readFile(testFile);
        });
        assert(readContent == content && "Content mismatch on first read");
        std::cout << "First read (disk): " << firstReadTime << " ms\n";
        
        // Second read (should be from cache)
        double secondReadTime = measureTime([&]() {
            readContent = fs->readFile(testFile);
        });
        assert(readContent == content && "Content mismatch on second read");
        std::cout << "Second read (cache): " << secondReadTime << " ms\n";
        std::cout << "Cache speedup: " << (firstReadTime / secondReadTime) << "x\n";
        
        // Test 3: Multiple Files and Directory Operations
        std::cout << "\nTest 3: Multiple Files and Directory Operations\n";
        const std::string testDir = "test_dir";
        fs->createDirectory(testDir);
        
        // Create multiple files in directory
        for (int i = 1; i <= 5; i++) {
            std::string fileName = testDir + "/file" + std::to_string(i) + ".txt";
            std::string fileContent = "Content of file " + std::to_string(i);
            fs->createFile(fileName);
            fs->writeFile(fileName, fileContent);
        }
        
        // List and verify directory contents
        auto files = fs->listDirectory(testDir);
        assert(files.size() == 5 && "Directory should contain 5 files");
        std::cout << "Created and verified 5 files in directory\n";
        
        // Test 4: Concurrent Access
        std::cout << "\nTest 4: Concurrent Access\n";
        const int numThreads = 4;
        std::vector<HANDLE> threads(numThreads);
        std::vector<DWORD> threadIds(numThreads);
        
        // Create threads
        for (int i = 0; i < numThreads; i++) {
            threads[i] = CreateThread(
                NULL,                   // Default security attributes
                0,                      // Default stack size
                threadFunction,         // Thread function
                fs.get(),              // Parameter to thread function
                0,                      // Default creation flags
                &threadIds[i]          // Receive thread identifier
            );
            assert(threads[i] != NULL && "Thread creation failed");
        }
        
        // Wait for all threads to complete
        WaitForMultipleObjects(numThreads, threads.data(), TRUE, INFINITE);
        
        // Clean up thread handles
        for (auto handle : threads) {
            CloseHandle(handle);
        }
        
        std::cout << "Successfully completed concurrent operations\n";
        
        // Test 5: Cleanup and Verification
        std::cout << "\nTest 5: Cleanup and Verification\n";
        
        // Delete test file and verify
        bool deleted = fs->deleteFile(testFile);
        assert(deleted && "File deletion failed");
        assert(!fs->exists(testFile) && "File should not exist after deletion");
        
        // Clear cache and try to read (should fail)
        fs->clearCache();
        try {
            fs->readFile(testFile);
            assert(false && "Reading deleted file should fail");
        } catch (const mtfs::common::FileNotFoundException&) {
            std::cout << "Successfully verified file deletion and cache clearing\n";
        }
        
        std::cout << "\n=== All Basic Functionality Tests Passed ===\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with error: " << e.what() << std::endl;
        throw;
    }
}

int main() {
    try {
        runBasicTest();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Integration test failed: " << e.what() << std::endl;
        return 1;
    }
} 