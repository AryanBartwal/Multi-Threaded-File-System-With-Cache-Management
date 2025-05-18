#pragma once

#include <memory>
#include <mutex>
#include <list>
#include <unordered_map>
#include "common/error.hpp"

namespace mtfs::cache {

template<typename Key, typename Value>
class LRUCache {
public:
    explicit LRUCache(size_t capacity);

    // Core operations
    void put(const Key& key, Value value);
    Value get(const Key& key);
    bool contains(const Key& key);
    void remove(const Key& key);
    void clear();

    // Statistics
    size_t size() const;
    size_t capacity() const;
    double hitRate() const;

private:
    struct CacheEntry {
        Key key;
        Value value;
        CacheEntry(const Key& k, const Value& v) : key(k), value(v) {}
    };

    using EntryList = std::list<CacheEntry>;
    using EntryMap = std::unordered_map<Key, typename EntryList::iterator>;

    void evict();

    const size_t maxCapacity;
    EntryList entries;
    EntryMap lookup;
    mutable std::mutex cacheMutex;

    // Statistics
    size_t hits{0};
    size_t misses{0};
};

// Implementation will be in a separate .tpp file
#include "cache/lru_cache.tpp"

} // namespace mtfs::cache 