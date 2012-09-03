#include "driver/measure-alloc.hpp"

namespace shootout {
  namespace test {

    void alloc(alloc_data* alloc_data) {
      alloc_data->pointer = reinterpret_cast< void* >(alloc_data->size);
    }

    void dealloc(alloc_data const* alloc_data) {}

  }
}
