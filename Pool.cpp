#include "Pool.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

namespace slab {

Pool::Pool(std::size_t chunkSize) : chunkSize_(chunkSize) {
    // TODO: Initialize the pool
    // 1. Create the first slab for this chunk size
    // 2. Add it to the slabs_ vector
    Slab* slab = new Slab(chunkSize);
    slabs_.push_back(slab);
}

Pool::~Pool() {
    // TODO: Clean up all slabs
    // 1. Delete all slabs in slabs_ vector
    // 2. Clear the vector
    for (size_t i = 0; i < slabs_.size(); i++) {
        delete slabs_[i];
    }
    slabs_.clear();
}

void* Pool::allocate() {
    // TODO: Allocate from available slabs
    // 1. Try to find a slab with available chunks
    // 2. If none found, create a new slab
    // 3. Allocate from the selected slab
    
    if (last_used_slab_ && last_used_slab_->has_free_chunks()) {
        return last_used_slab_->allocate();
    }
    
    Slab* avail = getOrCreateSlab();
    last_used_slab_ = avail;
    return avail->allocate();
}

void Pool::deallocate(void* ptr) {
    // TODO: Deallocate to the appropriate slab
    // 1. Find which slab owns this pointer
    // 2. Deallocate to that slab
    // 3. Handle case where pointer doesn't belong to any slab

    Slab* toDeallocate = findSlabForPointer(ptr);
    if (toDeallocate == nullptr) {
        throw std::runtime_error("Pointer does not belong to any slab");
    }
    toDeallocate->deallocate(ptr);
    
}

bool Pool::empty() const {
    // TODO: Check if all slabs are empty
    // 1. Check if slabs_ vector is empty
    // 2. Check if all slabs in the vector are empty

    for (size_t i =0; i < slabs_.size(); i++) {
        if (!slabs_[i]->empty()) return false;
    }

    return true;
}

bool Pool::has_free_chunks() const {
    // Check if any slab has free chunks
    for (size_t i = 0; i < slabs_.size(); i++) {
        if (slabs_[i]->has_free_chunks()) {
            return true;
        }
    }
    return false;
}

Slab* Pool::findSlabForPointer(void* ptr) const {
    // TODO: Find which slab owns the given pointer
    // 1. Iterate through all slabs
    // 2. Check if pointer is within each slab's memory range
    // 3. Return the owning slab or nullptr if not found

    for (size_t i = 0; i < slabs_.size(); i++) {
        if (slabs_[i]->contains(ptr)) {
            return slabs_[i];
        }
    }
    return nullptr;
}

Slab* Pool::getOrCreateSlab() {
    // TODO: Get an available slab or create a new one
    // 1. Look for a slab with available chunks
    // 2. If none found, create a new Slab and add to slabs_
    // 3. Return the available slab

    for (size_t i = 0; i < slabs_.size(); i++) {
        if (slabs_[i]->has_free_chunks()) {
            return slabs_[i];
        }
    }

    Slab* new_slab = new Slab(chunkSize_);
    slabs_.push_back(new_slab);
    return new_slab;
}

bool Pool::contains(void* ptr) const {
    // TODO: Check if the pointer is within the pool's memory range
    // 1. Iterate through all slabs
    // 2. Check if pointer is within each slab's memory range
    // 3. Return true if the pointer is within the pool's memory range

    for (size_t i = 0; i < slabs_.size(); i++) {
        if (slabs_[i]->contains(ptr)) {
            return true;
        }
    }
    return false;
}



} // namespace slab
