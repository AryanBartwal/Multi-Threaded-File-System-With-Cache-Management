#include "fs/filesystem.hpp"
#include "common/logger.hpp"
#include "common/error.hpp"
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <sstream>

namespace mtfs::fs {

using namespace mtfs::common;  // Add this to use exceptions from common namespace

FileSystem::FileSystem(const std::string& rootPath) 
    : rootPath(rootPath), fileCache(CACHE_CAPACITY) {
    LOG_INFO("Initializing filesystem at: " + rootPath);
    _mkdir(rootPath.c_str());
}

std::shared_ptr<FileSystem> FileSystem::create(const std::string& rootPath) {
    return std::shared_ptr<FileSystem>(new FileSystem(rootPath));
}

bool FileSystem::createFile(const std::string& path) {
    try {
        std::string fullPath = rootPath + "/" + path;
        std::ofstream file(fullPath);
        if (!file) {
            LOG_ERROR("Failed to create file: " + path);
            throw FSException("Failed to create file: " + path);
        }
        file.close();
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error creating file: ") + e.what());
        throw;
    }
}

bool FileSystem::writeFile(const std::string& path, const std::string& data) {
    try {
        std::string fullPath = rootPath + "/" + path;
        if (!exists(path)) {
            throw FileNotFoundException(path);
        }

        std::ofstream file(fullPath);
        if (!file) {
            throw FSException("Failed to open file for writing: " + path);
        }
        file << data;
        
        // Update cache with new content
        fileCache.put(path, data);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error writing file: ") + e.what());
        throw;
    }
}

std::string FileSystem::readFile(const std::string& path) {
    try {
        // Try to get from cache first
        std::string cachedData;
        if (fileCache.get(path, cachedData)) {
            LOG_DEBUG("Cache hit for file: " + path);
            return cachedData;
        }

        // Cache miss, read from disk
        LOG_DEBUG("Cache miss for file: " + path);
        std::string fullPath = rootPath + "/" + path;
        if (!exists(path)) {
            throw FileNotFoundException(path);
        }

        std::ifstream file(fullPath);
        if (!file) {
            throw FSException("Failed to open file for reading: " + path);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string data = buffer.str();
        
        // Add to cache
        fileCache.put(path, data);
        return data;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error reading file: ") + e.what());
        throw;
    }
}

bool FileSystem::deleteFile(const std::string& path) {
    try {
        std::string fullPath = rootPath + "/" + path;
        if (!exists(path)) {
            throw FileNotFoundException(path);
        }

        // Remove from cache first
        fileCache.clear();
        return remove(fullPath.c_str()) == 0;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error deleting file: ") + e.what());
        throw;
    }
}

bool FileSystem::createDirectory(const std::string& path) {
    try {
        std::string fullPath = rootPath + "/" + path;
        return _mkdir(fullPath.c_str()) == 0;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error creating directory: ") + e.what());
        throw;
    }
}

std::vector<std::string> FileSystem::listDirectory(const std::string& path) {
    try {
        std::string fullPath = rootPath + "/" + path;
        if (!exists(path)) {
            throw FileNotFoundException(path);
        }

        std::vector<std::string> entries;
        _finddata_t fileinfo;
        intptr_t handle = _findfirst((fullPath + "/*").c_str(), &fileinfo);
        
        if (handle != -1) {
            do {
                std::string name = fileinfo.name;
                if (name != "." && name != "..") {
                    entries.push_back(name);
                }
            } while (_findnext(handle, &fileinfo) == 0);
            _findclose(handle);
        }
        
        return entries;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error listing directory: ") + e.what());
        throw;
    }
}

FileMetadata FileSystem::getMetadata(const std::string& path) {
    try {
        std::string fullPath = rootPath + "/" + path;
        if (!exists(path)) {
            throw FileNotFoundException(path);
        }

        struct stat stats;
        if (stat(fullPath.c_str(), &stats) != 0) {
            throw FSException("Failed to get file stats: " + path);
        }

        FileMetadata metadata;
        metadata.name = path.substr(path.find_last_of("/\\") + 1);
        metadata.size = stats.st_size;
        metadata.isDirectory = (stats.st_mode & S_IFDIR) != 0;
        metadata.permissions = stats.st_mode & 0777;
        metadata.modifiedAt = std::chrono::system_clock::from_time_t(stats.st_mtime);
        metadata.createdAt = std::chrono::system_clock::from_time_t(stats.st_ctime);

        return metadata;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error getting metadata: ") + e.what());
        throw;
    }
}

void FileSystem::setPermissions(const std::string& path, uint32_t permissions) {
    try {
        std::string fullPath = rootPath + "/" + path;
        if (!exists(path)) {
            throw FileNotFoundException(path);
        }

        if (_chmod(fullPath.c_str(), permissions) != 0) {
            throw FSException("Failed to set permissions: " + path);
        }
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error setting permissions: ") + e.what());
        throw;
    }
}

bool FileSystem::exists(const std::string& path) {
    try {
        std::string fullPath = rootPath + "/" + path;
        struct stat stats;
        return stat(fullPath.c_str(), &stats) == 0;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error checking existence: ") + e.what());
        throw;
    }
}

void FileSystem::sync() {
    // In a real implementation, this would flush all buffers to disk
    LOG_INFO("Syncing filesystem");
}

void FileSystem::mount() {
    LOG_INFO("Mounting filesystem at: " + rootPath);
    _mkdir(rootPath.c_str());
}

void FileSystem::unmount() {
    LOG_INFO("Unmounting filesystem from: " + rootPath);
    sync();
}

std::size_t FileSystem::write(const std::string& path, const void* buffer, std::size_t size, std::size_t offset) {
    try {
        std::string fullPath = rootPath + "/" + path;
        if (!exists(path)) {
            throw FileNotFoundException(path);
        }

        std::fstream file(fullPath, std::ios::binary | std::ios::in | std::ios::out);
        if (!file) {
            throw FSException("Failed to open file for writing: " + path);
        }

        file.seekp(offset);
        file.write(static_cast<const char*>(buffer), size);
        return size;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error in low-level write: ") + e.what());
        throw;
    }
}

std::size_t FileSystem::read(const std::string& path, void* buffer, std::size_t size, std::size_t offset) {
    try {
        std::string fullPath = rootPath + "/" + path;
        if (!exists(path)) {
            throw FileNotFoundException(path);
        }

        std::ifstream file(fullPath, std::ios::binary);
        if (!file) {
            throw FSException("Failed to open file for reading: " + path);
        }

        file.seekg(offset);
        file.read(static_cast<char*>(buffer), size);
        return file.gcount();
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error in low-level read: ") + e.what());
        throw;
    }
}

FileMetadata FileSystem::resolvePath(const std::string& path) {
    LOG_DEBUG("Resolving path: " + path);
    return getMetadata(path);
}

// Cache control methods
void FileSystem::clearCache() {
    fileCache.clear();
    LOG_INFO("File system cache cleared");
}

size_t FileSystem::getCacheSize() const {
    return fileCache.size();
}

} // namespace mtfs::fs 