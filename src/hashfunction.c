#ifdef __cplusplus
# include <cstdint>
#else /* ifdef __cplusplus */
# include <stdint.h>
#endif /* ifdef __cplusplus */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#if defined(SHOOTOUT_FASTHASH)

uint32_t HashTableFastHash(uint8_t const* data, uint32_t data_length) {
  uint32_t hash = data_length;
  uint32_t tmp;

  if ((data_length <= 0) || (data == NULL)) {
    return 0;
  }

  uint32_t const remaining = data_length & 3;
  data_length >>= 2;

  /* Main loop */
  for (; data_length > 0; --data_length) {
    hash += *((uint16_t const*) (data));
    tmp   = (*((uint16_t const*) (data + 2)) << 11) ^ hash;
    hash  = (hash << 16) ^ tmp;
    data += 2 * sizeof(uint16_t);
    hash += hash >> 11;
  }

  /* Handle end cases */
  switch (remaining) {
    case 3:
      hash += *((uint16_t const*) (data));
      hash ^= hash << 16;
      hash ^= ((int8_t) data[sizeof(uint16_t)]) << 18;
      hash += hash >> 11;
      break;

    case 2:
      hash += *((uint16_t const*) (data));
      hash ^= hash << 11;
      hash += hash >> 17;
      break;

    case 1:
      hash += (int8_t) *data;
      hash ^= hash << 10;
      hash += hash >> 1;
  }

  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;

  return hash;
} /* HashTableFastHash */

# define SHOOTOUT_OUTPUT "hashfct-fast"
# define SHOOTOUT_HASH(result_m, value_m) result_m = HashTableFastHash((uint8_t*) &value_m, sizeof(value_m))
# define SHOOTOUT_RESULT_TYPE uint32_t

#elif defined(SHOOTOUT_MODULO)

# define SHOOTOUT_OUTPUT "hashfct-modulo"
# define SHOOTOUT_HASH(result_m, value_m) result_m = value_m % 1048573
# define SHOOTOUT_RESULT_TYPE uint32_t

#else /* if defined(SHOOTOUT_GLIB) */

# define SHOOTOUT_OUTPUT "hashfct-empty"
# define SHOOTOUT_HASH(result_m, value_m) do { result_m = value_m; } while (0)
# define SHOOTOUT_RESULT_TYPE uint32_t

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
  uint64_t              count        = atoi(argv[1]);
  uint64_t*             insert_ticks = (uint64_t*) malloc((count + 1) * sizeof(uint64_t));
  uint64_t*             remove_ticks = (uint64_t*) malloc((count + 1) * sizeof(uint64_t));
  uint64_t*             values       = (uint64_t*) malloc((count + 1) * sizeof(uint64_t));
  SHOOTOUT_RESULT_TYPE* output       = (SHOOTOUT_RESULT_TYPE*) malloc((count + 1) * sizeof(SHOOTOUT_RESULT_TYPE));

  size_t i = 0;

  for (i = 0; i < count; ++i) {
    values[i] = i;
  }

  FILE* results = fopen("results/" SHOOTOUT_OUTPUT, "w");

  uint64_t total = 0;
  srandom(1);

  uint64_t before = get_ticks();
  for (i = 0; i < count; ++i) {
    insert_ticks[i] = get_ticks();
    SHOOTOUT_HASH(output[i], values[i]);
  }
  insert_ticks[i] = get_ticks();

  uint64_t after = get_ticks();

  total += after - before;
  fprintf(results, "total_alloc %lf\n", ticks_to_ns(after - before));
  for (uint64_t i = 0; i < count; ++i) {
    fprintf(results, "alloc %lu %lf\n", i, ticks_to_ns(insert_ticks[i + 1] - insert_ticks[i]));
  }

  for (uint64_t i = 0; i < count; ++i) {
    fprintf(results, "result %lu\n", i, output);
  }

  free(output);
  free(values);
  free(insert_ticks);
  free(remove_ticks);

  fclose(results);

  printf("%lf\n", ticks_to_ns(total));
  sleep(5);

  return 0;
} /* main */
