#include "PoolAllocator.hpp"
#include <cstdlib>
#include <algorithm>

namespace slab {

PoolAllocator::PoolAllocator() {
    slabs_8_.push_back(new Slab(8));
    slabs_16_.push_back(new Slab(16));
    slabs_32_.push_back(new Slab(32));
    slabs_64_.push_back(new Slab(64));
    slabs_128_.push_back(new Slab(128));
    slabs_256_.push_back(new Slab(256));
    slabs_512_.push_back(new Slab(512));
    slabs_1024_.push_back(new Slab(1024));
}

PoolAllocator::~PoolAllocator() {
    for (Slab* slab : slabs_8_) delete slab;
    for (Slab* slab : slabs_16_) delete slab;
    for (Slab* slab : slabs_32_) delete slab;
    for (Slab* slab : slabs_64_) delete slab;
    for (Slab* slab : slabs_128_) delete slab;
    for (Slab* slab : slabs_256_) delete slab;
    for (Slab* slab : slabs_512_) delete slab;
    for (Slab* slab : slabs_1024_) delete slab;
}

void* PoolAllocator::allocate(std::size_t size) {
    if (__builtin_expect(size > kMaxPoolSize, 0)) {
        return malloc(size);
    }
    
    if (__builtin_expect(size <= 8, 1)) {
        if (__builtin_expect(last_used_slab_8_ && last_used_slab_8_->has_free_chunks(), 1)) {
            return last_used_slab_8_->allocate();
        }
        Slab* slab = getOrCreateSlab(slabs_8_, 8);
        last_used_slab_8_ = slab;
        return slab->allocate();
    }
    if (size <= 16) {
        if (last_used_slab_16_ && last_used_slab_16_->has_free_chunks()) {
            return last_used_slab_16_->allocate();
        }
        Slab* slab = getOrCreateSlab(slabs_16_, 16);
        last_used_slab_16_ = slab;
        return slab->allocate();
    }
    if (size <= 32) {
        if (last_used_slab_32_ && last_used_slab_32_->has_free_chunks()) {
            return last_used_slab_32_->allocate();
        }
        Slab* slab = getOrCreateSlab(slabs_32_, 32);
        last_used_slab_32_ = slab;
        return slab->allocate();
    }
    if (size <= 64) {
        if (last_used_slab_64_ && last_used_slab_64_->has_free_chunks()) {
            return last_used_slab_64_->allocate();
        }
        Slab* slab = getOrCreateSlab(slabs_64_, 64);
        last_used_slab_64_ = slab;
        return slab->allocate();
    }
    
    if (size <= 128) {
        Slab* slab = getOrCreateSlab(slabs_128_, 128);
        return slab->allocate();
    }
    if (size <= 256) {
        Slab* slab = getOrCreateSlab(slabs_256_, 256);
        return slab->allocate();
    }
    if (size <= 512) {
        Slab* slab = getOrCreateSlab(slabs_512_, 512);
        return slab->allocate();
    }
    if (size <= 1024) {
        Slab* slab = getOrCreateSlab(slabs_1024_, 1024);
        return slab->allocate();
    }
    
    return malloc(size);
}

void PoolAllocator::deallocate(void* ptr) {
    Slab* slab = findSlabForPointer(ptr);
    if (slab) {
        slab->deallocate(ptr);
    } else {
        free(ptr);
    }
}

void PoolAllocator::reset() {
    for (Slab* slab : slabs_8_) delete slab;
    for (Slab* slab : slabs_16_) delete slab;
    for (Slab* slab : slabs_32_) delete slab;
    for (Slab* slab : slabs_64_) delete slab;
    for (Slab* slab : slabs_128_) delete slab;
    for (Slab* slab : slabs_256_) delete slab;
    for (Slab* slab : slabs_512_) delete slab;
    for (Slab* slab : slabs_1024_) delete slab;
    
    slabs_8_.clear();
    slabs_16_.clear();
    slabs_32_.clear();
    slabs_64_.clear();
    slabs_128_.clear();
    slabs_256_.clear();
    slabs_512_.clear();
    slabs_1024_.clear();
    
    last_used_slab_8_ = nullptr;
    last_used_slab_16_ = nullptr;
    last_used_slab_32_ = nullptr;
    last_used_slab_64_ = nullptr;
    
    slabs_8_.push_back(new Slab(8));
    slabs_16_.push_back(new Slab(16));
    slabs_32_.push_back(new Slab(32));
    slabs_64_.push_back(new Slab(64));
    slabs_128_.push_back(new Slab(128));
    slabs_256_.push_back(new Slab(256));
    slabs_512_.push_back(new Slab(512));
    slabs_1024_.push_back(new Slab(1024));
}

Slab* PoolAllocator::getOrCreateSlab(std::vector<Slab*>& slabs, std::size_t chunk_size) {
    for (Slab* slab : slabs) {
        if (slab->has_free_chunks()) {
            return slab;
        }
    }
    
    Slab* new_slab = new Slab(chunk_size);
    slabs.push_back(new_slab);
    return new_slab;
}

Slab* PoolAllocator::findSlabForPointer(void* ptr) {
    for (Slab* slab : slabs_8_) {
        if (slab->contains(ptr)) return slab;
    }
    for (Slab* slab : slabs_16_) {
        if (slab->contains(ptr)) return slab;
    }
    for (Slab* slab : slabs_32_) {
        if (slab->contains(ptr)) return slab;
    }
    for (Slab* slab : slabs_64_) {
        if (slab->contains(ptr)) return slab;
    }
    
    for (Slab* slab : slabs_128_) {
        if (slab->contains(ptr)) return slab;
    }
    for (Slab* slab : slabs_256_) {
        if (slab->contains(ptr)) return slab;
    }
    for (Slab* slab : slabs_512_) {
        if (slab->contains(ptr)) return slab;
    }
    for (Slab* slab : slabs_1024_) {
        if (slab->contains(ptr)) return slab;
    }
    return nullptr;
}

} // namespace slab
