#include "csim.h"
#include <limits>

LRUCache::LRUCache(uint32_t size, uint32_t ways, uint32_t block_size,
                   bool write_alloc, bool write_thr)
    : Cache(size, ways, block_size, write_alloc, write_thr) {}

void LRUCache::update_on_hit(uint32_t set_index, uint32_t way) {
    // update timestamp to mark most recently used
    sets[set_index][way].last_access_time = ++global_timestamp;
}

void LRUCache::update_on_miss(uint32_t set_index, uint32_t way) {
    // set timestamp for newly inserted line
    sets[set_index][way].last_access_time = ++global_timestamp;
}

uint32_t LRUCache::find_victim(uint32_t set_index) {
    // look for invalid entry
    for (uint32_t way = 0; way < num_ways; way++) {
        if (!sets[set_index][way].valid) {
            return way;
        }
    }
    
    // find LRU (oldest timestamp)
    uint32_t lru = 0;
    uint64_t oldest_time = sets[set_index][0].last_access_time;
    
    for (uint32_t way = 1; way < num_ways; way++) {
        if (sets[set_index][way].last_access_time < oldest_time) {
            oldest_time = sets[set_index][way].last_access_time;
            lru = way;
        }
    }
    
    return lru;
}