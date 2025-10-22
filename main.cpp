#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <memory>

#include "csim.h"  // Include header instead of cpp

using namespace std;

struct Stats {
    int total_loads = 0;
    int total_stores = 0;
    int load_hits = 0;
    int load_misses = 0;
    int store_hits = 0;
    int store_misses = 0;
    unsigned long long total_cycles = 0;
};

// helper function to check if a number is a power of 2
bool is_power_of_2(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

int main(int argc, char **argv) {
    // Step 1: check command line arguments
    if (argc != 7) {
        cerr << "Usage: " << argv[0] << " <sets> <blocks> <bytes> "
             << "<write-allocate|no-write-allocate> "
             << "<write-through|write-back> <lru|fifo>" << endl;
        return 1;
    }
    
    // Step 2: parse parameters
    int num_sets = atoi(argv[1]);
    int blocks_per_set = atoi(argv[2]);
    int block_size = atoi(argv[3]);
    
    // Step 3: check parameters for validity, handle errors
    if (!is_power_of_2(num_sets)) {
        cerr << "Set count must be a power of 2" << endl;
        return 1;
    }
    if (!is_power_of_2(blocks_per_set)) {
        cerr << "Blocks per set must be a power of 2" << endl;
        return 1;
    }
    if (!is_power_of_2(block_size) || block_size < 4) {
        cerr << "Block size must be a power of 2 and at least 4" << endl;
        return 1;
    }
    
    // check for write type
    bool write_allocate;
    if (strcmp(argv[4], "write-allocate") == 0) {
        write_allocate = true;
    } else if (strcmp(argv[4], "no-write-allocate") == 0) {
        write_allocate = false;
    } else {
        cerr << "Invalid write allocate policy" << endl;
        return 1;
    }
    
    bool write_through;
    if (strcmp(argv[5], "write-through") == 0) {
        write_through = true;
    } else if (strcmp(argv[5], "write-back") == 0) {
        write_through = false;
    } else {
        cerr << "Invalid write policy" << endl;
        return 1;
    }
    
    if (!write_allocate && !write_through) {
        cerr << "no-write-allocate and write-back is not a valid combination" << endl;
        return 1;
    }
    
    bool use_lru;
    if (strcmp(argv[6], "lru") == 0) {
        use_lru = true;
    } else if (strcmp(argv[6], "fifo") == 0) {
        use_lru = false;
    } else {
        cerr << "Invalid eviction policy" << endl;
        return 1;
    }
    
    // Step 4: create cache and stats 
    // we use runtime polymorphism for good organization when switching caches
    unique_ptr<Cache> cache;
    
    if (use_lru) {
        cache = make_unique<LRUCache>(num_sets * blocks_per_set * block_size,
                                      blocks_per_set, block_size,
                                      write_allocate, write_through);
    } else {
        cache = make_unique<FIFOCache>(num_sets * blocks_per_set * block_size,
                                       blocks_per_set, block_size,
                                       write_allocate, write_through);
    }
    
    Stats stats;
    
    // Step 5: process file and run simulation
    char op;
    unsigned int addr;
    int ignore;
    
    while (scanf(" %c %x %d", &op, &addr, &ignore) == 3) {
        if (op == 'l') {
            stats.total_cycles += cache->handle_load(addr, stats);
        } else if (op == 's') {
            stats.total_cycles += cache->handle_store(addr, stats);
        } else {
            cerr << "Invalid operation: " << op << endl;
            return 1;
        }
    }
    
    // Step 6: print results
    cout << "Total loads: " << stats.total_loads << endl;
    cout << "Total stores: " << stats.total_stores << endl;
    cout << "Load hits: " << stats.load_hits << endl;
    cout << "Load misses: " << stats.load_misses << endl;
    cout << "Store hits: " << stats.store_hits << endl;
    cout << "Store misses: " << stats.store_misses << endl;
    cout << "Total cycles: " << stats.total_cycles << endl;
    
    return 0;
}