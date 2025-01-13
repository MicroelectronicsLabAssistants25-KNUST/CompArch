// Same Copy

#include <iostream> // For input and output
#include <iomanip>  // For formatted output (e.g., floating-point precision)
#include <cstdlib>  // For atoi
#include <cstring>  // For strcmp
#include <cmath>    // For mathematical operations
#include <vector>
#include "cache.h"

// 7z x -so art.trace.gz | cacheSim -s 16 -a 1 -l 16 -mp 30

// Global variables for default cache configuration
int associativity = 2;          // Associativity of cache
int blocksize_bytes = 32;       // Cache block size in bytes
int cachesize_kb = 64;          // Cache size in KB
int miss_penalty = 30;          // Miss penalty in cycles

// Function to display usage information
void print_usage() {
    std::cout << "Usage: 7z x -so <tracefile> | ./cache -a assoc -l blksz -s size -mp mispen\n";
    std::cout << "  tracefile : The memory trace file\n";
    std::cout << "  -a assoc : The associativity of the cache\n";
    std::cout << "  -l blksz : The blocksize (in bytes) of the cache\n";
    std::cout << "  -s size : The size (in KB) of the cache\n";
    std::cout << "  -mp mispen: The miss penalty (in cycles) of a miss\n";
    exit(0);
}


int main(int argc, char* argv[]) {
    long address;
    int loadstore, icount;
    char marker;

    int i = 0; // Counter for tracking memory accesses
    std::cout << "argc = " << argc << std::endl;
    for (int i = 0; i < argc; i++) {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    }
    // Process command-line arguments
    for (int j = 1; j < argc; ++j) {
        if (strcmp("-a", argv[j]) == 0) {
            ++j;
            if (j >= argc) print_usage();
            associativity = std::atoi(argv[j]);
        } 
        else if (strcmp("-l", argv[j]) == 0) {
            ++j;
            if (j >= argc) print_usage();
            blocksize_bytes = std::atoi(argv[j]);
            std::cout<<"argument  "<<argv[j]<<std::endl;
        } 
        else if (strcmp("-s", argv[j]) == 0) {
            ++j;
            if (j >= argc) print_usage();
            cachesize_kb = std::atoi(argv[j]);
        } 
        else if (strcmp("-mp", argv[j]) == 0) {
            ++j;
            if (j >= argc) print_usage();
            miss_penalty = std::atoi(argv[j]);
        } 
        else {
            print_usage();
        }
    }

    // Print out cache configuration
    std::cout << "Cache parameters:\n";
    std::cout << "Cache Size (KB)\t\t\t" << cachesize_kb << "\n";
    std::cout << "Cache Associativity\t\t" << associativity << "\n";
    std::cout << "Cache Block Size (bytes)\t" << blocksize_bytes << "\n";
    std::cout << "Miss penalty (cycles)\t\t" << miss_penalty << "\n\n";


    //Create Cache memory with specifications
    Cache cache_mem(associativity, blocksize_bytes, cachesize_kb, miss_penalty);
    

    // Process memory trace input
    while (std::cin >> marker >> loadstore >> std::hex >> address >> std::dec >> icount) {
        // Code to print out just the first 10 addresses. You can modify/remove this part later.
        
        cache_mem.memory_access(loadstore, address, icount);
        ++i;
    }


    // Statistics
    long total_instructions;
    long memory_accesses;
    int load_hits;
    int load_misses;
    int store_hits;
    int store_misses;
    int dirty_evictions;
    long execution_cycles;

    // Print summary statistics
    std::cout << "Lines found = " << i << " \n";
    std::cout << "Simulation results:\n";
    cache_mem.print_statistics();

    // Replace the placeholders with actual calculations as you build the simulator
    /*
    std::cout << "\texecution time " << ? << " cycles\n";
    std::cout << "\tinstructions " << ? << "\n";
    std::cout << "\tmemory accesses " << ? << "\n";
    std::cout << "\toverall miss rate " << std::fixed << std::setprecision(2) << ? << "\n";
    std::cout << "\tread miss rate " << std::fixed << std::setprecision(2) << ? << "\n";
    std::cout << "\tmemory CPI " << std::fixed << std::setprecision(2) << ? << "\n";
    std::cout << "\ttotal CPI " << std::fixed << std::setprecision(2) << ? << "\n";
    std::cout << "\taverage memory access time " << std::fixed << std::setprecision(2) << ? << " cycles\n";
    std::cout << "dirty evictions " << ? << "\n";
    std::cout << "load_misses " << ? << "\n";
    std::cout << "store_misses " << ? << "\n";
    std::cout << "load_hits " << ? << "\n";
    std::cout << "store_hits " << ? << "\n";
    */
   
   
//    Command to run
   //7z x -so art.trace.gz | cacheSim -s 64 -a 2 -l 32 -mp 30

    return 0;
}
