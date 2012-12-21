#include "driver/measure-allocator.hpp"

extern "C" void* tc_malloc(size_t size);
extern "C" void  tc_free(void*);

namespace shootout {
  namespace test {

    void alloc(alloc_data* alloc_data) {
      alloc_data->pointer = tc_malloc(alloc_data->size);
    }

    void dealloc(alloc_data const* alloc_data) {
      tc_free(alloc_data->pointer);
    }

  }
}
