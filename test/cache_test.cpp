#include <gtest/gtest.h>
#include "cache/lru_cache.hpp"
#include <string>

namespace mtfs {
namespace cache {
namespace test {

TEST(LRUCacheTest, BasicOperations) {
    LRUCache<std::string, int> cache(2);
    
    // Test put and get
    cache.put("key1", 1);
    cache.put("key2", 2);
    
    int value;
    EXPECT_TRUE(cache.get("key1", value));
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(cache.get("key2", value));
    EXPECT_EQ(value, 2);
    
    // Test capacity limit
    cache.put("key3", 3);
    EXPECT_FALSE(cache.get("key1", value)); // key1 should be evicted
    EXPECT_TRUE(cache.get("key2", value));
    EXPECT_EQ(value, 2);
    EXPECT_TRUE(cache.get("key3", value));
    EXPECT_EQ(value, 3);
}

TEST(LRUCacheTest, RemoveAndClear) {
    LRUCache<std::string, int> cache(3);
    
    cache.put("key1", 1);
    cache.put("key2", 2);
    cache.put("key3", 3);
    
    // Test remove
    cache.remove("key2");
    int value;
    EXPECT_FALSE(cache.get("key2", value));
    EXPECT_TRUE(cache.get("key1", value));
    EXPECT_TRUE(cache.get("key3", value));
    
    // Test clear
    cache.clear();
    EXPECT_FALSE(cache.get("key1", value));
    EXPECT_FALSE(cache.get("key3", value));
    EXPECT_EQ(cache.size(), 0);
}

TEST(LRUCacheTest, UpdateExisting) {
    LRUCache<std::string, int> cache(2);
    
    cache.put("key1", 1);
    cache.put("key2", 2);
    cache.put("key1", 10); // Update existing key
    
    int value;
    EXPECT_TRUE(cache.get("key1", value));
    EXPECT_EQ(value, 10);
    EXPECT_TRUE(cache.get("key2", value));
    EXPECT_EQ(value, 2);
}

} // namespace test
} // namespace cache
} // namespace mtfs 