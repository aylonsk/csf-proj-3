#include "csim.h"
#include <limits>

FIFOCache::FIFOCache(uint32_t size, uint32_t ways, uint32_t block_size,
                     bool write_alloc, bool write_thr)
    : Cache(size, ways, block_size, write_alloc, write_thr) {}

void FIFOCache::update_on_hit(uint32_t set_index, uint32_t way) {
    // FIFO doesn't update on hit, leave blank
    return;
}

void FIFOCache::update_on_miss(uint32_t set_index, uint32_t way) {
    // set insertion time for new line
    sets[set_index][way].insertion_time = ++global_timestamp;
}

uint32_t FIFOCache::find_victim(uint32_t set_index) {
    // look for invalid entry
    for (uint32_t way = 0; way < num_ways; way++) {
        if (!sets[set_index][way].valid) {
            return way;
        }
    }
    
    // find FI (oldest insertion time)
    uint32_t oldest = 0;
    uint64_t oldest_time = sets[set_index][0].insertion_time;
    
    for (uint32_t way = 1; way < num_ways; way++) {
        if (sets[set_index][way].insertion_time < oldest_time) {
            oldest_time = sets[set_index][way].insertion_time;
            oldest = way;
        }
    }
    
    return oldest;
}