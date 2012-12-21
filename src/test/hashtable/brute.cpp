#include "driver/measure-hashtable.hpp"

#include <stdint.h>

#include "test/hash_table.h"
#include "test/hash_table.c"

namespace shootout {
  namespace test {

    static HashTable* hash = HashTableCreate();

    void init() {}

    void insert(hash_data* data) {
      HashLink* link = reinterpret_cast< HashLink* >(data);

      HashTableInsert(hash, link);
    }

    void remove(hash_data const* data) {
      HashLink const* link = reinterpret_cast< HashLink const* >(data);

      HashTableRemove(hash, const_cast< HashLink* >(link));
    }

  }
}
