#pragma once

#include <cstddef>

namespace slab {

struct Node {
    Node* next;     // Points to the next Node
};

class Slab {
public:

    static constexpr std::size_t kSlabSize = 1 << 14;  // 16KB instead of 1MB 

    explicit Slab(std::size_t chunkSize); // chunk size (power-of-two preferred)
    ~Slab();

    void* allocate();           // O(1) pop from free list; nullptr if exhausted
    void  deallocate(void* p);  // O(1) push onto free list
    bool  empty() const;        // true if every chunk is currently free
    inline bool has_free_chunks() const { return freeList_ != nullptr; } // true if slab has any free chunks
    bool  contains(void* ptr) const; // true if p is within the slab's memory range

    Slab(const Slab&)            = delete;
    Slab& operator=(const Slab&) = delete;

private:
    std::size_t chunkSize_; // bytes per chunk
    void*       memory_;    // raw slab memory returned by malloc/mmap
    Node*       freeList_;  // singly-linked list of free chunks (intrusive)
};

} // namespace slab
