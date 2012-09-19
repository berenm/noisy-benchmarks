#ifndef __SHOOTOUT_MEASURE_HASH_HPP__
#define __SHOOTOUT_MEASURE_HASH_HPP__

#include <cstdlib>
#include <cstdint>

namespace shootout {
  namespace test {

    struct hash_data {
      std::uint64_t key;
      char          value[256];
    };

    void init();
    void insert(std::uint64_t key, void* value);
    void remove(std::uint64_t key, void* value);

  }
}

#endif // ifndef __SHOOTOUT_MEASURE_HASH_HPP__
