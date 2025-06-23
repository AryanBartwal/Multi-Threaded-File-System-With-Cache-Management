#pragma once

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace mtfs::cache {

// ===== EnhancedLRUCache Implementation =====

template<typename Key, typename Value>
EnhancedLRUCache<Key, Value>::EnhancedLRUCache(size_t capacity) 
    : maxCapacity(capacity) {}

template<typename Key, typename Value>
void EnhancedLRUCache<Key, Value>::put(const Key& key, const Value& value) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    auto it = lookup.find(key);
    if (it != lookup.end()) {
        // Update existing entry
        it->second->value = value;
        it->second->lastAccessed = std::chrono::system_clock::now();
        moveToFront(it->second);
        return;
    }
    
    // Add new entry
    if (entries.size() >= maxCapacity) {
        evict();
    }
    
    entries.emplace_front(key, value);
    lookup[key] = entries.begin();
}

template<typename Key, typename Value>
Value EnhancedLRUCache<Key, Value>::get(const Key& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    auto it = lookup.find(key);
    if (it == lookup.end()) {
        stats.misses++;
        stats.updateHitRate();
        throw std::runtime_error("Key not found in cache");
    }
    
    // Update access info
    it->second->accessCount++;
    it->second->lastAccessed = std::chrono::system_clock::now();
    moveToFront(it->second);
    
    stats.hits++;
    stats.updateHitRate();
    return it->second->value;
}

template<typename Key, typename Value>
bool EnhancedLRUCache<Key, Value>::contains(const Key& key) const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    return lookup.find(key) != lookup.end();
}

template<typename Key, typename Value>
void EnhancedLRUCache<Key, Value>::remove(const Key& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    auto it = lookup.find(key);
    if (it != lookup.end()) {
        entries.erase(it->second);
        lookup.erase(it);
        pinnedKeys.erase(key);
    }
}

template<typename Key, typename Value>
void EnhancedLRUCache<Key, Value>::clear() {
    std::lock_guard<std::mutex> lock(cacheMutex);
    entries.clear();
    lookup.clear();
    pinnedKeys.clear();
}

template<typename Key, typename Value>
size_t EnhancedLRUCache<Key, Value>::size() const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    return entries.size();
}

template<typename Key, typename Value>
size_t EnhancedLRUCache<Key, Value>::capacity() const {
    return maxCapacity;
}

template<typename Key, typename Value>
CacheStatistics EnhancedLRUCache<Key, Value>::getStatistics() const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    auto statsCopy = stats;
    statsCopy.pinnedItems = pinnedKeys.size();
    return statsCopy;
}

template<typename Key, typename Value>
void EnhancedLRUCache<Key, Value>::resetStatistics() {
    std::lock_guard<std::mutex> lock(cacheMutex);
    stats = CacheStatistics();
}

template<typename Key, typename Value>
void EnhancedLRUCache<Key, Value>::pin(const Key& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    if (lookup.find(key) != lookup.end()) {
        pinnedKeys.insert(key);
    }
}

template<typename Key, typename Value>
void EnhancedLRUCache<Key, Value>::unpin(const Key& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    pinnedKeys.erase(key);
}

template<typename Key, typename Value>
bool EnhancedLRUCache<Key, Value>::isPinned(const Key& key) const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    return pinnedKeys.find(key) != pinnedKeys.end();
}

template<typename Key, typename Value>
void EnhancedLRUCache<Key, Value>::prefetch(const Key& key, const Value& value) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    if (lookup.find(key) == lookup.end() && entries.size() < maxCapacity) {
        entries.emplace_front(key, value);
        lookup[key] = entries.begin();
        stats.prefetchedItems++;
    }
}

template<typename Key, typename Value>
std::vector<Key> EnhancedLRUCache<Key, Value>::getKeys() const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    std::vector<Key> keys;
    for (const auto& entry : entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

template<typename Key, typename Value>
void EnhancedLRUCache<Key, Value>::evict() {
    while (!entries.empty()) {
        auto lastIt = std::prev(entries.end());
        if (pinnedKeys.find(lastIt->key) == pinnedKeys.end()) {
            lookup.erase(lastIt->key);
            entries.erase(lastIt);
            stats.evictions++;
            break;
        }
        
        // Move pinned item to front and try next
        moveToFront(lastIt);
        if (entries.size() <= 1) break; // Prevent infinite loop
    }
}

template<typename Key, typename Value>
void EnhancedLRUCache<Key, Value>::moveToFront(typename EntryList::iterator it) {
    if (it != entries.begin()) {
        entries.splice(entries.begin(), entries, it);
    }
}

// ===== LFUCache Implementation =====

template<typename Key, typename Value>
LFUCache<Key, Value>::LFUCache(size_t capacity) : maxCapacity(capacity) {}

template<typename Key, typename Value>
void LFUCache<Key, Value>::put(const Key& key, const Value& value) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    if (maxCapacity == 0) return;
    
    auto it = keyToEntry.find(key);
    if (it != keyToEntry.end()) {
        // Update existing entry
        it->second.value = value;
        it->second.lastAccessed = std::chrono::system_clock::now();
        updateFrequency(key);
        return;
    }
    
    // Add new entry
    if (keyToEntry.size() >= maxCapacity) {
        evict();
    }
    
    keyToEntry[key] = EntryType(key, value);
    keyToFreq[key] = 1;
    frequencies[1].push_back(key);
    minFrequency = 1;
}

template<typename Key, typename Value>
Value LFUCache<Key, Value>::get(const Key& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    auto it = keyToEntry.find(key);
    if (it == keyToEntry.end()) {
        stats.misses++;
        stats.updateHitRate();
        throw std::runtime_error("Key not found in cache");
    }
    
    // Update access info
    it->second.accessCount++;
    it->second.lastAccessed = std::chrono::system_clock::now();
    updateFrequency(key);
    
    stats.hits++;
    stats.updateHitRate();
    return it->second.value;
}

template<typename Key, typename Value>
bool LFUCache<Key, Value>::contains(const Key& key) const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    return keyToEntry.find(key) != keyToEntry.end();
}

template<typename Key, typename Value>
void LFUCache<Key, Value>::remove(const Key& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    auto it = keyToEntry.find(key);
    if (it != keyToEntry.end()) {
        size_t freq = keyToFreq[key];
        auto& freqList = frequencies[freq];
        freqList.remove(key);
        if (freqList.empty() && freq == minFrequency) {
            minFrequency++;
        }
        
        keyToEntry.erase(key);
        keyToFreq.erase(key);
        pinnedKeys.erase(key);
    }
}

template<typename Key, typename Value>
void LFUCache<Key, Value>::clear() {
    std::lock_guard<std::mutex> lock(cacheMutex);
    keyToEntry.clear();
    keyToFreq.clear();
    frequencies.clear();
    pinnedKeys.clear();
    minFrequency = 1;
}

template<typename Key, typename Value>
size_t LFUCache<Key, Value>::size() const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    return keyToEntry.size();
}

template<typename Key, typename Value>
size_t LFUCache<Key, Value>::capacity() const {
    return maxCapacity;
}

template<typename Key, typename Value>
CacheStatistics LFUCache<Key, Value>::getStatistics() const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    auto statsCopy = stats;
    statsCopy.pinnedItems = pinnedKeys.size();
    return statsCopy;
}

template<typename Key, typename Value>
void LFUCache<Key, Value>::resetStatistics() {
    std::lock_guard<std::mutex> lock(cacheMutex);
    stats = CacheStatistics();
}

template<typename Key, typename Value>
void LFUCache<Key, Value>::pin(const Key& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    if (keyToEntry.find(key) != keyToEntry.end()) {
        pinnedKeys.insert(key);
    }
}

template<typename Key, typename Value>
void LFUCache<Key, Value>::unpin(const Key& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    pinnedKeys.erase(key);
}

template<typename Key, typename Value>
bool LFUCache<Key, Value>::isPinned(const Key& key) const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    return pinnedKeys.find(key) != pinnedKeys.end();
}

template<typename Key, typename Value>
void LFUCache<Key, Value>::prefetch(const Key& key, const Value& value) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    if (keyToEntry.find(key) == keyToEntry.end() && keyToEntry.size() < maxCapacity) {
        keyToEntry[key] = EntryType(key, value);
        keyToFreq[key] = 1;
        frequencies[1].push_back(key);
        minFrequency = 1;
        stats.prefetchedItems++;
    }
}

template<typename Key, typename Value>
std::vector<Key> LFUCache<Key, Value>::getKeys() const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    std::vector<Key> keys;
    for (const auto& pair : keyToEntry) {
        keys.push_back(pair.first);
    }
    return keys;
}

template<typename Key, typename Value>
void LFUCache<Key, Value>::evict() {
    // Find least frequent unpinned key
    for (auto freqIt = frequencies.find(minFrequency); 
         freqIt != frequencies.end(); ++freqIt) {
        auto& keyList = freqIt->second;
        
        for (auto keyIt = keyList.begin(); keyIt != keyList.end(); ++keyIt) {
            if (pinnedKeys.find(*keyIt) == pinnedKeys.end()) {
                Key keyToRemove = *keyIt;
                keyList.erase(keyIt);
                keyToEntry.erase(keyToRemove);
                keyToFreq.erase(keyToRemove);
                stats.evictions++;
                
                if (keyList.empty() && freqIt->first == minFrequency) {
                    minFrequency++;
                }
                return;
            }
        }
    }
}

template<typename Key, typename Value>
void LFUCache<Key, Value>::updateFrequency(const Key& key) {
    size_t oldFreq = keyToFreq[key];
    size_t newFreq = oldFreq + 1;
    
    // Remove from old frequency list
    frequencies[oldFreq].remove(key);
    if (frequencies[oldFreq].empty() && oldFreq == minFrequency) {
        minFrequency++;
    }
    
    // Add to new frequency list
    keyToFreq[key] = newFreq;
    frequencies[newFreq].push_back(key);
}

// ===== FIFOCache Implementation =====

template<typename Key, typename Value>
FIFOCache<Key, Value>::FIFOCache(size_t capacity) : maxCapacity(capacity) {}

template<typename Key, typename Value>
void FIFOCache<Key, Value>::put(const Key& key, const Value& value) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    auto it = entries.find(key);
    if (it != entries.end()) {
        // Update existing entry
        it->second.value = value;
        it->second.lastAccessed = std::chrono::system_clock::now();
        return;
    }
    
    // Add new entry
    if (entries.size() >= maxCapacity) {
        evict();
    }
    
    entries[key] = EntryType(key, value);
    insertionOrder.push(key);
}

template<typename Key, typename Value>
Value FIFOCache<Key, Value>::get(const Key& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    auto it = entries.find(key);
    if (it == entries.end()) {
        stats.misses++;
        stats.updateHitRate();
        throw std::runtime_error("Key not found in cache");
    }
    
    // Update access info
    it->second.accessCount++;
    it->second.lastAccessed = std::chrono::system_clock::now();
    
    stats.hits++;
    stats.updateHitRate();
    return it->second.value;
}

template<typename Key, typename Value>
bool FIFOCache<Key, Value>::contains(const Key& key) const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    return entries.find(key) != entries.end();
}

template<typename Key, typename Value>
void FIFOCache<Key, Value>::remove(const Key& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    auto it = entries.find(key);
    if (it != entries.end()) {
        entries.erase(it);
        pinnedKeys.erase(key);
        // Note: Can't efficiently remove from queue, will be handled in evict
    }
}

template<typename Key, typename Value>
void FIFOCache<Key, Value>::clear() {
    std::lock_guard<std::mutex> lock(cacheMutex);
    entries.clear();
    pinnedKeys.clear();
    insertionOrder = EntryQueue(); // Clear queue
}

template<typename Key, typename Value>
size_t FIFOCache<Key, Value>::size() const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    return entries.size();
}

template<typename Key, typename Value>
size_t FIFOCache<Key, Value>::capacity() const {
    return maxCapacity;
}

template<typename Key, typename Value>
CacheStatistics FIFOCache<Key, Value>::getStatistics() const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    auto statsCopy = stats;
    statsCopy.pinnedItems = pinnedKeys.size();
    return statsCopy;
}

template<typename Key, typename Value>
void FIFOCache<Key, Value>::resetStatistics() {
    std::lock_guard<std::mutex> lock(cacheMutex);
    stats = CacheStatistics();
}

template<typename Key, typename Value>
void FIFOCache<Key, Value>::pin(const Key& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    if (entries.find(key) != entries.end()) {
        pinnedKeys.insert(key);
    }
}

template<typename Key, typename Value>
void FIFOCache<Key, Value>::unpin(const Key& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    pinnedKeys.erase(key);
}

template<typename Key, typename Value>
bool FIFOCache<Key, Value>::isPinned(const Key& key) const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    return pinnedKeys.find(key) != pinnedKeys.end();
}

template<typename Key, typename Value>
void FIFOCache<Key, Value>::prefetch(const Key& key, const Value& value) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    if (entries.find(key) == entries.end() && entries.size() < maxCapacity) {
        entries[key] = EntryType(key, value);
        insertionOrder.push(key);
        stats.prefetchedItems++;
    }
}

template<typename Key, typename Value>
std::vector<Key> FIFOCache<Key, Value>::getKeys() const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    std::vector<Key> keys;
    for (const auto& pair : entries) {
        keys.push_back(pair.first);
    }
    return keys;
}

template<typename Key, typename Value>
void FIFOCache<Key, Value>::evict() {
    while (!insertionOrder.empty()) {
        Key oldestKey = insertionOrder.front();
        insertionOrder.pop();
        
        // Check if key still exists and is not pinned
        auto it = entries.find(oldestKey);
        if (it != entries.end() && pinnedKeys.find(oldestKey) == pinnedKeys.end()) {
            entries.erase(it);
            stats.evictions++;
            break;
        }
    }
}

// ===== CacheManager Implementation =====

template<typename Key, typename Value>
CacheManager<Key, Value>::CacheManager(size_t capacity, CachePolicy policy) 
    : cacheCapacity(capacity), currentPolicy(policy) {
    recreateCache();
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::put(const Key& key, const Value& value) {
    std::lock_guard<std::mutex> lock(managerMutex);
    cache->put(key, value);
}

template<typename Key, typename Value>
Value CacheManager<Key, Value>::get(const Key& key) {
    std::lock_guard<std::mutex> lock(managerMutex);
    return cache->get(key);
}

template<typename Key, typename Value>
bool CacheManager<Key, Value>::contains(const Key& key) const {
    std::lock_guard<std::mutex> lock(managerMutex);
    return cache->contains(key);
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::remove(const Key& key) {
    std::lock_guard<std::mutex> lock(managerMutex);
    cache->remove(key);
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::clear() {
    std::lock_guard<std::mutex> lock(managerMutex);
    cache->clear();
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::setPolicy(CachePolicy policy) {
    std::lock_guard<std::mutex> lock(managerMutex);
    if (currentPolicy != policy) {
        currentPolicy = policy;
        recreateCache();
    }
}

template<typename Key, typename Value>
CachePolicy CacheManager<Key, Value>::getPolicy() const {
    std::lock_guard<std::mutex> lock(managerMutex);
    return currentPolicy;
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::resize(size_t newCapacity) {
    std::lock_guard<std::mutex> lock(managerMutex);
    if (cacheCapacity != newCapacity) {
        cacheCapacity = newCapacity;
        recreateCache();
    }
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::pin(const Key& key) {
    std::lock_guard<std::mutex> lock(managerMutex);
    cache->pin(key);
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::unpin(const Key& key) {
    std::lock_guard<std::mutex> lock(managerMutex);
    cache->unpin(key);
}

template<typename Key, typename Value>
bool CacheManager<Key, Value>::isPinned(const Key& key) const {
    std::lock_guard<std::mutex> lock(managerMutex);
    return cache->isPinned(key);
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::prefetch(const Key& key, const Value& value) {
    std::lock_guard<std::mutex> lock(managerMutex);
    cache->prefetch(key, value);
}

template<typename Key, typename Value>
CacheStatistics CacheManager<Key, Value>::getStatistics() const {
    std::lock_guard<std::mutex> lock(managerMutex);
    return cache->getStatistics();
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::resetStatistics() {
    std::lock_guard<std::mutex> lock(managerMutex);
    cache->resetStatistics();
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::showCacheAnalytics() const {
    auto stats = getStatistics();
    
    std::cout << "\n======== Cache Analytics Dashboard ========\n";
    std::cout << "Policy: ";
    switch (currentPolicy) {
        case CachePolicy::LRU: std::cout << "LRU (Least Recently Used)"; break;
        case CachePolicy::LFU: std::cout << "LFU (Least Frequently Used)"; break;
        case CachePolicy::FIFO: std::cout << "FIFO (First In, First Out)"; break;
        case CachePolicy::LIFO: std::cout << "LIFO (Last In, First Out)"; break;
    }
    std::cout << "\n";
    std::cout << "Capacity: " << cacheCapacity << "\n";
    std::cout << "Current Size: " << cache->size() << "\n";
    std::cout << "Hit Rate: " << std::fixed << std::setprecision(2) << stats.hitRate << "%\n";
    std::cout << "Total Hits: " << stats.hits << "\n";
    std::cout << "Total Misses: " << stats.misses << "\n";
    std::cout << "Total Evictions: " << stats.evictions << "\n";
    std::cout << "Pinned Items: " << stats.pinnedItems << "\n";
    std::cout << "Prefetched Items: " << stats.prefetchedItems << "\n";
    std::cout << "==========================================\n\n";
}

template<typename Key, typename Value>
std::vector<Key> CacheManager<Key, Value>::getHotKeys(size_t count) const {
    std::lock_guard<std::mutex> lock(managerMutex);
    auto keys = cache->getKeys();
    
    // For now, return first 'count' keys
    // In a real implementation, you'd sort by access frequency/recency
    if (keys.size() > count) {
        keys.resize(count);
    }
    return keys;
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::warmup(const std::vector<std::pair<Key, Value>>& data) {
    std::lock_guard<std::mutex> lock(managerMutex);
    for (const auto& pair : data) {
        cache->prefetch(pair.first, pair.second);
    }
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::optimizeForWorkload() {
    // This could implement adaptive policy selection based on access patterns
    // For now, it's a placeholder for future optimization algorithms
    auto stats = getStatistics();
    
    // Simple heuristic: if hit rate is low, consider switching policies
    if (stats.hitRate < 50.0 && stats.totalAccesses > 100) {
        std::cout << "Cache performance is suboptimal. Consider switching cache policy.\n";
    }
}

template<typename Key, typename Value>
void CacheManager<Key, Value>::recreateCache() {
    switch (currentPolicy) {
        case CachePolicy::LRU:
            cache = std::make_unique<EnhancedLRUCache<Key, Value>>(cacheCapacity);
            break;
        case CachePolicy::LFU:
            cache = std::make_unique<LFUCache<Key, Value>>(cacheCapacity);
            break;
        case CachePolicy::FIFO:
            cache = std::make_unique<FIFOCache<Key, Value>>(cacheCapacity);
            break;
        case CachePolicy::LIFO:
            // For now, use FIFO as LIFO placeholder
            cache = std::make_unique<FIFOCache<Key, Value>>(cacheCapacity);
            break;
        default:
            cache = std::make_unique<EnhancedLRUCache<Key, Value>>(cacheCapacity);
            break;
    }
}

} // namespace mtfs::cache
