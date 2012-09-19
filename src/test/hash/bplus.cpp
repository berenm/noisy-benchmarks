#include "driver/measure-hash.hpp"

#include "../bplus-tree-v2/test/bplus_tree_all.c"

namespace shootout {
  namespace test {

    static BplusTree* hash = bplus_tree_new(true);

    void init() {}
    void insert(std::uint64_t key, void* value) { bplus_tree_insert(hash, reinterpret_cast< BplusKey >(key), value); }
    void remove(std::uint64_t key, void* value) { bplus_tree_remove(hash, reinterpret_cast< BplusKey >(key)); }

  }
}
