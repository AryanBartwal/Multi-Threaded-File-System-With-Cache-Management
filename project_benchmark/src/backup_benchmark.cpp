#include <benchmark/benchmark.h>
#include "fs/backup_manager.hpp"
#include <filesystem>
#include <fstream>
#include <string>

static void BM_CreateBackup(benchmark::State& state) {
    // Setup: create a test directory with a file
    std::filesystem::create_directory("bm_backup_testdir");
    std::ofstream("bm_backup_testdir/file.txt") << "Backup benchmark test";
    mtfs::fs::BackupManager manager("bm_backup_backups");
    for (auto _ : state) {
        manager.createBackup("bm_bench_backup", "bm_backup_testdir");
        std::filesystem::remove_all("bm_backup_backups/bm_bench_backup");
        std::filesystem::remove("bm_backup_backups/bm_bench_backup_metadata.txt");
    }
    std::filesystem::remove_all("bm_backup_testdir");
    std::filesystem::remove_all("bm_backup_backups");
}
BENCHMARK(BM_CreateBackup);

static void BM_RestoreBackup(benchmark::State& state) {
    // Setup: create a test directory and backup
    std::filesystem::create_directory("bm_backup_testdir");
    std::ofstream("bm_backup_testdir/file.txt") << "Backup benchmark test";
    mtfs::fs::BackupManager manager("bm_backup_backups");
    manager.createBackup("bm_bench_backup", "bm_backup_testdir");
    for (auto _ : state) {
        manager.restoreBackup("bm_bench_backup", "bm_backup_restored");
        std::filesystem::remove_all("bm_backup_restored");
    }
    std::filesystem::remove_all("bm_backup_testdir");
    std::filesystem::remove_all("bm_backup_backups");
}
BENCHMARK(BM_RestoreBackup);
