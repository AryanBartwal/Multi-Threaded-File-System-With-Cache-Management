#include <iostream>
#include <chrono>

int main() {
    std::cout << "=== Simple Benchmark Test ===" << std::endl;
    std::cout << "This is a basic benchmark executable to test the build system." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simple computation
    int sum = 0;
    for (int i = 0; i < 1000000; ++i) {
        sum += i;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Computed sum: " << sum << std::endl;
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;
    std::cout << "Benchmark build system is working!" << std::endl;
    
    return 0;
}
