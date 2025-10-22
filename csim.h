#ifndef CACHE_HPP
#define CACHE_HPP

#include <vector>
#include <tuple>
#include <cstdint>

// forward declaration for stats in main.cpp
struct Stats;

struct CacheLine {
    bool valid = false;
    bool dirty = false;
    uint32_t tag = 0;
    uint64_t last_access_time = 0;  // LRU
    uint64_t insertion_time = 0;    // FIFO
};

class Cache {
protected:
    uint32_t cache_size;
    uint32_t num_ways;
    uint32_t block_size;
    uint32_t num_sets;
    uint32_t offset_bits;
    uint32_t index_bits;
    uint32_t tag_bits;
    
    bool write_allocate;
    bool write_through;
    
    std::vector<std::vector<CacheLine>> sets;
    
    // shared operations
    std::tuple<uint32_t, uint32_t, uint32_t> parse_address(uint32_t addr);
    std::pair<bool, uint32_t> check_hit(uint32_t set_index, uint32_t tag);
    void evict_line(uint32_t set_index, uint32_t way);
    void install_line(uint32_t set_index, uint32_t way, uint32_t tag, bool is_write);
    
    // operations to be implemented by subclasses
    virtual void update_on_hit(uint32_t set_index, uint32_t way) = 0;
    virtual void update_on_miss(uint32_t set_index, uint32_t way) = 0;
    virtual uint32_t find_victim(uint32_t set_index) = 0;
    
public:
    Cache(uint32_t size, uint32_t ways, uint32_t block_size, 
          bool write_alloc, bool write_thr);
    virtual ~Cache() = default; //for lru and fifo to implement separately
    
    // public interface - returns the cycles used
    int handle_load(uint32_t address, Stats& stats);
    int handle_store(uint32_t address, Stats& stats);
};

// LRU Cache Implementation
class LRUCache : public Cache {
private:
    uint64_t global_timestamp = 0;
    
protected:
    void update_on_hit(uint32_t set_index, uint32_t way) override;
    void update_on_miss(uint32_t set_index, uint32_t way) override;
    uint32_t find_victim(uint32_t set_index) override;
    
public:
    LRUCache(uint32_t size, uint32_t ways, uint32_t block_size,
             bool write_alloc, bool write_thr);
};

// FIFO Cache Implementation
class FIFOCache : public Cache {
private:
    uint64_t global_timestamp = 0;
    
protected:
    void update_on_hit(uint32_t set_index, uint32_t way) override;
    void update_on_miss(uint32_t set_index, uint32_t way) override;
    uint32_t find_victim(uint32_t set_index) override;
    
public:
    FIFOCache(uint32_t size, uint32_t ways, uint32_t block_size,
              bool write_alloc, bool write_thr);
};

#endif