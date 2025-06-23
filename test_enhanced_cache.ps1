#!/bin/bash
# Enhanced Cache System Test Script

echo "=========================================="
echo "Enhanced Cache System Test"
echo "=========================================="

# Test 1: Basic cache operations with different policies
echo "=== Test 1: Cache Policy Testing ==="
echo "create-file test1.txt
write-file test1.txt Hello World
read-file test1.txt
get-cache-policy
set-cache-policy LFU
get-cache-policy
cache-analytics

=== Test 2: LFU Policy Testing ===
create-file test2.txt
write-file test2.txt Content 2
read-file test1.txt
read-file test1.txt
read-file test2.txt
cache-analytics

=== Test 3: FIFO Policy Testing ===
set-cache-policy FIFO
create-file test3.txt
write-file test3.txt Content 3
read-file test3.txt
cache-analytics

=== Test 4: Pinning and Prefetching ===
set-cache-policy LRU
pin-file test1.txt
prefetch-file test2.txt
cache-analytics
resize-cache 2
create-file test4.txt
write-file test4.txt Content 4
read-file test4.txt
cache-analytics

=== Test 5: Hot Files and Analytics ===
hot-files 5
show-stats

exit" | ../../../build/cli/Release/mtfs_cli.exe
