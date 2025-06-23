#include <benchmark/benchmark.h>
#include "fs/compression.hpp"
#include <string>
#include <vector>

static void BM_CompressString(benchmark::State& state) {
    std::string data(1000, 'A'); // 1000 'A's
    for (auto _ : state) {
        auto compressed = mtfs::fs::FileCompression::compress(data);
        benchmark::DoNotOptimize(compressed);
    }
}
BENCHMARK(BM_CompressString);

static void BM_DecompressString(benchmark::State& state) {
    std::string data(1000, 'A');
    auto compressed = mtfs::fs::FileCompression::compress(data);
    for (auto _ : state) {
        auto decompressed = mtfs::fs::FileCompression::decompress(compressed);
        benchmark::DoNotOptimize(decompressed);
    }
}
BENCHMARK(BM_DecompressString);

static void BM_CompressLargeString(benchmark::State& state) {
    std::string data(10 * 1024 * 1024, 'B'); // 10 MB of 'B'
    for (auto _ : state) {
        auto compressed = mtfs::fs::FileCompression::compress(data);
        benchmark::DoNotOptimize(compressed);
    }
}
BENCHMARK(BM_CompressLargeString);

static void BM_DecompressLargeString(benchmark::State& state) {
    std::string data(10 * 1024 * 1024, 'B');
    auto compressed = mtfs::fs::FileCompression::compress(data);
    for (auto _ : state) {
        auto decompressed = mtfs::fs::FileCompression::decompress(compressed);
        benchmark::DoNotOptimize(decompressed);
    }
}
BENCHMARK(BM_DecompressLargeString);
