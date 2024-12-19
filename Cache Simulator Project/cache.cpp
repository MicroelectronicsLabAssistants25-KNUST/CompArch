
#include <iostream>
#include <math.h>
//#include <netinet/in.h>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

struct CacheBlock {
  bool valid;
  bool dirty;
  long tag;
  int lru_counter;

  CacheBlock() : valid(false), dirty(false), tag(0), lru_counter(0) {}
};

void printCacheBlock(CacheBlock &cache_block) {
  std::cout << "Valid Bit: " << cache_block.valid << "\t" 
    << "Dirty Bit: " << cache_block.dirty << "\t"
    << "Tag Bit: " << cache_block.tag << "\t"
    << "LRU Counter: " << cache_block.lru_counter << "\n";
}

class Cache {
private:
  int associativity;
  int blocksize_bytes;
  int cachesize_kb;
  int miss_penalty;

  int num_sets;
  int index_bits;
  int block_offset_bits;

  std::vector<std::vector<CacheBlock>> cache_data;

  // Statistics
  long total_instructions;
  long memory_accesses;
  int load_hits;
  int load_misses;
  int store_hits;
  int store_misses;
  int dirty_evictions;
  long execution_cycles;

public:
  Cache(int assoc, int blk_size, int cache_size, int miss_pen)
      : associativity(assoc), blocksize_bytes(blk_size),
        cachesize_kb(cache_size), miss_penalty(miss_pen), total_instructions(0),
        memory_accesses(0), load_hits(0), load_misses(0), store_hits(0),
        store_misses(0), dirty_evictions(0), execution_cycles(0) {


    // Calculate cache organization
    int total_blocks = (cachesize_kb * 1024) / blocksize_bytes;
    num_sets = total_blocks / associativity;

    // Calculate bit fields
    block_offset_bits = log2(blocksize_bytes);
    index_bits = log2(num_sets);

    // Initialize cache structure
    cache_data.resize(num_sets, std::vector<CacheBlock>(associativity));
  }

  std::vector<std::vector<CacheBlock>> get_cache_data() {
    return cache_data;
  }

  void access_memory(int loadstore, long address, int icount) {
    memory_accesses++;
    total_instructions += icount;
    execution_cycles += icount; // Base execution time for instructions

    // Extract tag and index from address
    long tag = address >> (block_offset_bits + index_bits);
    int index = (address >> block_offset_bits) & ((1 << index_bits) - 1);

    bool hit = false;
    int way = -1;

    // Check for hit
    for (int i = 0; i < associativity; i++) {
      if (cache_data[index][i].valid && cache_data[index][i].tag == tag) {
        hit = true;
        way = i;
        break;
      }
    }

    if (hit) {
      // Update LRU counter
      cache_data[index][way].lru_counter = memory_accesses;

      if (loadstore == 0) { // Load hit
        load_hits++;
      } else { // Store hit
        store_hits++;
        cache_data[index][way].dirty = true;
      }
    } else {
      // Handle miss
      if (loadstore == 0) { // Load miss
        load_misses++;
      } else { // Store miss
        store_misses++;
      }

      execution_cycles += miss_penalty;

      // Find LRU way or invalid block
      way = -1;
      int lru_way = 0;
      long min_counter = cache_data[index][0].lru_counter;

      for (int i = 0; i < associativity; i++) {
        if (!cache_data[index][i].valid) {
          way = i;
          break;
        }
        if (cache_data[index][i].lru_counter < min_counter) {
          min_counter = cache_data[index][i].lru_counter;
          lru_way = i;
        }
      }

      if (way == -1) {
        way = lru_way;
        // Check if we're evicting a dirty block
        if (cache_data[index][way].dirty) {
          dirty_evictions++;
          execution_cycles += 2; // Extra cycles for writing dirty block
        }
      }

      // Update block
      cache_data[index][way].valid = true;
      cache_data[index][way].dirty = (loadstore == 1); // Set dirty if store
      cache_data[index][way].tag = tag;
      cache_data[index][way].lru_counter = memory_accesses;
    }
  }

  void print_statistics() {
    printf("Simulation results:\n");
    printf("\texecution time %ld cycles\n", execution_cycles);
    printf("\tinstructions %ld\n", total_instructions);
    printf("\tmemory accesses %ld\n", memory_accesses);
    printf("\toverall miss rate %.2f\n",
           (float)(load_misses + store_misses) / memory_accesses);
    printf("\tread miss rate %.2f\n",
           (float)load_misses / (load_hits + load_misses));
    printf("\tmemory CPI %.2f\n",
           (float)(execution_cycles - total_instructions) / total_instructions);
    printf("\ttotal CPI %.2f\n", (float)execution_cycles / total_instructions);
    printf("\taverage memory access time %.2f cycles\n",
           (float)(execution_cycles - total_instructions) / memory_accesses);
    printf("dirty evictions %d\n", dirty_evictions);
    printf("load_misses %d\n", load_misses);
    printf("store_misses %d\n", store_misses);
    printf("load_hits %d\n", load_hits);
    printf("store_hits %d\n", store_hits);
  }
};

int main(int argc, char *argv[]) {
  int associativity = 2;
  int blocksize_bytes = 32;
  int cachesize_kb = 64;
  int miss_penalty = 30;

  // Process command line arguments
  for (int j = 1; j < argc; j++) {
    if (strcmp("-a", argv[j]) == 0) {
      associativity = atoi(argv[++j]);
    } else if (strcmp("-l", argv[j]) == 0) {
      blocksize_bytes = atoi(argv[++j]);
    } else if (strcmp("-s", argv[j]) == 0) {
      cachesize_kb = atoi(argv[++j]);
    } else if (strcmp("-mp", argv[j]) == 0) {
      miss_penalty = atoi(argv[++j]);
    }
  }

  // Print cache configuration
  printf("Cache parameters:\n");
  printf("Cache Size (KB)\t\t\t%d\n", cachesize_kb);
  printf("Cache Associativity\t\t%d\n", associativity);
  printf("Cache Block Size (bytes)\t%d\n", blocksize_bytes);
  printf("Miss penalty (cyc)\t\t%d\n", miss_penalty);
  printf("\n");

  // Create cache
  Cache cache(associativity, blocksize_bytes, cachesize_kb, miss_penalty);

  // Process trace
  char marker;
  int loadstore;
  long address;
  int icount;

  while (scanf("%c %d %lx %d\n", &marker, &loadstore, &address, &icount) !=
         EOF) {
    cache.access_memory(loadstore, address, icount);
  }

  // Print out the data blocks in the 
  while (true) {
    for (auto cache: cache.get_cache_data()) {
      for (auto blk: cache) {
        printCacheBlock(blk);
      }
    }

    break;
  }

  // Print statistics
  cache.print_statistics();

  return 0;
}

