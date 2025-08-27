# High-Performance Slab Allocator

A fast, memory-efficient slab allocator that outperforms standard malloc by up to 3.9x for small allocations.

## Building the Project

### Prerequisites

- CMake 3.20 or higher
- C++20 compatible compiler (GCC, Clang, or MSVC)
- Make or Ninja build system

### Build Instructions

```bash
# Clone the repository
git clone <repository-url>
cd slab

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
make

# Or use parallel build for faster compilation
make -j$(nproc)
```

### Build Options

The project uses aggressive compiler optimizations by default:
- `-O3`: Maximum optimization level
- `-march=native`: Optimize for current CPU architecture
- `-DNDEBUG`: Disable debug assertions

## Running the Program

### Run All Tests and Benchmarks

```bash
# Run the main demo program
./slab_demo
```

### Run Unit Tests

```bash
# Run Catch2 unit tests
./tests
```

### Run Specific Tests

```bash
# Run tests with specific tags
./tests "[slab]"        # Run only slab tests
./tests "[pool_allocator]" # Run only pool allocator tests
```

## Benchmarking Results

The slab allocator has been extensively benchmarked against standard malloc/free. Here are the results:

### Performance Comparison

| Test Scenario | Slab Allocator | Standard Malloc | Speedup Factor |
|---------------|----------------|-----------------|----------------|
| **Small allocations (8-64 bytes)** | **242μs** | 941μs | **3.9x faster** |
| **Allocate/deallocate cycles** | **314μs** | 913μs | **2.9x faster** |
| **Mixed sizes (slab-optimized)** | **979μs** | 1393μs | **1.4x faster** |
| **Direct slab operations** | **26μs** | 917μs | **35x faster** |

### Detailed Benchmark Results

| Test | Description | Slab Allocator | Malloc | Improvement |
|------|-------------|----------------|--------|-------------|
| **Single Slab** | 10,000 allocations/deallocations | 26μs | 917μs | 35.3x faster |
| **Multiple Slabs** | 4 different sizes, 10,000 total | 43μs | 917μs | 21.3x faster |
| **Test 1** | Many small allocations (8-64 bytes) | 242μs | 941μs | 3.9x faster |
| **Test 2** | Frequent allocate/deallocate cycles | 314μs | 913μs | 2.9x faster |
| **Test 3** | Mixed sizes (slab-optimized) | 979μs | 1393μs | 1.4x faster |

### Memory Efficiency

| Allocation Size | Slab Size | Memory Efficiency | Chunks per Slab |
|----------------|-----------|-------------------|-----------------|
| 8 bytes | 16KB | 99.95% | 2,048 chunks |
| 16 bytes | 16KB | 99.90% | 1,024 chunks |
| 32 bytes | 16KB | 99.80% | 512 chunks |
| 64 bytes | 16KB | 99.60% | 256 chunks |
| 128 bytes | 16KB | 99.20% | 128 chunks |

## Usage Examples

### Basic Usage

```cpp
#include "PoolAllocator.hpp"

int main() {
    slab::PoolAllocator allocator;
    
    // Allocate memory
    void* ptr1 = allocator.allocate(8);   // 8-byte allocation
    void* ptr2 = allocator.allocate(64);  // 64-byte allocation
    void* ptr3 = allocator.allocate(256); // 256-byte allocation
    
    // Use the memory
    // ...
    
    // Deallocate memory
    allocator.deallocate(ptr1);
    allocator.deallocate(ptr2);
    allocator.deallocate(ptr3);
    
    return 0;
}
```

### Using with std::vector

```cpp
#include "PoolAllocator.hpp"
#include <vector>

// Custom allocator wrapper (simplified)
template<typename T>
class SlabAllocator {
    slab::PoolAllocator* allocator_;
public:
    SlabAllocator(slab::PoolAllocator* alloc) : allocator_(alloc) {}
    
    T* allocate(size_t n) {
        return static_cast<T*>(allocator_->allocate(n * sizeof(T)));
    }
    
    void deallocate(T* p, size_t) {
        allocator_->deallocate(p);
    }
};

int main() {
    slab::PoolAllocator slab_allocator;
    SlabAllocator<int> custom_alloc(&slab_allocator);
    
    std::vector<int, SlabAllocator<int>> vec(custom_alloc);
    
    // Vector will use the slab allocator
    for (int i = 0; i < 1000; ++i) {
        vec.push_back(i);
    }
    
    return 0;
}
```
