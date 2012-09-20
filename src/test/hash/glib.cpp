#include "driver/measure-hash.hpp"

#include <glib.h>

namespace shootout {
  namespace test {

    static GHashTable* hash = g_hash_table_new(g_direct_hash, g_direct_equal);

    void init() {}
    void insert(hash_data* data) { g_hash_table_insert(hash, GINT_TO_POINTER(data->key), data->value); }
    void remove(hash_data const* data) { g_hash_table_remove(hash, GINT_TO_POINTER(data->key)); }

  }
}
