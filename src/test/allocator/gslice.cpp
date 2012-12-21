#include "driver/measure-allocator.hpp"

#include <glib.h>

namespace shootout {
  namespace test {

    void alloc(alloc_data* alloc_data) {
      alloc_data->pointer = g_slice_alloc(alloc_data->size);
    }

    void dealloc(alloc_data const* alloc_data) {
      g_slice_free1(alloc_data->size, alloc_data->pointer);
    }

  }
}
