#pragma once

#include <list>
#include <unordered_map>
#include <string>
#include <sstream>
#include <windows.h>
#include "common/logger.hpp"

namespace mtfs::cache {

template<typename Key, typename Value>
class LRUCache {
public:
    explicit LRUCache(size_t capacity) : capacity_(capacity) {
        InitializeCriticalSection(&cs_);
    }

    ~LRUCache() {
        DeleteCriticalSection(&cs_);
    }

    void put(const Key& key, const Value& value) {
        EnterCriticalSection(&cs_);
        
        auto it = cache_map_.find(key);
        if (it != cache_map_.end()) {
            // Key exists, update value and move to front
            access_list_.erase(it->second.first);
            access_list_.push_front({key, value});
            it->second = {access_list_.begin(), value};
            LOG_DEBUG("Cache update: " + toString(key));
            LeaveCriticalSection(&cs_);
            return;
        }

        // Check if cache is full
        if (cache_map_.size() >= capacity_) {
            // Remove least recently used item
            const auto& last = access_list_.back();
            cache_map_.erase(last.first);
            access_list_.pop_back();
            LOG_DEBUG("Cache eviction: " + toString(key));
        }

        // Insert new item
        access_list_.push_front({key, value});
        cache_map_[key] = {access_list_.begin(), value};
        LOG_DEBUG("Cache insert: " + toString(key));
        LeaveCriticalSection(&cs_);
    }

    bool get(const Key& key, Value& value) {
        EnterCriticalSection(&cs_);
        
        auto it = cache_map_.find(key);
        if (it == cache_map_.end()) {
            LOG_DEBUG("Cache miss: " + toString(key));
            LeaveCriticalSection(&cs_);
            return false;
        }

        // Move accessed item to front
        access_list_.erase(it->second.first);
        access_list_.push_front({key, it->second.second});
        it->second.first = access_list_.begin();
        value = it->second.second;
        
        LOG_DEBUG("Cache hit: " + toString(key));
        LeaveCriticalSection(&cs_);
        return true;
    }

    bool exists(const Key& key) const {
        EnterCriticalSection(&cs_);
        bool result = cache_map_.find(key) != cache_map_.end();
        LeaveCriticalSection(&cs_);
        return result;
    }

    void clear() {
        EnterCriticalSection(&cs_);
        cache_map_.clear();
        access_list_.clear();
        LOG_DEBUG("Cache cleared");
        LeaveCriticalSection(&cs_);
    }

    size_t size() const {
        EnterCriticalSection(&cs_);
        size_t result = cache_map_.size();
        LeaveCriticalSection(&cs_);
        return result;
    }

private:
    const size_t capacity_;
    mutable CRITICAL_SECTION cs_;
    std::list<std::pair<Key, Value>> access_list_;
    std::unordered_map<Key, std::pair<typename std::list<std::pair<Key, Value>>::iterator, Value>> cache_map_;

    // Helper function to convert key to string for logging
    std::string toString(const Key& key) const {
        std::ostringstream oss;
        oss << key;
        return oss.str();
    }
};

} // namespace mtfs::cache 