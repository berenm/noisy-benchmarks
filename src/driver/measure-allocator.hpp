#ifndef __SHOOTOUT_MEASURE_ALLOCATOR_HPP__
#define __SHOOTOUT_MEASURE_ALLOCATOR_HPP__

#include <cstdlib>

namespace shootout {
  namespace test {

    struct alloc_data {
      void*       pointer;
      std::size_t size;
    };

    void alloc(alloc_data* alloc_data);
    void dealloc(alloc_data const* alloc_data);

  }
}

#endif // ifndef __SHOOTOUT_MEASURE_ALLOCATOR_HPP__
