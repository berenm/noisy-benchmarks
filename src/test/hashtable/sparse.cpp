#include "driver/measure-hashtable.hpp"

#include <google/sparse_hash_map>

typedef google::sparse_hash_map< std::uint64_t, void* > hash_t;

namespace shootout {
  namespace test {

    static hash_t hash(100000);

    void init() {
      hash.set_deleted_key(static_cast< std::uint64_t >(-1));
    }

    void insert(hash_data* data) {
      hash.insert(hash_t::value_type(data->key, data->value));
    }

    void remove(hash_data const* data) {
      hash.erase(data->key);
    }

  }
}
