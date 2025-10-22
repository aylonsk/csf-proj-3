#include "csim.h"
#include <iostream>
#include <cmath>

// include stats def
struct Stats {
    int total_loads = 0;
    int total_stores = 0;
    int load_hits = 0;
    int load_misses = 0;
    int store_hits = 0;
    int store_misses = 0;
    unsigned long long total_cycles = 0;
};

Cache::Cache(uint32_t size, uint32_t ways, uint32_t block_size, 
             bool write_alloc, bool write_thr)
    : cache_size(size), num_ways(ways), block_size(block_size),
      write_allocate(write_alloc), write_through(write_thr) {
    
    num_sets = cache_size / (num_ways * block_size);
    offset_bits = log2(block_size);
    index_bits = log2(num_sets);
    tag_bits = 32 - offset_bits - index_bits;
    
    sets.resize(num_sets);
    for (auto& set : sets) {
        set.resize(num_ways);
    }
}

std::tuple<uint32_t, uint32_t, uint32_t> Cache::parse_address(uint32_t address) {
    uint32_t offset = address & ((1 << offset_bits) - 1);
    uint32_t index = (address >> offset_bits) & ((1 << index_bits) - 1);
    uint32_t tag = address >> (offset_bits + index_bits);
    return {tag, index, offset};
}

std::pair<bool, uint32_t> Cache::check_hit(uint32_t set_index, uint32_t tag) {
    for (uint32_t way = 0; way < num_ways; way++) {
        if (sets[set_index][way].valid && sets[set_index][way].tag == tag) {
            return {true, way};
        }
    }
    return {false, 0};
}

void Cache::evict_line(uint32_t set_index, uint32_t way) {
    // writeback cycles handled in handle_load/handle_store functions
    sets[set_index][way].valid = false;
    sets[set_index][way].dirty = false;
}

void Cache::install_line(uint32_t set_index, uint32_t way, uint32_t tag, bool is_write) {
    sets[set_index][way].valid = true;
    sets[set_index][way].tag = tag;
    sets[set_index][way].dirty = !write_through && is_write;
}

int Cache::handle_load(uint32_t address, Stats& stats) {
    stats.total_loads++;
    int cycles = 0;
    
    auto [tag, set_index, offset] = parse_address(address);
    auto [hit, hit_way] = check_hit(set_index, tag);
    
    if (hit) {
        stats.load_hits++;
        cycles = 1;
        update_on_hit(set_index, hit_way);
    } else { // load from memory
        stats.load_misses++;
        cycles = 100 * (block_size / 4);  
        
        // handle eviction
        uint32_t victim = find_victim(set_index);
        
        // check for writeback
        if (sets[set_index][victim].valid && 
            sets[set_index][victim].dirty) {
            cycles += 100 * (block_size / 4);  // writeback cycle handling
        }
        
        evict_line(set_index, victim);
        install_line(set_index, victim, tag, false);
        update_on_miss(set_index, victim);
    }
    
    return cycles;
}

int Cache::handle_store(uint32_t address, Stats& stats) {
    stats.total_stores++;
    int cycles = 0;
    
    auto [tag, set_index, offset] = parse_address(address);
    auto [hit, hit_way] = check_hit(set_index, tag);
    
    if (hit) {
        stats.store_hits++;
        cycles = 1;
        
        if (write_through) { // write to memory
            cycles += 100;  
        } else {
            sets[set_index][hit_way].dirty = true;
        }
        
        update_on_hit(set_index, hit_way);
    } else {
        stats.store_misses++;
        
        if (write_allocate) {
            // write-allocate: bring block into cache
            cycles = 100 * (block_size / 4);
            
            uint32_t victim = find_victim(set_index);
            
            if (sets[set_index][victim].valid && 
                sets[set_index][victim].dirty) {
                cycles += 100 * (block_size / 4);
            }
            
            evict_line(set_index, victim);
            install_line(set_index, victim, tag, true);  // true = is a write
            update_on_miss(set_index, victim);
            
            if (write_through) {
                cycles += 100;
            }
        } else {
            // no-write-allocate: write directly to memory
            cycles = 100;
        }
    }
    
    return cycles;
}