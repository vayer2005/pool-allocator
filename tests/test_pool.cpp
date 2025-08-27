#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "Slab.hpp"
#include "PoolAllocator.hpp"
#include <vector>
#include <random>

TEST_CASE("Single-slab basic allocate/free", "[slab]") {
    constexpr std::size_t kChunk = 64;
    slab::Slab slab{kChunk};

    void* p1 = slab.allocate();
    REQUIRE(p1 != nullptr);

    slab.deallocate(p1);
    REQUIRE(slab.empty());
}

TEST_CASE("Slab multiple allocations", "[slab]") {
    constexpr std::size_t kChunk = 32;
    slab::Slab slab{kChunk};

    std::vector<void*> ptrs;
    
    // Allocate multiple chunks
    for (int i = 0; i < 10; ++i) {
        void* ptr = slab.allocate();
        REQUIRE(ptr != nullptr);
        ptrs.push_back(ptr);
    }

    // Deallocate all
    for (void* ptr : ptrs) {
        slab.deallocate(ptr);
    }

    REQUIRE(slab.empty());
}

TEST_CASE("Slab exhaustion and reuse", "[slab]") {
    constexpr std::size_t kChunk = 16;
    slab::Slab slab{kChunk};

    std::vector<void*> ptrs;
    
    // Allocate until exhaustion
    while (void* ptr = slab.allocate()) {
        ptrs.push_back(ptr);
    }

    REQUIRE(ptrs.size() > 0);
    REQUIRE(slab.allocate() == nullptr);  // Should be exhausted

    // Free all and verify we can allocate again
    for (void* ptr : ptrs) {
        slab.deallocate(ptr);
    }

    REQUIRE(slab.empty());
    REQUIRE(slab.allocate() != nullptr);  // Should work again
}



TEST_CASE("PoolAllocator basic functionality", "[pool_allocator]") {
    slab::PoolAllocator allocator;

    // Test different sizes
    void* p1 = allocator.allocate(8);
    void* p2 = allocator.allocate(16);
    void* p3 = allocator.allocate(32);
    void* p4 = allocator.allocate(64);

    REQUIRE(p1 != nullptr);
    REQUIRE(p2 != nullptr);
    REQUIRE(p3 != nullptr);
    REQUIRE(p4 != nullptr);

    allocator.deallocate(p1);
    allocator.deallocate(p2);
    allocator.deallocate(p3);
    allocator.deallocate(p4);
}

TEST_CASE("PoolAllocator large allocations", "[pool_allocator]") {
    slab::PoolAllocator allocator;

    // Test allocation larger than max pool size
    void* large_ptr = allocator.allocate(2048);
    REQUIRE(large_ptr != nullptr);
    allocator.deallocate(large_ptr);
}

TEST_CASE("PoolAllocator stress test", "[pool_allocator]") {
    slab::PoolAllocator allocator;
    std::vector<void*> ptrs;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> size_dist(1, 1000);

    // Allocate many different sizes
    for (int i = 0; i < 1000; ++i) {
        std::size_t size = size_dist(gen);
        void* ptr = allocator.allocate(size);
        REQUIRE(ptr != nullptr);
        ptrs.push_back(ptr);
    }

    // Deallocate all
    for (void* ptr : ptrs) {
        allocator.deallocate(ptr);
    }
}

TEST_CASE("PoolAllocator reset functionality", "[pool_allocator]") {
    slab::PoolAllocator allocator;

    // Allocate some memory
    void* p1 = allocator.allocate(64);
    void* p2 = allocator.allocate(128);
    REQUIRE(p1 != nullptr);
    REQUIRE(p2 != nullptr);

    // Reset should clear all pools
    allocator.reset();

    // Allocate again after reset
    void* p3 = allocator.allocate(64);
    void* p4 = allocator.allocate(128);
    REQUIRE(p3 != nullptr);
    REQUIRE(p4 != nullptr);

    allocator.deallocate(p3);
    allocator.deallocate(p4);
} 