#include "driver/measure-alloc.hpp"

namespace shootout {
  namespace test {

    void alloc(alloc_data* alloc_data) {
      alloc_data->pointer = new char[alloc_data->size];
    }

    void dealloc(alloc_data const* alloc_data) {
      delete[] reinterpret_cast< char const* >(alloc_data->pointer);
    }

  }
}
