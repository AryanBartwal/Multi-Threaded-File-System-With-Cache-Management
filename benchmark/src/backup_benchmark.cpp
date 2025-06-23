#include <chrono>
#include <string>
#include <iostream>
#include <functional>
#include <vector>
#include <filesystem>
#include <fstream>
#include <random>

// Simple benchmark timing utility
class SimpleBenchmark {
public:
    static void benchmark(const std::string& name, std::function<void()> func, int iterations = 1) {
        std::cout << "Running " << name << " (" << iterations << " iterations)..." << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        double avg_ms = static_cast<double>(duration.count()) / iterations;
        
        std::cout << "  Total time: " << duration.count() << " ms" << std::endl;
        std::cout << "  Average time per iteration: " << avg_ms << " ms" << std::endl;
        std::cout << std::endl;
    }
};

// Generate random data for testing
std::string generate_random_data(size_t size) {
    static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.reserve(size);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);
    
    for (size_t i = 0; i < size; ++i) {
        result += charset[dis(gen)];
    }
    return result;
}

// Setup test files and directories
void setup_test_environment() {
    namespace fs = std::filesystem;
    
    // Create test directories
    fs::create_directories("test_backup_source");
    fs::create_directories("test_backup_source/subdir1");
    fs::create_directories("test_backup_source/subdir2");
    
    // Create test files
    std::vector<std::pair<std::string, size_t>> test_files = {
        {"test_backup_source/file1.txt", 1024},
        {"test_backup_source/file2.txt", 2048},
        {"test_backup_source/subdir1/file3.txt", 512},
        {"test_backup_source/subdir2/file4.txt", 4096},
        {"test_backup_source/large_file.txt", 50000}
    };
    
    for (const auto& [filename, size] : test_files) {
        std::ofstream ofs(filename);
        ofs << generate_random_data(size);
        ofs.close();
    }
}

void cleanup_test_environment() {
    namespace fs = std::filesystem;
    
    if (fs::exists("test_backup_source")) {
        fs::remove_all("test_backup_source");
    }
    if (fs::exists("test_backup_dest_std")) {
        fs::remove_all("test_backup_dest_std");
    }
    if (fs::exists("test_backup_dest_custom")) {
        fs::remove_all("test_backup_dest_custom");
    }
}

// Baseline benchmarks using std::filesystem
void benchmark_std_file_copy() {
    namespace fs = std::filesystem;
    
    SimpleBenchmark::benchmark("Standard File Copy (std::filesystem)", []() {
        fs::copy_file("test_backup_source/file1.txt", "temp_copy.txt", 
                     fs::copy_options::overwrite_existing);
        fs::remove("temp_copy.txt");
    }, 50);
}

void benchmark_std_directory_copy() {
    namespace fs = std::filesystem;
    
    SimpleBenchmark::benchmark("Standard Directory Copy (std::filesystem)", []() {
        if (fs::exists("test_backup_dest_std")) {
            fs::remove_all("test_backup_dest_std");
        }
        fs::copy("test_backup_source", "test_backup_dest_std", 
                fs::copy_options::recursive);
        fs::remove_all("test_backup_dest_std");
    }, 10);
}

// Custom backup implementation (simplified)
class SimpleBackupManager {
public:
    static void copy_file(const std::string& source, const std::string& dest) {
        std::ifstream src(source, std::ios::binary);
        std::ofstream dst(dest, std::ios::binary);
        dst << src.rdbuf();
    }
    
    static void copy_directory(const std::string& source, const std::string& dest) {
        namespace fs = std::filesystem;
        
        if (!fs::exists(dest)) {
            fs::create_directories(dest);
        }
        
        for (const auto& entry : fs::recursive_directory_iterator(source)) {
            const auto& path = entry.path();
            auto relative_path = fs::relative(path, source);
            auto dest_path = fs::path(dest) / relative_path;
            
            if (entry.is_directory()) {
                fs::create_directories(dest_path);
            } else if (entry.is_regular_file()) {
                copy_file(path.string(), dest_path.string());
            }
        }
    }
    
    static void incremental_backup(const std::string& source, const std::string& dest) {
        namespace fs = std::filesystem;
        
        if (!fs::exists(dest)) {
            copy_directory(source, dest);
            return;
        }
        
        for (const auto& entry : fs::recursive_directory_iterator(source)) {
            if (entry.is_regular_file()) {
                const auto& source_path = entry.path();
                auto relative_path = fs::relative(source_path, source);
                auto dest_path = fs::path(dest) / relative_path;
                
                bool should_copy = false;
                if (!fs::exists(dest_path)) {
                    should_copy = true;
                } else {
                    auto source_time = fs::last_write_time(source_path);
                    auto dest_time = fs::last_write_time(dest_path);
                    should_copy = (source_time > dest_time);
                }
                
                if (should_copy) {
                    fs::create_directories(dest_path.parent_path());
                    copy_file(source_path.string(), dest_path.string());
                }
            }
        }
    }
};

// Custom backup benchmarks
void benchmark_custom_file_copy() {
    SimpleBenchmark::benchmark("Custom File Copy", []() {
        SimpleBackupManager::copy_file("test_backup_source/file1.txt", "temp_custom_copy.txt");
        std::filesystem::remove("temp_custom_copy.txt");
    }, 50);
}

void benchmark_custom_directory_copy() {
    SimpleBenchmark::benchmark("Custom Directory Copy", []() {
        if (std::filesystem::exists("test_backup_dest_custom")) {
            std::filesystem::remove_all("test_backup_dest_custom");
        }
        SimpleBackupManager::copy_directory("test_backup_source", "test_backup_dest_custom");
        std::filesystem::remove_all("test_backup_dest_custom");
    }, 10);
}

void benchmark_incremental_backup() {
    SimpleBenchmark::benchmark("Incremental Backup (first run)", []() {
        if (std::filesystem::exists("test_incremental_dest")) {
            std::filesystem::remove_all("test_incremental_dest");
        }
        SimpleBackupManager::incremental_backup("test_backup_source", "test_incremental_dest");
    }, 5);
    
    // Modify a file to test incremental functionality
    std::ofstream ofs("test_backup_source/file1.txt", std::ios::app);
    ofs << "\nModified content";
    ofs.close();
    
    SimpleBenchmark::benchmark("Incremental Backup (update run)", []() {
        SimpleBackupManager::incremental_backup("test_backup_source", "test_incremental_dest");
    }, 5);
    
    std::filesystem::remove_all("test_incremental_dest");
}

// Large-scale backup benchmarks
void benchmark_large_scale_backup() {
    // Create a larger test environment
    std::filesystem::create_directories("large_backup_source");
    
    // Create multiple files and directories
    for (int dir = 0; dir < 5; ++dir) {
        std::string dir_path = "large_backup_source/dir" + std::to_string(dir);
        std::filesystem::create_directories(dir_path);
        
        for (int file = 0; file < 10; ++file) {
            std::string file_path = dir_path + "/file" + std::to_string(file) + ".txt";
            std::ofstream ofs(file_path);
            ofs << generate_random_data(1024 * (file + 1)); // Variable size files
            ofs.close();
        }
    }
    
    SimpleBenchmark::benchmark("Large Scale Directory Backup (50 files)", []() {
        if (std::filesystem::exists("large_backup_dest")) {
            std::filesystem::remove_all("large_backup_dest");
        }
        SimpleBackupManager::copy_directory("large_backup_source", "large_backup_dest");
        std::filesystem::remove_all("large_backup_dest");
    }, 3);
    
    std::filesystem::remove_all("large_backup_source");
}

// Backup verification benchmark
void benchmark_backup_verification() {
    SimpleBenchmark::benchmark("Backup Verification", []() {
        // Create backup
        SimpleBackupManager::copy_directory("test_backup_source", "verify_backup_dest");
        
        // Verify backup by comparing files
        namespace fs = std::filesystem;
        bool verification_passed = true;
        
        for (const auto& entry : fs::recursive_directory_iterator("test_backup_source")) {
            if (entry.is_regular_file()) {
                auto relative_path = fs::relative(entry.path(), "test_backup_source");
                auto backup_path = fs::path("verify_backup_dest") / relative_path;
                
                if (!fs::exists(backup_path)) {
                    verification_passed = false;
                    break;
                }
                
                // Simple size comparison
                if (fs::file_size(entry.path()) != fs::file_size(backup_path)) {
                    verification_passed = false;
                    break;
                }
            }
        }
        
        if (!verification_passed) {
            std::cout << "Backup verification failed!" << std::endl;
        }
        
        fs::remove_all("verify_backup_dest");
    }, 10);
}

void run_backup_benchmarks() {
    std::cout << "=== Backup Benchmarks ===" << std::endl;
    
    // Setup test environment
    cleanup_test_environment();
    setup_test_environment();
    
    // Baseline benchmarks
    std::cout << "\n--- Baseline Benchmarks (std::filesystem) ---" << std::endl;
    benchmark_std_file_copy();
    benchmark_std_directory_copy();
    
    // Custom backup benchmarks
    std::cout << "\n--- Custom Backup Benchmarks ---" << std::endl;
    benchmark_custom_file_copy();
    benchmark_custom_directory_copy();
    benchmark_incremental_backup();
    
    // Advanced benchmarks
    std::cout << "\n--- Advanced Backup Benchmarks ---" << std::endl;
    benchmark_large_scale_backup();
    benchmark_backup_verification();
    
    // Cleanup
    cleanup_test_environment();
    
    std::cout << "Backup benchmarks completed." << std::endl;
}
