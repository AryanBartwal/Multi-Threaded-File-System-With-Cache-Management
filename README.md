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

## CLI Feature Demo

Here’s a full demo set of CLI commands for your multi-threaded file system, with explanations for each. You can copy and paste these into your CLI to see how each feature works.

### 1. File Operations

```
create-file demo.txt
write-file demo.txt "Hello, world!"
read-file demo.txt
file-info demo.txt
```

Creates a file, writes text, reads it, and shows file info.

### 2. Directory Operations

```
create-dir demodir
list-dir .
list-dir demodir
```

Creates a directory and lists contents of current and new directory.

### 3. Copy, Move, Rename, Find, Delete

```
copy-file demo.txt demo_copy.txt
move-file demo_copy.txt demodir/demo_moved.txt
rename-file demo.txt demo_renamed.txt
find-file demo*
delete-file demodir/demo_moved.txt
delete-file demo_renamed.txt
```

Copies, moves, renames, finds, and deletes files.

### 4. Compression

```
create-file compressme.txt
write-file compressme.txt "Compress this file!"
compress-file compressme.txt
compression-stats
decompress-file compressme.txt
delete-file compressme.txt
```

Compresses and decompresses a file, shows compression stats.

### 5. Backup Management

```
create-backup mybackup
list-backups
backup-dashboard
restore-backup mybackup demodir
delete-backup mybackup
```

Creates, lists, restores, and deletes backups.

### 6. Cache Management

```
set-cache-policy LRU
get-cache-policy
resize-cache 10
pin-file demo_renamed.txt
unpin-file demo_renamed.txt
prefetch-file demo_renamed.txt
cache-analytics
hot-files 3
```

Sets cache policy, resizes cache, pins/unpins files, prefetches, shows analytics and hot files.

### 7. Stats

```
show-stats
reset-stats
```

Shows and resets file system statistics.

### 8. Exit

```
exit
```

Exits the CLI.

Let me know if you want a script for PowerShell or want to see the output for any specific command!

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

## Benchmarking

The project includes a comprehensive benchmarking suite to measure the performance of the file system, cache, compression, and backup features.

### Building Benchmarks

Benchmarks are built automatically with the rest of the project. After building, the benchmark executables will be located in `build/benchmark/`.

### Running Benchmarks

To run all benchmarks:

```bash
cd build/benchmark
./benchmark_main
```

This will execute all registered benchmarks and print performance results for each operation (e.g., cache put/get, compression, backup, etc.).

#### Example Output

```
BM_LRUCachePut           1000000  1234 ns/op
BM_LRUCacheGet           1000000  567 ns/op
BM_LRUCachePutLarge         1000  1.23 ms/op
BM_LRUCacheRandomAccess     1000  2.34 ms/op
BM_LRUCacheEviction         1000  3.45 ms/op
BM_CompressString        1000000  890 ns/op
BM_CompressLargeString      1000  4.56 ms/op
BM_DecompressString      1000000  910 ns/op
BM_DecompressLargeString    1000  5.67 ms/op
BM_CreateBackup             1000  6.78 ms/op
BM_RestoreBackup            1000  7.89 ms/op
```

### Customizing Benchmarks

You can run a specific benchmark or change the number of iterations using Google Benchmark command-line options. For example:

```bash
./benchmark_main --benchmark_filter=BM_LRUCachePut
```

For more options, see the [Google Benchmark documentation](https://github.com/google/benchmark).

### Interpreting Results

- **ns/op**: Nanoseconds per operation (lower is better)
- **ms/op**: Milliseconds per operation
- Each benchmark tests a specific feature (see the benchmark name for details)

Benchmarks help you compare performance before and after code changes, or under different configurations.

## License

[Add your license information here]
