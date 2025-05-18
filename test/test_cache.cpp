#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include "cache/lru_cache.h"

using namespace mtfs::cache;

TEST(LRUCacheTest, BasicOperations) {
    LRUCache<int, std::string> cache(3);
    
    // Test put and get
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");
    
    std::string value;
    EXPECT_TRUE(cache.get(1, value));
    EXPECT_EQ(value, "one");
    EXPECT_TRUE(cache.get(2, value));
    EXPECT_EQ(value, "two");
    EXPECT_TRUE(cache.get(3, value));
    EXPECT_EQ(value, "three");
    
    // Test size
    EXPECT_EQ(cache.size(), 3);
    
    // Test exists
    EXPECT_TRUE(cache.exists(1));
    EXPECT_FALSE(cache.exists(4));
}

TEST(LRUCacheTest, Eviction) {
    LRUCache<int, std::string> cache(2);
    
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three"); // This should evict 1
    
    std::string value;
    EXPECT_FALSE(cache.get(1, value)); // 1 should be evicted
    EXPECT_TRUE(cache.get(2, value));
    EXPECT_TRUE(cache.get(3, value));
}

TEST(LRUCacheTest, Clear) {
    LRUCache<int, std::string> cache(2);
    
    cache.put(1, "one");
    cache.put(2, "two");
    cache.clear();
    
    EXPECT_EQ(cache.size(), 0);
    EXPECT_FALSE(cache.exists(1));
    EXPECT_FALSE(cache.exists(2));
}

TEST(LRUCacheTest, ThreadSafety) {
    LRUCache<int, int> cache(100);
    std::vector<std::thread> threads;
    
    // Create multiple threads that read and write to the cache
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&cache, i]() {
            for (int j = 0; j < 100; ++j) {
                cache.put(j, i * j);
                int value;
                cache.get(j, value);
            }
        });
    }
    
    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify the cache is still in a valid state
    EXPECT_LE(cache.size(), 100);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 