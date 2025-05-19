# Multi-Threaded File System With Cache Management - Technical Documentation

## Overview

This document provides a detailed technical explanation of each module in the Multi-Threaded File System with Cache Management project.

## Table of Contents

1. [File System Module](#file-system-module)
2. [Cache Module](#cache-module)
3. [Storage Module](#storage-module)
4. [Threading Module](#threading-module)
5. [Journal Module](#journal-module)
6. [Test Module](#test-module)
7. [Integration Points](#integration-points)
8. [Performance Features](#performance-features)

## File System Module

### Core Purpose

Provides the main interface for file operations

### Key Components

- `FileSystem` class with operations like create, read, write, delete
- `FileMetadata` struct for storing file information
- Integration with cache system

### Key Features

- Basic file operations (create, read, write, delete)
- Directory operations (create, list)
- Advanced operations (partial read/write, permissions)
- Cache integration for performance
- Thread-safe operations

### Implementation Details

```cpp
struct FileMetadata {
    std::string name;
    std::size_t size{0};
    bool isDirectory{false};
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point modifiedAt;
    uint32_t permissions{0644};  // Default Unix-style permissions
};
```

## Cache Module

### Core Purpose

Implements LRU (Least Recently Used) caching for file contents

### Key Components

- Template-based `LRUCache` class
- Thread-safe implementation using Windows Critical Sections

### Key Features

- O(1) lookup time using hash map
- LRU eviction policy
- Thread-safe operations
- Configurable cache capacity
- Logging for cache operations
- Memory-efficient storage

### Implementation Details

The cache uses a combination of:

- Hash map for fast lookups
- Doubly linked list for LRU tracking
- Critical sections for thread safety

## Storage Module

### Core Purpose

Manages low-level block storage operations

### Key Components

- `BlockManager` class for block-level operations
- Bitmap-based block allocation

### Key Features

- 4KB block size
- Thread-safe block operations
- Bitmap-based free space management
- Direct disk I/O
- Future support for compression and encryption
- Block allocation and deallocation

### Implementation Details

```cpp
static constexpr size_t BLOCK_SIZE = 4096;  // 4KB blocks
static constexpr size_t MAX_BLOCKS = 1024;  // Initial maximum blocks
static constexpr size_t BITMAP_BYTES = (MAX_BLOCKS + 7) / 8;  // Size of bitmap in bytes
```

## Threading Module

### Core Purpose

Manages concurrent operations through a thread pool

### Key Components

- `ThreadPool` class for task management
- Task queue and worker threads

### Key Features

- Dynamic task submission
- Thread pool management
- Task queuing
- Pause/resume capability
- Status monitoring
- Future-based task results

### Implementation Details

The thread pool provides:

- Task submission with future results
- Worker thread management
- Thread-safe task queue
- Pause/resume functionality

## Journal Module

### Core Purpose

Provides crash recovery and transaction support

### Key Features

- Transaction logging
- Crash recovery
- Operation rollback
- Data consistency maintenance

## Test Module

### Core Purpose

Validates system functionality

### Key Components

- Integration tests
- Unit tests for each module
- Performance benchmarks

### Test Coverage

- File operations
- Cache performance
- Concurrent access
- Error handling
- Recovery scenarios

## Integration Points

### 1. FS ↔ Cache

- File system uses cache for fast access
- Cache updates on file modifications

### 2. FS ↔ Storage

- File system translates operations to block operations
- Block manager handles physical storage

### 3. Threading ↔ All Modules

- Ensures thread-safe operations
- Manages concurrent access

### 4. Journal ↔ FS

- Logs operations for recovery
- Maintains system consistency

## Performance Features

### 1. Caching

- LRU policy for optimal cache utilization
- Thread-safe cache operations
- Configurable cache size

### 2. Storage

- Efficient block allocation
- Direct disk I/O
- Bitmap-based space management

### 3. Threading

- Thread pool for operation parallelization
- Task queuing for load balancing
- Pause/resume capability

## Performance Metrics

Based on test results:

- Cache hit rates show significant improvement in read times
- First read: ~20ms (from disk)
- Second read: ~7ms (from cache)
- Third read: ~3ms (cached and optimized)
- Performance improvement: 3-7x with caching

## Future Enhancements

1. Block compression
2. Block encryption
3. Storage expansion capability
4. Block caching improvements
5. Defragmentation
6. Distributed file system support
7. Enhanced security features
8. Cloud storage integration

## Test Commands and Usage Guide

### Prerequisites

- Ensure all executables are compiled
- Required test directories exist (`test_fs/`, `test_cache_fs/`)
- Sufficient disk space for test files

### 1. Integration Tests

```bash
# Run complete integration test suite
./integration_test.exe

# Expected output:
# === Running Basic Functionality Test ===
# Test 1: File Creation and Writing
# Test 2: Cache Performance
# Test 3: Multiple Files and Directory Operations
# Test 4: Concurrent Access
# Test 5: Cleanup and Verification
```

### 2. File System Tests

```bash
# Run file system specific tests
./fs_test.exe

# Expected output:
# Created file: hello.txt
# Wrote content to file
# Read content: Hello, Multi-threaded File System!
# Deleted file: hello.txt
# Basic filesystem operations completed successfully!
```

### 3. Cache Tests

```bash
# Run cache performance tests
./test_fs_cache.exe

# Expected output:
# Creating filesystem with cache...
# First read (from disk): ~20ms
# Second read (from cache): ~7ms
# Third read (cached): ~3ms
# Test completed successfully!
```

### 4. Block Manager Tests

```bash
# Run storage block manager tests
./test_block_manager.exe

# Expected output:
# Initializing block manager...
# Testing block allocation...
# Testing block read/write...
# Testing block deallocation...
# All block manager tests passed!
```

### 5. Running Individual Test Components

#### File Operations Test

```bash
./fs_test.exe --test-type=file-ops

# Tests basic file operations:
# - Create/Delete files
# - Read/Write operations
# - Directory operations
```

#### Cache Performance Test

```bash
./test_fs_cache.exe --benchmark

# Runs cache performance benchmarks:
# - Cache hit rates
# - Read/Write latencies
# - Memory usage statistics
```

#### Concurrency Test

```bash
./integration_test.exe --test-type=concurrent

# Tests thread safety:
# - Multiple simultaneous operations
# - Thread pool performance
# - Resource contention handling
```

### 6. Test Environment Setup

```bash
# Create required test directories
mkdir -p test_fs
mkdir -p test_cache_fs

# Clean up test environment
rm -rf test_fs/*
rm -rf test_cache_fs/*
```

### 7. Common Test Options

All test executables support the following options:

```bash
--verbose        # Display detailed test information
--quiet         # Suppress all output except errors
--log=<path>    # Save test output to specified file
--timeout=<sec> # Set test timeout in seconds
```

### 8. Troubleshooting Tests

If tests fail, check:

1. Directory permissions
2. Available disk space
3. No other processes accessing test directories
4. All dependencies are properly installed

Common error resolution:

```bash
# Reset test environment
rm -rf test_fs/* test_cache_fs/*
mkdir -p test_fs test_cache_fs

# Check file permissions
chmod 755 test_fs test_cache_fs

# Verify executables
ls -l *.exe
```

### 9. Performance Testing

```bash
# Run complete performance benchmark
./integration_test.exe --benchmark

# Expected output:
# File Operations Benchmark:
# - Create: X ops/sec
# - Read: X ops/sec
# - Write: X ops/sec
# - Delete: X ops/sec
# Cache Performance:
# - Hit rate: X%
# - Miss rate: X%
# - Average latency: X ms
```
