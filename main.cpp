#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "Slab.hpp"
#include "PoolAllocator.hpp"

void testSlab() {
    std::cout << "=== Testing Slab Allocator ===" << std::endl;
    
    slab::Slab slab(64);
    
    std::cout << "Allocating 5 chunks..." << std::endl;
    std::vector<void*> ptrs;
    for (int i = 0; i < 5; ++i) {
        void* ptr = slab.allocate();
        if (ptr) {
            std::cout << "  Allocated chunk " << i << " at " << ptr << std::endl;
            ptrs.push_back(ptr);
        } else {
            std::cout << "  Failed to allocate chunk " << i << std::endl;
        }
    }
    
    std::cout << "Deallocating all chunks..." << std::endl;
    for (void* ptr : ptrs) {
        slab.deallocate(ptr);
        std::cout << "  Deallocated chunk at " << ptr << std::endl;
    }
    
    std::cout << "Slab empty: " << (slab.empty() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
}



void testPoolAllocator() {
    std::cout << "=== Testing Pool Allocator (Multi-size) ===" << std::endl;
    
    slab::PoolAllocator allocator;
    
    std::cout << "Testing different allocation sizes..." << std::endl;
    std::vector<std::pair<void*, std::size_t>> allocations;
    
    std::vector<std::size_t> sizes = {8, 16, 32, 64, 128, 256, 512, 1024, 2048};
    
    for (std::size_t size : sizes) {
        void* ptr = allocator.allocate(size);
        if (ptr) {
            std::cout << "  Allocated " << size << " bytes at " << ptr << std::endl;
            allocations.push_back({ptr, size});
        } else {
            std::cout << "  Failed to allocate " << size << " bytes" << std::endl;
        }
    }
    
    std::cout << "Deallocating all..." << std::endl;
    for (auto [ptr, size] : allocations) {
        allocator.deallocate(ptr);
        std::cout << "  Deallocated " << size << " bytes at " << ptr << std::endl;
    }
    
    std::cout << std::endl;
}

void testSlabDirect() {
    std::cout << "=== Testing Slab Direct ===" << std::endl;
    
    // Test 1: Single slab performance
    slab::Slab slab(64);
    std::vector<void*> ptrs;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        void* ptr = slab.allocate();
        if (ptr) ptrs.push_back(ptr);
    }
    
    for (void* ptr : ptrs) {
        slab.deallocate(ptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Single Slab: " << 10000 << " allocations/deallocations in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Test 2: Multiple slabs for different sizes
    std::cout << "\nTest 2: Multiple slabs for different sizes..." << std::endl;
    
    slab::Slab slab8(8);
    slab::Slab slab16(16);
    slab::Slab slab32(32);
    slab::Slab slab64(64);
    
    std::vector<void*> ptrs8, ptrs16, ptrs32, ptrs64;
    
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 2500; ++i) {
        void* ptr8 = slab8.allocate();
        void* ptr16 = slab16.allocate();
        void* ptr32 = slab32.allocate();
        void* ptr64 = slab64.allocate();
        
        if (ptr8) ptrs8.push_back(ptr8);
        if (ptr16) ptrs16.push_back(ptr16);
        if (ptr32) ptrs32.push_back(ptr32);
        if (ptr64) ptrs64.push_back(ptr64);
    }
    
    for (void* ptr : ptrs8) slab8.deallocate(ptr);
    for (void* ptr : ptrs16) slab16.deallocate(ptr);
    for (void* ptr : ptrs32) slab32.deallocate(ptr);
    for (void* ptr : ptrs64) slab64.deallocate(ptr);
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Multiple Slabs: " << 10000 << " allocations/deallocations in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Test 3: Compare with malloc for same pattern
    std::cout << "\nTest 3: Malloc comparison..." << std::endl;
    
    std::vector<void*> malloc_ptrs;
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        std::size_t size = 8 + (i % 4) * 8;  // 8, 16, 24, 32 bytes
        void* ptr = malloc(size);
        malloc_ptrs.push_back(ptr);
    }
    
    for (void* ptr : malloc_ptrs) {
        free(ptr);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Malloc: " << 10000 << " allocations/deallocations in " 
              << duration.count() << " microseconds" << std::endl;
    

}

void performanceTest() {
    std::cout << "=== Performance Test ===" << std::endl;
    
    slab::PoolAllocator slab_allocator;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    const int num_allocations = 10000;
    std::vector<void*> ptrs;
    ptrs.reserve(num_allocations);
    
    // Test 1: Many small allocations of same size (slab allocator's strength)
    std::cout << "Test 1: Many small allocations (8-64 bytes)..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_allocations; ++i) {
        std::size_t size = 8 + (i % 4) * 8;  
        void* ptr = slab_allocator.allocate(size);
        ptrs.push_back(ptr);
    }
    
    for (void* ptr : ptrs) {
        slab_allocator.deallocate(ptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Slab allocator: " << num_allocations << " small allocations in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Test malloc with same pattern
    ptrs.clear();
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_allocations; ++i) {
        std::size_t size = 8 + (i % 4) * 8;
        void* ptr = malloc(size);
        ptrs.push_back(ptr);
    }
    
    for (void* ptr : ptrs) {
        free(ptr);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Standard malloc/free: " << num_allocations << " small allocations in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Test 2: Frequent allocate/deallocate cycles (memory reuse)
    std::cout << "\nTest 2: Frequent allocate/deallocate cycles..." << std::endl;
    
    ptrs.clear();
    start = std::chrono::high_resolution_clock::now();
    
    for (int cycle = 0; cycle < 100; ++cycle) {
        // Allocate a batch
        for (int i = 0; i < 100; ++i) {
            std::size_t size = 16 + (i % 3) * 16;  // 16, 32, 48 bytes
            void* ptr = slab_allocator.allocate(size);
            ptrs.push_back(ptr);
        }
        
        // Deallocate the batch
        for (void* ptr : ptrs) {
            slab_allocator.deallocate(ptr);
        }
        ptrs.clear();
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Slab allocator: " << 10000 << " allocate/deallocate cycles in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Test malloc with same pattern
    ptrs.clear();
    start = std::chrono::high_resolution_clock::now();
    
    for (int cycle = 0; cycle < 100; ++cycle) {
        // Allocate a batch
        for (int i = 0; i < 100; ++i) {
            std::size_t size = 16 + (i % 3) * 16;
            void* ptr = malloc(size);
            ptrs.push_back(ptr);
        }
        
        // Deallocate the batch
        for (void* ptr : ptrs) {
            free(ptr);
        }
        ptrs.clear();
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Standard malloc/free: " << 10000 << " allocate/deallocate cycles in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Test 3: Mixed sizes but optimized for slab strengths
    std::cout << "\nTest 3: Mixed sizes (slab-optimized)..." << std::endl;
    
    ptrs.clear();
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_allocations; ++i) {
        std::size_t size;
        if (i % 10 < 7) {
            size = 8 + (i % 8) * 8; 
        } else if (i % 10 < 9) {
            size = 128 + (i % 4) * 128;  
        } else {
            size = 1024 + (i % 4) * 1024;  
        }
        
        void* ptr = slab_allocator.allocate(size);
        ptrs.push_back(ptr);
    }
    
    for (void* ptr : ptrs) {
        slab_allocator.deallocate(ptr);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Slab allocator: " << num_allocations << " mixed allocations in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Test malloc with same pattern
    ptrs.clear();
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_allocations; ++i) {
        std::size_t size;
        if (i % 10 < 7) {
            size = 8 + (i % 8) * 8;
        } else if (i % 10 < 9) {
            size = 128 + (i % 4) * 128;
        } else {
            size = 1024 + (i % 4) * 1024;
        }
        
        void* ptr = malloc(size);
        ptrs.push_back(ptr);
    }
    
    for (void* ptr : ptrs) {
        free(ptr);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Standard malloc/free: " << num_allocations << " mixed allocations in " 
              << duration.count() << " microseconds" << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "Slab Allocator Manual Test Suite" << std::endl;
    std::cout << "=================================" << std::endl << std::endl;
    
    try {
        testSlab();
        testPoolAllocator();
        testSlabDirect();
        performanceTest();
        
        std::cout << "All tests completed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
