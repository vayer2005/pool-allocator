#include "Slab.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace slab {

Slab::Slab(size_t chunkSize) : chunkSize_(chunkSize), memory_(nullptr), freeList_(nullptr) {
    // TODO: Initialize the slab
    // 1. Allocate memory_ using malloc or aligned_alloc
    // 2. Initialize freeList_ as a linked list of all chunks
    // 3. Each chunk should point to the next chunk

    memory_ = malloc(kSlabSize);

    for (size_t i = 0; i < kSlabSize; i += chunkSize_) {
        void* chunk = ((char*)memory_) + i;
        Node* node = (Node*)chunk;  
        node->next = freeList_;
        freeList_ = node;
    }

    // std::cout << "Slab initialized with " << kSlabSize << " bytes" << std::endl;
}

Slab::~Slab() {
    // TODO: Free the allocated memory
    // 1. Free memory_ using free()
    // 2. Reset pointers to nullptr

    free(memory_);
    freeList_ = nullptr;
}

void* Slab::allocate() {
    // TODO: Allocate a chunk from the free list
    // 1. Check if freeList_ is not nullptr
    // 2. Remove the first chunk from freeList_
    // 3. Return the chunk pointer
    // 4. Return nullptr if no chunks available
    if (freeList_ == nullptr) return nullptr;

    Node* node = freeList_;
    freeList_ = node->next;
    return (void*)node; 
}

void Slab::deallocate(void* ptr) {
    // TODO: Return a chunk to the free list
    // 1. Validate the pointer is within the slab's memory range
    // 2. Add the chunk back to the front of freeList_
    // 3. Update freeList_ to point to this chunk

    auto* base = (char*)memory_;
    auto* end_of_slab = ((char*)memory_ + kSlabSize);
    auto* p = (char*)ptr;
    if(p >= base && p < end_of_slab) {
        Node* node = (Node*)ptr; 
        node->next = freeList_;
        freeList_ = node;
    }
}

bool Slab::empty() const {
    // TODO: Check if all chunks are free
    // 1. Count total chunks in the slab
    // 2. Count chunks in freeList_
    // 3. Return true if all chunks are in freeList_
    
    size_t totalChunks = kSlabSize / chunkSize_;
    
    size_t freeChunks = 0;
    Node* current = freeList_;
    while (current != nullptr) {
        freeChunks++;
        current = current->next;
    }
    
    return freeChunks == totalChunks;
}



bool Slab::contains(void* ptr) const {
    // TODO: Check if the pointer is within the slab's memory range
    // 1. Validate the pointer is not nullptr
    // 2. Check if the pointer is within the slab's memory range
    // 3. Return true if the pointer is within the slab's memory range
    auto* base = (char*)memory_;
    auto* end_of_slab = ((char*)memory_ + kSlabSize);
    auto* p = (char*)ptr;
    if(p >= base && p < end_of_slab) return true;
    return false;
}

} // namespace slab
