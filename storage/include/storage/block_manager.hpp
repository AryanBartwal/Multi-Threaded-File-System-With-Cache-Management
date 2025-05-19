#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "common/error.hpp"

namespace mtfs::storage {

using BlockId = uint64_t;

struct Block {
    static constexpr size_t BLOCK_SIZE = 4096;  // 4KB blocks
    uint8_t data[BLOCK_SIZE];
};

class BlockManager {
public:
    // Factory method
    static std::shared_ptr<BlockManager> create(const std::string& devicePath);

    // Block operations
    virtual BlockId allocateBlock() = 0;
    virtual void freeBlock(BlockId blockId) = 0;
    virtual void readBlock(BlockId blockId, Block& block) = 0;
    virtual void writeBlock(BlockId blockId, const Block& block) = 0;

    // Batch operations
    virtual std::vector<BlockId> allocateBlocks(size_t count) = 0;
    virtual void readBlocks(const std::vector<BlockId>& blockIds, std::vector<Block>& blocks) = 0;
    virtual void writeBlocks(const std::vector<BlockId>& blockIds, const std::vector<Block>& blocks) = 0;

    // Management operations
    virtual void format() = 0;
    virtual void sync() = 0;
    virtual size_t getFreeBlockCount() const = 0;
    virtual size_t getTotalBlockCount() const = 0;

    ~BlockManager();

protected:
    BlockManager() = default;
};

} // namespace mtfs::storage 