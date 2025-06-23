# What "Standard" Implementations Are We Comparing Against?

When we say we're comparing our **custom implementations** vs **standard implementations**, here's exactly what we mean:

## 1. File System Operations

### Custom Implementation (Our Project):
- Our custom `FileSystem` class with metadata management, journaling, and caching
- Location: `fs/src/filesystem.cpp` and `fs/include/fs/filesystem.hpp`

### Standard Implementation (Baseline):
- **`std::ofstream`** and **`std::ifstream`** - Direct C++ standard library file I/O
- **`std::filesystem`** operations for file/directory management
- Example in benchmark:
```cpp
// Standard approach
std::ofstream ofs("test_file.txt");
ofs << data;
ofs.close();

std::ifstream ifs("test_file.txt");
std::string content((std::istreambuf_iterator<char>(ifs)),
                   std::istreambuf_iterator<char>());
```

## 2. Cache Operations

### Custom Implementation (Our Project):
- Our custom `LRUCache` class with thread-safe LRU eviction
- Location: `cache/src/lru_cache.cpp` and `cache/include/cache/lru_cache.hpp`

### Standard Implementation (Baseline):
- **`std::unordered_map<K, V>`** - Standard hash map with no eviction policy
- Example in benchmark:
```cpp
// Standard approach
std::unordered_map<std::string, std::string> map;
map["key"] = "value";
auto it = map.find("key");
```

**Why our custom cache is valuable:**
- LRU eviction prevents unlimited memory growth
- Thread-safe operations for multi-threaded access
- Cache hit/miss statistics and monitoring
- Optimized for frequent access patterns

## 3. Compression Operations

### Custom Implementation (Our Project):
- Our custom compression algorithms in `fs/src/compression.cpp`
- May include LZ77, RLE, or other custom algorithms

### Standard Implementation (Baseline):
- **No compression** (just copy the data)
- **Simple string operations** like `std::reverse()` for performance comparison
- Example in benchmark:
```cpp
// Standard approach (no compression)
std::string copy = original_data;

// Or simple string manipulation
std::string reversed = data;
std::reverse(reversed.begin(), reversed.end());
```

## 4. Backup Operations

### Custom Implementation (Our Project):
- Our custom `BackupManager` class with incremental backups, metadata preservation
- Location: `fs/src/backup_manager.cpp`

### Standard Implementation (Baseline):
- **`std::filesystem::copy_file()`** and **`std::filesystem::copy()`**
- Basic file copying without metadata, versioning, or incremental features
- Example in benchmark:
```cpp
// Standard approach
std::filesystem::copy_file(src, dst, 
    std::filesystem::copy_options::overwrite_existing);
```

## Why Compare Against Standards?

1. **Performance Baseline**: Shows if our custom implementations have reasonable overhead
2. **Feature Value**: Demonstrates what extra features we provide beyond basic operations
3. **Trade-off Analysis**: Shows where we sacrifice speed for functionality (and vice versa)

## Expected Results:

- **File Operations**: Our custom FS may be slower due to metadata/journaling overhead, but provides crash recovery and consistency
- **Cache Operations**: Our LRU cache may be slower for simple get/put, but prevents memory exhaustion and provides better long-term performance
- **Compression**: Our algorithms will be slower than no-compression, but save storage space
- **Backup**: Our backup system may be slower than simple copy, but provides versioning and incremental backups

## Realistic Cache Performance:

The benchmarks include realistic scenarios like:
- **80% hit rate simulation**: Most real applications have high cache hit rates
- **File system cache simulation**: Shows how our cache would perform in real file access patterns
- **Hot data access**: Demonstrates LRU efficiency for frequently accessed items

This is why our custom implementations are valuable - they provide features that standard library components don't offer, even if they have some performance overhead.
