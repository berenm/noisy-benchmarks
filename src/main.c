#ifdef __cplusplus
# include <cstdint>
#else /* ifdef __cplusplus */
# include <stdint.h>
#endif /* ifdef __cplusplus */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#ifndef SHOOTOUT_SIZE
# define SHOOTOUT_SIZE 16
#endif /* ifndef SHOOTOUT_SIZE */

#if defined(SHOOTOUT_GLIB)

# include <glib.h>

# define SHOOTOUT_OUTPUT "gslice"
# define SHOOTOUT_MALLOC(type_m)          g_slice_new(type_m)
# define SHOOTOUT_FREE(type_m, pointer_m) g_slice_free(type_m, pointer_m)

#elif defined(SHOOTOUT_TCMALLOC)

# define SHOOTOUT_OUTPUT "tcmalloc"
# define SHOOTOUT_MALLOC(type_m)          malloc(sizeof(type_m))
# define SHOOTOUT_FREE(type_m, pointer_m) free(pointer_m)

#elif defined(SHOOTOUT_JEMALLOC)

# include <jemalloc/jemalloc.h>

# define SHOOTOUT_OUTPUT "jemalloc"
# define SHOOTOUT_MALLOC(type_m)          malloc(sizeof(type_m))
# define SHOOTOUT_FREE(type_m, pointer_m) free(pointer_m)

#elif defined(SHOOTOUT_EMPTY)

# define SHOOTOUT_OUTPUT "empty"
# define SHOOTOUT_MALLOC(type_m)          0
# define SHOOTOUT_FREE(type_m, pointer_m) do {} while (0);

#else /* if defined(SHOOTOUT_GLIB) */

# define SHOOTOUT_OUTPUT "malloc"
# define SHOOTOUT_MALLOC(type_m)          malloc(sizeof(type_m))
# define SHOOTOUT_FREE(type_m, pointer_m) free(pointer_m)

#endif /* if defined(SHOOTOUT_GLIB) */

typedef struct {
  char data[SHOOTOUT_SIZE];
} AllocatedType;

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

static void allocate(uint64_t count, void** allocated, uint64_t* ticks) {
  size_t i = 0;

  for (i = 0; i < count; ++i) {
    ticks[i]     = get_ticks();
    allocated[i] = SHOOTOUT_MALLOC(AllocatedType);
  }
  ticks[i] = get_ticks();
}

static void deallocate(uint64_t count, void** allocated, uint64_t* ticks) {
  size_t i = 0;

  for (i = 0; i < count; ++i) {
    ticks[i] = get_ticks();
    SHOOTOUT_FREE(AllocatedType, allocated[i]);
  }
  ticks[i] = get_ticks();
}

int main(int argc, char const* argv[]) {
  uint64_t  count     = atoi(argv[1]);
  uint64_t* ticks     = malloc((count + 1) * sizeof(uint64_t));
  void**    allocated = malloc(count * sizeof(void*));

  FILE* results = fopen("results/" SHOOTOUT_OUTPUT, "w");

  uint64_t total  = 0;
  uint64_t before = get_ticks();

  allocate(count, allocated, ticks);
  uint64_t after = get_ticks();

  total += after - before;
  fprintf(results, "total_alloc %lf\n", ticks_to_ns(after - before));
  for (uint64_t i = 0; i < count; ++i) {
    fprintf(results, "alloc %lu %lf\n", i, ticks_to_ns(ticks[i + 1] - ticks[i]));
  }

  before = get_ticks();
  deallocate(count, allocated, ticks);
  after = get_ticks();

  total += after - before;
  fprintf(results, "total_dealloc %lf\n", ticks_to_ns(after - before));
  for (uint64_t i = 0; i < count; ++i) {
    fprintf(results, "dealloc %lu %lf\n", i, ticks_to_ns(ticks[i + 1] - ticks[i]));
  }

  free(ticks);
  free(allocated);

  fclose(results);

  printf("%lf\n", ticks_to_ns(total));
  sleep(5);

  return 0;
}
