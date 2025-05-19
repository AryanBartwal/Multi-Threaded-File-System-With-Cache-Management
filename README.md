# Multi-Threaded File System With Cache Management

A high-performance, thread-safe file system implementation with integrated caching and journaling capabilities.

## Project Structure

```
.
├── src/                    # Source code
│   ├── filesystem/        # Core file system operations
│   │   ├── FileSystem.cpp
│   │   └── FileSystem.h
│   ├── cache/            # LRU cache implementation
│   │   ├── Cache.cpp
│   │   └── Cache.h
│   ├── storage/          # Disk I/O and block management
│   │   ├── BlockManager.cpp
│   │   └── BlockManager.h
│   ├── journal/          # Crash recovery system
│   │   ├── Journal.cpp
│   │   └── Journal.h
│   └── cli/              # Command-line interface
│       ├── main.cpp
│       └── CLI.h
├── include/              # Header files
├── test/                # Unit tests
├── CMakeLists.txt       # Build configuration
└── README.md           # Project documentation
```

## Features

### Implemented

- Thread-safe file operations with mutex-based synchronization
- LRU caching system with configurable size
- Basic block management for file storage
- Command-line interface supporting:
  - File operations (create, read, write, delete)
  - Directory operations (create, list, remove)
  - Cache management (view cache status)
- Journaling system for crash recovery
- Comprehensive error handling and logging

### In Progress

- Performance optimization
- Advanced caching strategies
- Extended file system features

## Prerequisites

- C++17 or later
- CMake 3.15+
- A modern C++ compiler (GCC 7+, Clang 6+, or MSVC 2019+)

## Building the Project

```bash
# Create build directory
mkdir build && cd build

# Generate build files
cmake ..

# Build the project
cmake --build .
```

## Usage

The system provides a command-line interface with the following commands:

```
File Operations:
  create <path> <size>    Create a new file
  read <path>            Read file contents
  write <path> <data>    Write data to file
  delete <path>          Delete a file

Directory Operations:
  mkdir <path>           Create a new directory
  ls <path>              List directory contents
  rmdir <path>           Remove a directory

Cache Management:
  cache                  Show cache status

System:
  exit                   Exit the program
  help                   Show this help message
```

## Implementation Details

### File System

- Thread-safe operations using mutex locks
- Hierarchical file structure
- Support for basic file operations

### Cache System

- LRU (Least Recently Used) eviction policy
- Configurable cache size
- Thread-safe cache operations

### Block Management

- Efficient block allocation and deallocation
- Block-level operations for file storage
- Journaling for crash recovery

### Journaling

- Transaction-based logging
- Crash recovery support
- Atomic operations

## Development Guidelines

1. **Code Style**

   - Follow Google C++ Style Guide
   - Use meaningful variable and function names
   - Document public APIs

2. **Threading**

   - Use mutex locks for thread safety
   - Document thread safety guarantees
   - Avoid deadlocks

3. **Error Handling**
   - Use exceptions for exceptional conditions
   - Log errors appropriately
   - Maintain strong exception guarantees

## License

[MIT License](LICENSE)

## Contributing

See [CONTRIBUTING.md](docs/CONTRIBUTING.md) for detailed guidelines.
