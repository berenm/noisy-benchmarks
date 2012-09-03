#ifdef __cplusplus
# include <cstdint>
#else /* ifdef __cplusplus */
# include <stdint.h>
#endif /* ifdef __cplusplus */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#if defined(SHOOTOUT_GLIB)

# include <glib.h>

# define SHOOTOUT_OUTPUT "hash-glib"
# define SHOOTOUT_CREATE(hash_m)                 GHashTable * hash = g_hash_table_new(g_direct_hash, g_direct_equal)
# define SHOOTOUT_INSERT(hash_m, key_m, value_m) g_hash_table_insert(hash_m, GINT_TO_POINTER(key_m), value_m)
# define SHOOTOUT_REMOVE(hash_m, key_m)          g_hash_table_remove(hash_m, GINT_TO_POINTER(key_m))
# define SHOOTOUT_VALUE_TYPE uint64_t
# define SHOOTOUT_INIT_VALUE(values_m, index_m) values_m[index_m] = random()

#elif defined(SHOOTOUT_SPARSE)

# include <google/sparse_hash_map>

typedef google::sparse_hash_map< uint64_t, void* > hash_t;

# define SHOOTOUT_OUTPUT "hash-sparse"
# define SHOOTOUT_CREATE(hash_m)                 hash_t hash_m(100000); hash_m.set_deleted_key(-1)
# define SHOOTOUT_INSERT(hash_m, key_m, value_m) hash_m.insert(hash_t::value_type(key_m, value_m))
# define SHOOTOUT_REMOVE(hash_m, key_m)          hash_m.erase(key_m)
# define SHOOTOUT_VALUE_TYPE uint64_t
# define SHOOTOUT_INIT_VALUE(values_m, index_m) values_m[index_m] = random()

#elif defined(SHOOTOUT_BPLUS)

# include "../bplus-tree-v2/test/bplus_tree_all.c"

# define SHOOTOUT_OUTPUT "hash-bplus"
# define SHOOTOUT_CREATE(hash_m)                 BplusTree * hash = BplusTreeNew()
# define SHOOTOUT_INSERT(hash_m, key_m, value_m) BplusTreeInsert(hash_m, key_m, value_m)
# define SHOOTOUT_REMOVE(hash_m, key_m)          BplusTreeRemove(hash_m, key_m)
# define SHOOTOUT_VALUE_TYPE uint64_t
# define SHOOTOUT_INIT_VALUE(values_m, index_m) values_m[index_m] = random()

#elif defined(SHOOTOUT_BRUTE)

# include "test/hash_table.h"
# include "test/hash_table.c"

# define SHOOTOUT_OUTPUT "hash-brute"
# define SHOOTOUT_CREATE(hash_m)                 HashTable * hash = HashTableCreate();
# define SHOOTOUT_INSERT(hash_m, key_m, value_m) HashTableInsert(hash_m, key_m, value_m)
# define SHOOTOUT_REMOVE(hash_m, key_m)          (void) HashTableRemove(hash_m, key_m)
# define SHOOTOUT_VALUE_TYPE HashLink
# define SHOOTOUT_INIT_VALUE(values_m, index_m) values_m[index_m].value = (void*) random()

#else /* if defined(SHOOTOUT_GLIB) */

# define SHOOTOUT_OUTPUT "hash-empty"
# define SHOOTOUT_CREATE(hash_m)                 do {} while (0)
# define SHOOTOUT_INSERT(hash_m, key_m, value_m) do { (void) (key_m); } while (0)
# define SHOOTOUT_REMOVE(hash_m, key_m)          do { (void) (key_m); } while (0)
# define SHOOTOUT_VALUE_TYPE uint64_t
# define SHOOTOUT_INIT_VALUE(values_m, index_m) do {} while (0)

#endif /* if defined(SHOOTOUT_GLIB) */

__inline__ uint64_t get_ticks(void) {
  uint32_t lo, hi;

  __asm__ __volatile__ (
    "        xorl %%eax,%%eax \n"
    "        cpuid" // serialize
    ::: "%rax", "%rbx", "%rcx", "%rdx");

  /* We cannot use "=A", since this would use %rax on x86_64 and return only the lower 32bits of the TSC */
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return (uint64_t) hi << 32 | lo;
}

static double ticks_to_ns(uint64_t ticks) {
  return ((double) ticks) / 2.26;
}

int main(int argc, char const* argv[]) {
  uint64_t             count        = atoi(argv[1]);
  uint64_t*            insert_ticks = (uint64_t*) malloc((count + 1) * sizeof(uint64_t));
  uint64_t*            remove_ticks = (uint64_t*) malloc((count + 1) * sizeof(uint64_t));
  SHOOTOUT_VALUE_TYPE* values       = (SHOOTOUT_VALUE_TYPE*) malloc((count + 1) * sizeof(SHOOTOUT_VALUE_TYPE));

  size_t i = 0;

  for (i = 0; i < count; ++i) {
    SHOOTOUT_INIT_VALUE(values, i);
  }

  SHOOTOUT_CREATE(hash);
  FILE* results = fopen("results/" SHOOTOUT_OUTPUT, "w");

  uint64_t total = 0;
  srandom(1);

  uint64_t before = get_ticks();
  for (i = 0; i < count; ++i) {
    insert_ticks[i] = get_ticks();
    SHOOTOUT_INSERT(hash, i, values + i);
  }
  insert_ticks[i] = get_ticks();

  uint64_t after = get_ticks();

  total += after - before;
  fprintf(results, "total_alloc %lf\n", ticks_to_ns(after - before));
  for (uint64_t i = 0; i < count; ++i) {
    fprintf(results, "alloc %lu %lf\n", i, ticks_to_ns(insert_ticks[i + 1] - insert_ticks[i]));
  }

  srandom(1);
  before = get_ticks();
  for (i = 0; i < count; ++i) {
    remove_ticks[i] = get_ticks();
    SHOOTOUT_REMOVE(hash, i);
  }
  remove_ticks[i] = get_ticks();
  after           = get_ticks();

  total += after - before;
  fprintf(results, "total_dealloc %lf\n", ticks_to_ns(after - before));
  for (uint64_t i = 0; i < count; ++i) {
    fprintf(results, "dealloc %lu %lf\n", i, ticks_to_ns(remove_ticks[i + 1] - remove_ticks[i]));
  }

  free(insert_ticks);
  free(remove_ticks);

  fclose(results);

  printf("%lf\n", ticks_to_ns(total));
  sleep(5);

  return 0;
} /* main */
