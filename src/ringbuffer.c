#ifdef __cplusplus
# include <cstdint>
#else /* ifdef __cplusplus */
# include <stdint.h>
#endif /* ifdef __cplusplus */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef SHOOTOUT_SIZE
# define SHOOTOUT_SIZE 16
#endif /* ifndef SHOOTOUT_SIZE */

#if defined(SHOOTOUT_RINGBUFFER)

# include "test/ringbuffer.h"
# include "test/ringbuffer.c"

# define SHOOTOUT_OUTPUT "buffer-ring"
# define SHOOTOUT_CREATE(buffer_m, capacity_m) BNPPP_Ringbuffer * buffer_m = BNPPP_RingbufferCreate(capacity_m * sizeof(AllocatedType))
# define SHOOTOUT_WRITE(buffer_m, value_m)     BNPPP_RingbufferWrite(buffer_m, (uint8_t*) &value_m, sizeof(value_m))
# define SHOOTOUT_READ(buffer_m, data_m, data_length_m)         \
  do { BNPPP_RingbufferRead(buffer_m, &data_m, &data_length_m); \
       BNPPP_RingbufferReadCommit(buffer_m, data_length_m); } while (0)

#elif defined(SHOOTOUT_BOOSTRING)

# include <boost/circular_buffer.hpp>

# define SHOOTOUT_OUTPUT "buffer-boost"
# define SHOOTOUT_CREATE(buffer_m, capacity_m)          boost::circular_buffer< AllocatedType > buffer_m(capacity_m)
# define SHOOTOUT_WRITE(buffer_m, value_m)              buffer_m.push_back(value_m)
# define SHOOTOUT_READ(buffer_m, data_m, data_length_m) buffer_m.pop_front()

#else /* if defined(SHOOTOUT_RINGBUFFER) */

# define SHOOTOUT_OUTPUT "buffer-empty"
# define SHOOTOUT_CREATE(buffer_m, capacity_m)          do {} while (0)
# define SHOOTOUT_WRITE(buffer_m, value_m)              do {} while (0)
# define SHOOTOUT_READ(buffer_m, data_m, data_length_m) do {} while (0)

#endif /* if defined(SHOOTOUT_RINGBUFFER) */

typedef struct {
  char data[SHOOTOUT_SIZE];
} AllocatedType;

uint64_t get_ticks(void) {
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
  uint64_t       count        = atoi(argv[1]);
  uint64_t*      insert_ticks = (uint64_t*) malloc((count + 1) * sizeof(uint64_t));
  uint64_t*      remove_ticks = (uint64_t*) malloc((count + 1) * sizeof(uint64_t));
  AllocatedType* values       = (AllocatedType*) malloc((count + 1) * sizeof(AllocatedType));

  size_t i = 0;

  for (i = 0; i < count; ++i) {
    memset(values + i, i, sizeof(AllocatedType));
  }

  SHOOTOUT_CREATE(buffer, (count + 1) * sizeof(AllocatedType));
  FILE* results = fopen("results/" SHOOTOUT_OUTPUT, "w");

  uint64_t total = 0;
  srandom(1);

  uint64_t before = get_ticks();
  for (i = 0; i < count; ++i) {
    insert_ticks[i] = get_ticks();
    SHOOTOUT_WRITE(buffer, values[i]);
  }
  insert_ticks[i] = get_ticks();

  uint64_t after = get_ticks();

  total += after - before;
  fprintf(results, "total_alloc %lf\n", ticks_to_ns(after - before));
  for (uint64_t i = 0; i < count; ++i) {
    fprintf(results, "alloc %lu %lf\n", i, ticks_to_ns(insert_ticks[i + 1] - insert_ticks[i]));
  }

  uint8_t* data;
  size_t   data_length;

  srandom(1);
  before = get_ticks();
  for (i = 0; i < count; ++i) {
    remove_ticks[i] = get_ticks();
    SHOOTOUT_READ(buffer, data, data_length);
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
