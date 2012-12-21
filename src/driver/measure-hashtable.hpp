#ifndef __SHOOTOUT_MEASURE_HASHTABLE_HPP__
#define __SHOOTOUT_MEASURE_HASHTABLE_HPP__

#include <cstdlib>
#include <cstdint>

namespace shootout {
  namespace test {

    struct hash_data {
      hash_data*    hash_next;
      std::uint64_t key;
      char          value[256];
    };

    void init();
    void insert(hash_data* data);
    void remove(hash_data const* data);

  }
}

#endif // ifndef __SHOOTOUT_MEASURE_HASHTABLE_HPP__
