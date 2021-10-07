# Cache-Simulator
Simulates a 1 or 2 level cache, and returns the number of times a cache hit or miss occurred, and the number of times memory was written to or read from, given a trace file input.

## One Level Cache:

 ### Input: 
   cache size, associativity, cache ejection policy, cache block size, trace file
    
    where:
     Cache Size is in bytes, and is a power of 2.
     Associativity is either "direct", "assoc", or "assoc:n", where n is a power of 2. (assoc referring to fully associative, and assoc:n referring to n-asocciative.
     Cache Ejection Policy is either "fifo" or "lru". ("First In, First Out" and "Least Recently Used" respectively)
     Cache Block Size is the size of each cache block in bytes and is a power of 2.  
     Trace file is a .txt file that on each line contains whether it is a Read (R) or Write (W) command, followed by a 48-bit memory address that is being accessed.
          Example:
                  W 0x9cb62d0
                  R 0x9cb62d4
  
 ### Output: 
    Number of times memory had to be read from.
    Number of times memory had to be written to.
    Number of times the accessed address was cached.
    Number of times the acceessed address was not cached.
  
## Two Level Cache:
    
    The two caches have a write-through relationship, so things that exist in L1 do not exist in L2 and vice versa.
    
   ### Input: 
  L1 cache size, L1 associativity, L1 cache ejection policy, L1 block size, L2 cache size, L2 associativity, L2 cache ejection policy, trace file
  
   ### Output: 
     Number of times memory had to be read from.
     Number of times memory had to be written to.
     Number of times the accessed address was cached in L1.
     Number of times the acceessed address was not cached in L1.
     Number of times the accessed address was cached in L2.
     Number of times the acceessed address was not cached in L2.
  
## Validity Checks: 
   ### The program will check for the following input errors, and will return and print "error":
      - Incorrect number of inputs.
      - Cache Size is not a power of 2.
      - Cache Block Size is not a power of 2.
      - Trace file does not exist.
      - Associativity is incorrectly formatted.
