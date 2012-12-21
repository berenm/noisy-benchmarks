#include "driver/measure-allocator.hpp"

#define JEMALLOC_NO_DEMANGLE
#include <jemalloc/jemalloc.h>

namespace shootout {
  namespace test {

    void alloc(alloc_data* alloc_data) {
      alloc_data->pointer = je_malloc(alloc_data->size);
    }

    void dealloc(alloc_data const* alloc_data) {
      je_free(alloc_data->pointer);
    }

  }
}
