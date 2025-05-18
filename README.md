# Multi-Threaded File System With Cache Management

A high-performance, thread-safe file system implementation with integrated caching and journaling capabilities.

## Project Structure

```
.
├── fs/             # Core file system operations
├── cache/          # LRU cache implementation
├── storage/        # Disk I/O and block management
├── threading/      # Thread pool and synchronization
├── cli/            # Command-line interface
├── journal/        # Crash recovery system
├── common/         # Shared utilities
├── benchmark/      # Performance testing
├── test/          # Unit tests
└── docs/          # Documentation
```

## Features

- Thread-safe file operations
- LRU caching system for improved performance
- Journaling for crash recovery
- Efficient block allocation
- Command-line interface for system interaction
- Comprehensive logging and error handling
- Performance benchmarking tools

## Prerequisites

- C++17 or later
- CMake 3.15+
- GoogleTest (for unit testing)
- A modern C++ compiler (GCC 7+, Clang 6+, or MSVC 2019+)

## Building the Project

```bash
# Create build directory
mkdir build && cd build

# Generate build files
cmake ..

# Build the project
cmake --build .

# Run tests
ctest
```

## Development Guidelines

1. **Code Style**

   - Follow Google C++ Style Guide
   - Use meaningful variable and function names
   - Document public APIs using Doxygen format

2. **Testing**

   - Write unit tests for new features
   - Ensure all tests pass before committing
   - Include performance benchmarks for critical paths

3. **Threading**

   - Use thread-safe data structures from `threading/`
   - Avoid raw mutex usage; use RAII locks
   - Document thread safety guarantees

4. **Error Handling**
   - Use exceptions for exceptional conditions
   - Log errors appropriately using common logger
   - Maintain strong exception guarantees where possible

## Team Workflow

1. **Task Division**

   - File System Core (Person 1)
   - Caching & Storage (Person 2)
   - Threading & CLI (Person 3)

2. **Code Review Process**
   - Create feature branches
   - Submit PRs for review
   - Require at least one approval
   - Ensure CI passes

## License

[MIT License](LICENSE)

## Contributing

See [CONTRIBUTING.md](docs/CONTRIBUTING.md) for detailed guidelines.
