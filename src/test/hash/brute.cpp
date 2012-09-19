#include "driver/measure-hash.hpp"

#include <stdint.h>

#include "test/hash_table.h"
#include "test/hash_table.c"

namespace shootout {
  namespace test {

    static HashTable* hash = HashTableCreate();

    void init() {}

    void insert(std::uint64_t key, void* value) {
      HashLink* link = reinterpret_cast< HashLink* >(value);

      link->key.data[0]     = key;
      link->key.data_length = 1;
      HashTableInsert(hash, link);
    }

    void remove(std::uint64_t key, void* value) {
      HashLink* link = reinterpret_cast< HashLink* >(value);

      HashTableRemove(hash, link);
    }

  }
}
