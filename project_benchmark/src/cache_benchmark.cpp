#include <benchmark/benchmark.h>
#include "cache/lru_cache.hpp"
#include <string>
#include <vector>
#include <random>

static void BM_LRUCachePut(benchmark::State& state) {
    mtfs::cache::LRUCache<std::string, std::string> cache(100);
    for (auto _ : state) {
        cache.put("key", "value");
    }
}
BENCHMARK(BM_LRUCachePut);

static void BM_LRUCacheGet(benchmark::State& state) {
    mtfs::cache::LRUCache<std::string, std::string> cache(100);
    cache.put("key", "value");
    std::string value;
    for (auto _ : state) {
        cache.get("key", value);
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(BM_LRUCacheGet);

// Advanced/Large-scale: Put many unique keys (large cache stress)
static void BM_LRUCachePutLarge(benchmark::State& state) {
    size_t cache_size = 100000;
    mtfs::cache::LRUCache<std::string, std::string> cache(cache_size);
    for (auto _ : state) {
        for (size_t i = 0; i < cache_size; ++i) {
            cache.put("key" + std::to_string(i), "value" + std::to_string(i));
        }
    }
}
BENCHMARK(BM_LRUCachePutLarge);

// Advanced: Random get/put operations
static void BM_LRUCacheRandomAccess(benchmark::State& state) {
    size_t cache_size = 10000;
    mtfs::cache::LRUCache<std::string, std::string> cache(cache_size);
    std::vector<std::string> keys;
    for (size_t i = 0; i < cache_size; ++i) {
        std::string k = "key" + std::to_string(i);
        cache.put(k, "value" + std::to_string(i));
        keys.push_back(k);
    }
    std::mt19937 rng(42);
    std::uniform_int_distribution<size_t> dist(0, cache_size - 1);
    std::string value;
    for (auto _ : state) {
        size_t idx = dist(rng);
        cache.get(keys[idx], value);
        benchmark::DoNotOptimize(value);
        if (idx % 10 == 0) {
            cache.put("key" + std::to_string(idx + cache_size), "value" + std::to_string(idx + cache_size));
        }
    }
}
BENCHMARK(BM_LRUCacheRandomAccess);

// Advanced: Eviction stress (cycle through more keys than cache size)
static void BM_LRUCacheEviction(benchmark::State& state) {
    size_t cache_size = 1000;
    size_t total_keys = 10000;
    mtfs::cache::LRUCache<std::string, std::string> cache(cache_size);
    for (auto _ : state) {
        for (size_t i = 0; i < total_keys; ++i) {
            cache.put("key" + std::to_string(i), "value" + std::to_string(i));
        }
    }
}
BENCHMARK(BM_LRUCacheEviction);
