#include "driver/measure-allocator.hpp"

namespace shootout {
  namespace test {

    void alloc(alloc_data* alloc_data) {
      alloc_data->pointer = malloc(alloc_data->size);
    }

    void dealloc(alloc_data const* alloc_data) {
      free(alloc_data->pointer);
    }

  }
}
