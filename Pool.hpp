#pragma once

#include "Slab.hpp"
#include <vector>
#include <cstddef>

namespace slab {

class Pool {
public:
    explicit Pool(std::size_t chunkSize);
    ~Pool();

    void* allocate();           // Allocate from any available slab
    void  deallocate(void* p);  // Deallocate to the appropriate slab
    bool  empty() const;        // Check if all slabs are empty
    bool  has_free_chunks() const; // true if pool has any free chunks
    bool  contains(void* ptr) const; // true if p is within the pools's memory range

    // Disable copying
    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;

private:
    std::size_t chunkSize_;
    std::vector<Slab*> slabs_;  // Vector of slabs for this chunk size
    Slab* last_used_slab_ = nullptr;  // Cache for last used slab
    
    Slab* findSlabForPointer(void* ptr) const;  // Helper to find which slab owns a pointer
    Slab* getOrCreateSlab();                    // Get an available slab or create a new one
};

} // namespace slab
