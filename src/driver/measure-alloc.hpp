#ifndef __SHOOTOUT_MEASURE_CALL_HPP__
#define __SHOOTOUT_MEASURE_CALL_HPP__

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

#endif // ifndef __SHOOTOUT_MEASURE_CALL_HPP__
