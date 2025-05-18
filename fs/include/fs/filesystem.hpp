#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <chrono>
#include "common/error.hpp"
#include "cache/lru_cache.h"

namespace mtfs::fs {

struct FileMetadata {
    std::string name;
    std::size_t size{0};
    bool isDirectory{false};
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point modifiedAt;
    uint32_t permissions{0644};  // Default Unix-style permissions
};

class FileSystem {
public:
    // Factory method
    static std::shared_ptr<FileSystem> create(const std::string& rootPath);

    // Basic file operations
    bool createFile(const std::string& path);
    bool writeFile(const std::string& path, const std::string& data);
    std::string readFile(const std::string& path);
    bool deleteFile(const std::string& path);
    
    // Directory operations
    bool createDirectory(const std::string& path);
    std::vector<std::string> listDirectory(const std::string& path);
    
    // Advanced operations
    std::size_t write(const std::string& path, const void* buffer, std::size_t size, std::size_t offset);
    std::size_t read(const std::string& path, void* buffer, std::size_t size, std::size_t offset);
    void setPermissions(const std::string& path, uint32_t permissions);
    FileMetadata getMetadata(const std::string& path);
    
    // System operations
    void sync();
    void mount();
    void unmount();
    bool exists(const std::string& path);
    
    // Cache control
    void clearCache();
    size_t getCacheSize() const;
    
    virtual ~FileSystem() = default;

protected:
    explicit FileSystem(const std::string& rootPath);

private:
    FileMetadata resolvePath(const std::string& path);
    std::string rootPath;
    
    // Cache for file contents
    static constexpr size_t CACHE_CAPACITY = 1000;
    cache::LRUCache<std::string, std::string> fileCache;
};

} // namespace mtfs::fs 