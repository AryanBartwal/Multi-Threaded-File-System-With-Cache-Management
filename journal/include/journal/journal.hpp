#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>
#include "common/error.hpp"
#include "storage/block_manager.hpp"

namespace mtfs::journal {

enum class JournalEntryType {
    CREATE_FILE,
    DELETE_FILE,
    WRITE_DATA,
    CREATE_DIR,
    DELETE_DIR,
    UPDATE_METADATA
};

struct JournalEntry {
    uint64_t sequenceNumber;
    JournalEntryType type;
    std::chrono::system_clock::time_point timestamp;
    std::vector<storage::BlockId> blocks;
    std::vector<uint8_t> metadata;
};

class Journal {
public:
    // Factory method
    static std::shared_ptr<Journal> create(std::shared_ptr<storage::BlockManager> blockManager);

    // Journal operations
    virtual void beginTransaction() = 0;
    virtual void commitTransaction() = 0;
    virtual void rollbackTransaction() = 0;

    // Logging operations
    virtual void logEntry(const JournalEntry& entry) = 0;
    virtual std::vector<JournalEntry> getEntries(uint64_t fromSequence, uint64_t toSequence) = 0;

    // Recovery operations
    virtual bool needsRecovery() = 0;
    virtual void recover() = 0;
    virtual void checkpoint() = 0;

    // Management
    virtual void clear() = 0;
    virtual size_t size() const = 0;
    virtual uint64_t getLastSequenceNumber() const = 0;

    Journal();
    ~Journal();

protected:
    Journal() = default;
};

} // namespace mtfs::journal 