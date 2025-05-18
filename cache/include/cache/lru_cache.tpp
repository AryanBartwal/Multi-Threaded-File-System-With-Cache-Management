#pragma once

namespace mtfs {
namespace cache {

template<typename K, typename V>
LRUCache<K, V>::LRUCache(size_t capacity) : capacity_(capacity) {}

template<typename K, typename V>
bool LRUCache<K, V>::get(const K& key, V& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_map_.find(key);
    if (it == cache_map_.end()) {
        return false;
    }
    
    // Move the accessed item to the front of the list
    cache_list_.splice(cache_list_.begin(), cache_list_, it->second.first);
    value = it->second.second;
    return true;
}

template<typename K, typename V>
void LRUCache<K, V>::put(const K& key, const V& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_map_.find(key);
    
    if (it != cache_map_.end()) {
        // Update existing entry
        cache_list_.erase(it->second.first);
        cache_map_.erase(it);
    } else if (cache_map_.size() >= capacity_) {
        // Remove least recently used item
        const K& lru_key = cache_list_.back();
        cache_map_.erase(lru_key);
        cache_list_.pop_back();
    }
    
    // Insert new item at the front
    cache_list_.push_front(key);
    cache_map_[key] = {cache_list_.begin(), value};
}

template<typename K, typename V>
void LRUCache<K, V>::remove(const K& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_map_.find(key);
    if (it != cache_map_.end()) {
        cache_list_.erase(it->second.first);
        cache_map_.erase(it);
    }
}

template<typename K, typename V>
void LRUCache<K, V>::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_map_.clear();
    cache_list_.clear();
}

template<typename K, typename V>
size_t LRUCache<K, V>::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return cache_map_.size();
}

} // namespace cache
} // namespace mtfs 