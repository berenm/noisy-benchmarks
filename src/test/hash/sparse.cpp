#include "driver/measure-hash.hpp"

#include <google/sparse_hash_map>

typedef google::sparse_hash_map< std::uint64_t, void* > hash_t;

namespace shootout {
  namespace test {

    static hash_t hash(100000);

    void init() {
      hash.set_deleted_key(static_cast< std::uint64_t >(-1));
    }

    void insert(std::uint64_t key, void* value) {
      hash.insert(hash_t::value_type(key, value));
    }

    void remove(std::uint64_t key, void* value) {
      hash.erase(key);
    }

  }
}
