#include "driver/measure-hashtable.hpp"

#include "bplus_tree.h"

namespace shootout {
  namespace test {

    static BplusTree* hash = bplus_tree_new();

    void init() {}
    void insert(hash_data* data) { bplus_tree_insert(hash, reinterpret_cast< BplusKey >(data->key), data->value); }
    void remove(hash_data const* data) { bplus_tree_remove(hash, reinterpret_cast< BplusKey >(data->key)); }

  }
}
