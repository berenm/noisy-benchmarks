cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(AddBenchmark)

project(noisy CXX C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -O3 -Wl,-O1 -flto")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3 -Wl,-O1 -flto")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -g -fwhole-program -Wl,-O1 -flto")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

find_package(Boost COMPONENTS program_options filesystem system regex thread chrono REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2     glib-2.0)

include_directories(${Boost_INCLUDE_DIRS} ${GLIB2_INCLUDE_DIRS} include src)
link_directories(${Boost_LIBRARY_DIRS} ${GLIB2_LIBRARY_DIRS})


# -------------------------------------------------------------------------
# noisy
file(GLOB_RECURSE NOISY_COMMON_SOURCES src/common/*.cpp)
add_library(noisy-common STATIC ${NOISY_COMMON_SOURCES})
target_link_libraries(noisy-common rt)


# add_benchmark(call
#   NAME  "Function Call"
#   TESTS "empty:Empty function"
#   PLOTS ns-per-iteration
# )

# add_benchmark(allocator
#   NAME  "Memory Allocation"
#   TESTS empty    "Empty functions"
#         malloc   "malloc/free allocator"
#         new      "new/delete allocator"
#         gslice   "GLib2.0 Slice allocator"
#         jemalloc "Facebook's JEMalloc allocator"
#         tcmalloc "Google's TCMalloc allocator"
#   PLOTS ns-per-iteration
# )

add_benchmark(call empty)
add_plots(call empty ns-per-iteration)

add_benchmark(trycatch empty)
add_plots(trycatch empty ns-per-iteration STEPS no-throw no-throw-global-try no-throw-local-try throw-local-try throw-half-local-try)

add_benchmark(trycatch throwing)
add_plots(trycatch throwing ns-per-iteration STEPS no-throw no-throw-global-try no-throw-local-try throw-local-try throw-half-local-try)

add_benchmark(trycatch trying)
add_plots(trycatch trying ns-per-iteration STEPS no-throw no-throw-global-try no-throw-local-try throw-local-try throw-half-local-try)


add_benchmark(allocator empty)
add_plots(allocator empty ns-per-iteration SCENARIOS 16 32 128 256 random STEPS alloc dealloc)

add_benchmark(allocator malloc)
add_plots(allocator malloc ns-per-iteration SCENARIOS 16 32 128 256 random STEPS alloc dealloc)

add_benchmark(allocator new)
add_plots(allocator new ns-per-iteration SCENARIOS 16 32 128 256 random STEPS alloc dealloc)

add_benchmark(allocator gslice LIBRARIES ${GLIB2_LIBRARIES})
add_plots(allocator gslice ns-per-iteration SCENARIOS 16 32 128 256 random STEPS alloc dealloc)

add_benchmark(allocator jemalloc LIBRARIES jemalloc)
add_plots(allocator jemalloc ns-per-iteration SCENARIOS 16 32 128 256 random STEPS alloc dealloc)

add_benchmark(allocator tcmalloc LIBRARIES :libtcmalloc.so.4)
add_plots(allocator tcmalloc ns-per-iteration SCENARIOS 16 32 128 256 random STEPS alloc dealloc)

add_benchmark(allocator pool LIBRARIES :libtcmalloc.so.4)
add_plots(allocator pool ns-per-iteration SCENARIOS 16 32 128 256 random STEPS alloc dealloc)




add_benchmark(hashtable empty)
add_plots(hashtable empty ns-per-iteration STEPS insert remove)

# include_directories(bplus-tree-v2/include bplus-tree-v2/src)
# add_library(bplus-tree STATIC bplus-tree-v2/test/bplus_tree_all.c)
# add_benchmark(hashtable bplus LIBRARIES bplus-tree ${GLIB2_LIBRARIES})
# add_plots(hashtable bplus ns-per-iteration STEPS insert remove)

add_benchmark(hashtable brute)
add_plots(hashtable brute ns-per-iteration STEPS insert remove)

add_benchmark(hashtable glib LIBRARIES ${GLIB2_LIBRARIES})
add_plots(hashtable glib ns-per-iteration STEPS insert remove)

add_benchmark(hashtable sparse)
add_plots(hashtable sparse ns-per-iteration STEPS insert remove)


add_custom_target(plot-hashtable-comparison
  COMMAND gnuplot ${PROJECT_SOURCE_DIR}/src/plot/hashtable-comparison.plot
)
add_dependencies(plot-hashtable-comparison data-hashtable-bplus data-hashtable-brute data-hashtable-glib data-hashtable-sparse)
add_dependencies(all-plot plot-hashtable-comparison)

# build/hash-glib: build
#   $(CC) src/hashtable.c -o build/hash-glib -Dnoisy_GLIB $(OPTIMIZE) $(LINK) -DG_DISABLE_CHECKS -DG_DISABLE_ASSERT -std=gnu99 -D_NDEBUG `pkg-config --cflags --libs glib-2.0`

# build/hash-sparse: build
#   $(CXX) src/hashtable.c -std=c++11 -x c++ -o build/hash-sparse -Dnoisy_SPARSE $(OPTIMIZE) $(LINK) -D_NDEBUG

# build/hash-bplus: build
#   $(CC) src/hashtable.c -o build/hash-bplus -Dnoisy_BPLUS $(OPTIMIZE) $(LINK) -DG_DISABLE_CHECKS -DG_DISABLE_ASSERT -std=gnu99 -D_NDEBUG `pkg-config --cflags --libs glib-2.0` -Ibplus-tree-v2/src -Ibplus-tree-v2/include

# build/hash-brute: build
#   $(CC) src/hashtable.c -o build/hash-brute -Dnoisy_BRUTE $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

# build/hash-empty: build
#   $(CC) src/hashtable.c -o build/hash-empty $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG


# build/hashfct-fast: build
#   $(CC) src/hashfunction.c -o build/hashfct-fast -Dnoisy_FASTHASH $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

# build/hashfct-modulo: build
#   $(CC) src/hashfunction.c -o build/hashfct-modulo -Dnoisy_MODULO $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

# build/hashfct-empty: build
#   $(CC) src/hashfunction.c -o build/hashfct-empty $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG


# build/buffer-ring: build
#   $(CC) src/ringbuffer.c -o build/buffer-ring -Dnoisy_RINGBUFFER $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

# build/buffer-boost: build
#   $(CXX) src/ringbuffer.c -std=c++11 -x c++ -o build/buffer-boost -Dnoisy_BOOSTRING $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

# build/buffer-empty: build
#   $(CC) src/ringbuffer.c -o build/buffer-empty $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG
