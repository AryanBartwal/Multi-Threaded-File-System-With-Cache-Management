# Multi-Threaded File System With Cache Management

A high-performance file system implementation with multi-threading support and cache management capabilities.

## Prerequisites

- CMake (version 3.15 or higher)
- C++17 compatible compiler (GCC, Clang, or MSVC)
- Git

## Building the Project

1. Clone the repository:

```bash
git clone <repository-url>
cd Multi-Threaded-File-System-With-Cache-Management
```

2. Create a build directory and navigate to it:

```bash
mkdir build
cd build
```

3. Configure the project with CMake:

```bash
cmake ..
```

4. Build the project:

```bash
cmake --build .
```

## Running the Project

After building, you can find the executables in the `build` directory:

- The main CLI application will be in `build/cli/`
- Test executables will be in `build/test/`

To run the CLI application:

```bash
./cli/mtfs-cli
```

## Project Structure

- `cli/` - Command-line interface implementation
- `fs/` - Core file system implementation
- `cache/` - Cache management system
- `storage/` - Storage layer implementation
- `journal/` - Journaling system
- `common/` - Common utilities and shared code
- `test/` - Test suite
- `threading/` - Threading utilities
- `benchmark/` - Performance benchmarking tools

## Development

The project uses CMake as its build system and follows modern C++ practices. The code is written in C++17.

### Building Tests

Tests are automatically built when you build the project. To run the tests:

```bash
cd build
ctest
```

## License

[Add your license information here]
