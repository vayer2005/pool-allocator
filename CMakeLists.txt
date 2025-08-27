cmake_minimum_required(VERSION 3.20)
project(SlabAllocator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive- /O2)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror -O3 -march=native -DNDEBUG)
endif()

add_library(slab_allocator
    Slab.cpp
    PoolAllocator.cpp
)

target_include_directories(slab_allocator
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

include(FetchContent)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(catch2)

enable_testing()
add_executable(tests
    tests/test_pool.cpp
)

target_link_libraries(tests PRIVATE slab_allocator Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(tests)

# Add main executable
add_executable(slab_demo
    main.cpp
)

target_link_libraries(slab_demo PRIVATE slab_allocator) 