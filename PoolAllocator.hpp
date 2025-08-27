#pragma once

#include "Slab.hpp"
#include <cstddef>
#include <vector>

namespace slab {

class PoolAllocator {
public:
    PoolAllocator();
    ~PoolAllocator();

    void* allocate(std::size_t size);     // Allocate memory of given size
    void  deallocate(void* ptr);          // Deallocate memory
    void  reset();                        // Reset all pools (free all memory)

    // Disable copying
    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator& operator=(const PoolAllocator&) = delete;

private:
    static constexpr std::size_t kMaxPoolSize = 4096;  // Maximum size for pools
    
    std::vector<Slab*> slabs_8_;   // 8-byte slabs
    std::vector<Slab*> slabs_16_;  // 16-byte slabs
    std::vector<Slab*> slabs_32_;  // 32-byte slabs
    std::vector<Slab*> slabs_64_;  // 64-byte slabs
    std::vector<Slab*> slabs_128_; // 128-byte slabs
    std::vector<Slab*> slabs_256_; // 256-byte slabs
    std::vector<Slab*> slabs_512_; // 512-byte slabs
    std::vector<Slab*> slabs_1024_; // 1024-byte slabs
    
    // Fast path caching
    Slab* last_used_slab_8_ = nullptr;
    Slab* last_used_slab_16_ = nullptr;
    Slab* last_used_slab_32_ = nullptr;
    Slab* last_used_slab_64_ = nullptr;
    
    Slab* getOrCreateSlab(std::vector<Slab*>& slabs, std::size_t chunk_size);
    Slab* findSlabForPointer(void* ptr);
};

} // namespace slab
