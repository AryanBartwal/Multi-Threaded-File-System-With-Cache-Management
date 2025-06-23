# Multi-Threaded File System With Cache Management

A high-performance file system implementation with multi-threading support and advanced cache management capabilities.

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Building the Project](#building-the-project)
3. [Running the Project](#running-the-project)
4. [CLI Feature Demo](#cli-feature-demo)
5. [Project Structure](#project-structure)
6. [Benchmarking](#benchmarking)
7. [Development](#development)
8. [License](#license)

## Prerequisites

- **CMake** (version 3.15 or higher)
- **C++17** compatible compiler (GCC, Clang, or MSVC)
- **Git** for version control

## Building the Project

### Step 1: Clone the Repository
```bash
git clone <repository-url>
cd Multi-Threaded-File-System-With-Cache-Management
```

### Step 2: Create Build Directory
```bash
mkdir build
cd build
```

### Step 3: Configure with CMake
```bash
cmake ..
```

### Step 4: Build the Project
```bash
cmake --build .
```

## Running the Project

After building, you can find the executables in the `build` directory:

- **Main CLI application**: `build/cli/mtfs-cli`
- **Test executables**: `build/test/`
- **Benchmark tools**: `build/benchmark/`

### Starting the CLI Application
```bash
./cli/mtfs-cli
```

## CLI Feature Demo

Here's a comprehensive demonstration of CLI commands for the multi-threaded file system. Each section shows specific features with clear examples.

### 1. File Operations
**Basic file creation, writing, reading, and information:**
```bash
create-file demo.txt
write-file demo.txt "Hello, world!"
read-file demo.txt
file-info demo.txt
```
*Creates a file, writes text content, reads it back, and displays file information.*

### 2. Directory Operations
**Directory creation and listing:**
```bash
create-dir demodir
list-dir .
list-dir demodir
```
*Creates a directory and lists contents of current and new directories.*

### 3. File Management Operations
**Copy, move, rename, find, and delete operations:**
```bash
copy-file demo.txt demo_copy.txt
move-file demo_copy.txt demodir/demo_moved.txt
rename-file demo.txt demo_renamed.txt
find-file demo*
delete-file demodir/demo_moved.txt
delete-file demo_renamed.txt
```
*Demonstrates file copying, moving, renaming, searching, and deletion.*

### 4. Compression Features
**File compression and decompression:**
```bash
create-file compressme.txt
write-file compressme.txt "Compress this file!"
compress-file compressme.txt
compression-stats
decompress-file compressme.txt
delete-file compressme.txt
```
*Shows compression workflow with statistics tracking.*

### 5. Backup Management
**Backup creation, listing, and restoration:**
```bash
create-backup mybackup
list-backups
backup-dashboard
restore-backup mybackup demodir
delete-backup mybackup
```
*Demonstrates complete backup lifecycle management.*

### 6. Cache Management
**Cache policy, resizing, and file operations:**
```bash
set-cache-policy LRU
get-cache-policy
resize-cache 10
pin-file demo_renamed.txt
unpin-file demo_renamed.txt
prefetch-file demo_renamed.txt
cache-analytics
hot-files 3
```
*Shows cache configuration, file pinning, prefetching, and analytics.*

### 7. System Statistics
**Statistics viewing and management:**
```bash
show-stats
reset-stats
```
*Displays and resets file system performance statistics.*

### 8. Exit Application
```bash
exit
```
*Cleanly exits the CLI application.*

## Complete CLI Test Suite

This comprehensive test suite covers all CLI commands and can be used for thorough testing and demonstration purposes. Copy and paste each command into the CLI to test all features systematically.

### User Management Tests
```bash
register testuser password123
login testuser password123
whoami
logout
register adminuser adminpass admin
login adminuser adminpass
whoami
```

### Basic File Operations Tests
```bash
create-file test1.txt
write-file test1.txt "Hello World! This is a test file."
read-file test1.txt
file-info test1.txt
create-file test2.txt
write-file test2.txt "This is the second test file with different content."
```

### Directory Operations Tests
```bash
create-dir testdir1
create-dir testdir2
list-dir .
list-dir testdir1
```

### File Management Tests
```bash
copy-file test1.txt test1_copy.txt
move-file test2.txt testdir1/test2_moved.txt
rename-file test1_copy.txt test1_renamed.txt
list-dir .
list-dir testdir1
```

### Search Operations Tests
```bash
find-file "test*"
find-file "*.txt"
find-file "test" testdir1
```

### Compression Tests
```bash
compress-file test1.txt
compression-stats
decompress-file test1.txt
```

### Backup Operations Tests
```bash
create-backup my_first_backup
list-backups
backup-dashboard
create-file important.txt
write-file important.txt "This is important data to backup"
create-backup my_second_backup
list-backups
```

### Cache Management Tests
```bash
get-cache-policy
set-cache-policy LRU
get-cache-policy
set-cache-policy LFU
set-cache-policy FIFO
pin-file test1_renamed.txt
prefetch-file important.txt
cache-analytics
hot-files 5
```

### Statistics Tests
```bash
show-stats
reset-stats
show-stats
```

### Restore Testing
```bash
delete-file important.txt
list-dir .
restore-backup my_second_backup
list-dir .
```

### Cleanup Tests
```bash
delete-file test1_renamed.txt
delete-file testdir1/test2_moved.txt
delete-file important.txt
delete-backup my_first_backup
delete-backup my_second_backup
remove-user testuser
remove-user adminuser
exit
```

**Note**: This test suite creates temporary files and users for testing purposes. The cleanup section at the end removes all test artifacts to restore the system to its original state.

## Project Structure

The project is organized into modular components:

```
Multi-Threaded-File-System-With-Cache-Management/
├── cli/                 # Command-line interface implementation
├── fs/                  # Core file system implementation
├── cache/               # Cache management system
├── storage/             # Storage layer implementation
├── journal/             # Journaling system
├── common/              # Common utilities and shared code
├── threading/           # Threading utilities
├── test/                # Test suite
├── benchmark/           # Performance benchmarking tools
├── docs/                # Documentation
└── build/               # Build artifacts (generated)
```

### Component Details
- **`cli/`**: Interactive command-line interface with full feature access
- **`fs/`**: Core file system operations, compression, and backup management
- **`cache/`**: LRU cache implementation with advanced features
- **`storage/`**: Low-level storage operations and block management
- **`journal/`**: Transaction logging and crash recovery
- **`common/`**: Shared utilities, data structures, and helper functions
- **`threading/`**: Thread pool and synchronization primitives
- **`test/`**: Comprehensive test suite for all components
- **`benchmark/`**: Performance measurement and comparison tools

## Benchmarking

The project includes comprehensive benchmarking to measure performance across all features with side-by-side comparisons to standard implementations.

### Building and Running Benchmarks

Benchmarks are built automatically with the project:

```bash
cd build/benchmark
./benchmark_main
```

### Benchmark Features

#### 1. **Side-by-Side Comparisons**
   - Custom LRU Cache vs. `std::unordered_map`
   - Custom File System vs. `std::filesystem`
   - Real-time performance metrics during execution

#### 2. **Comprehensive Coverage**
   - **File Operations**: Read, write, copy, move, rename, delete
   - **Directory Operations**: Create, list, navigate, remove
   - **Cache Operations**: Put, get, eviction, hit/miss ratios
   - **Compression**: Compress/decompress with size reduction stats
   - **Backup**: Create, restore, verification operations
   - **Multi-threading**: Concurrent access patterns

#### 3. **Live Statistics**
   - Real-time operation timing
   - Cache hit/miss ratios
   - Memory usage tracking
   - Throughput measurements

### Example Benchmark Output

```
=== File System Benchmark Results ===
File Read (1MB):
  Custom FS:     1.234 ms  (810.4 MB/s)
  std::fstream:  1.567 ms  (637.8 MB/s)
  Improvement:   21.2% faster

Cache Performance (10,000 operations):
  Custom LRU:    567 ns/op  (95.2% hit rate)
  std::map:      890 ns/op  (N/A hit rate)
  Improvement:   36.3% faster

Compression (1MB text):
  Compress:      45.6 ms   (78.2% size reduction)
  Decompress:    23.1 ms   (99.9% accuracy)
```

### Custom Benchmark Classes

The project includes specialized benchmark classes:

- **`SideBySideBenchmark`**: Direct comparisons between implementations
- **`LiveBenchmark`**: Real-time performance monitoring
- **Custom LRU Cache**: Optimized cache with advanced features
- **Custom File System**: High-performance file operations

## Development

### Building Tests

Tests are automatically built with the project:

```bash
cd build
ctest
```

### Code Style and Standards

- **Language**: C++17
- **Build System**: CMake 3.15+
- **Threading**: Modern C++ threading primitives
- **Memory Management**: RAII and smart pointers
- **Error Handling**: Exception-safe design

### Contributing

1. Fork the repository
2. Create a feature branch
3. Write tests for new functionality
4. Ensure all tests pass
5. Submit a pull request

## Performance Characteristics

### Multi-Threading Benefits
- **Concurrent file operations**: Up to 4x throughput improvement
- **Cache efficiency**: Reduced contention with fine-grained locking
- **Background operations**: Non-blocking backup and compression

### Cache Management
- **LRU eviction policy**: Optimal for temporal locality
- **Memory efficiency**: Configurable cache sizes
- **Hit rate optimization**: Prefetching and pinning support

### Storage Optimization
- **Block-level operations**: Minimized I/O overhead
- **Journal integrity**: Crash-safe operations
- **Compression integration**: Transparent space savings

## License

This project is licensed under the MIT License - see the LICENSE file for details.

---

**Note**: This is a demonstration project showcasing advanced file system concepts including multi-threading, caching, compression, and backup management with comprehensive benchmarking capabilities.
