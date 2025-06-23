#include <benchmark/benchmark.h>
#include <fstream>
#include <string>

// Simple benchmark: write and read a file
static void BM_FileWrite(benchmark::State& state) {
    for (auto _ : state) {
        std::ofstream ofs("bm_test.txt");
        ofs << "Hello, benchmark!";
        ofs.close();
    }
}
BENCHMARK(BM_FileWrite);

static void BM_FileRead(benchmark::State& state) {
    for (auto _ : state) {
        std::ifstream ifs("bm_test.txt");
        std::string content;
        std::getline(ifs, content);
        benchmark::DoNotOptimize(content);
    }
}
BENCHMARK(BM_FileRead);
