#include "ringbuffer.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct _BNPPP_Ringbuffer {
  size_t capacity;

  uint8_t* buffer;
  uint8_t* buffer_end;

  uint8_t* read_position;
  uint8_t* write_position;

  uint64_t read_cycle;
  uint64_t write_cycle;
};

typedef struct _BNPPP_RingbufferHeader {
  uint8_t cycle_flag;
  size_t  data_length;
} BNPPP_RingbufferHeader;

BNPPP_Ringbuffer* BNPPP_RingbufferCreate(size_t const capacity) {
  BNPPP_Ringbuffer* ring_buffer = malloc(sizeof(BNPPP_Ringbuffer));

  BNPPP_ERROR_IF(return 0, !ring_buffer, "create ring_buffer failed, unable to allocate memory of size %zu.", sizeof(BNPPP_Ringbuffer));

  ring_buffer->buffer = malloc(capacity * sizeof(uint8_t));
  BNPPP_ERROR_IF(return ring_buffer,
                 !ring_buffer->buffer,
                 "create ring_buffer failed, unable to allocate memory of size %zu.",
                 capacity * sizeof(uint8_t));

  ring_buffer->capacity       = capacity;
  ring_buffer->read_cycle     = 1;
  ring_buffer->write_cycle    = 0;
  ring_buffer->buffer_end     = ring_buffer->buffer + capacity * sizeof(uint8_t);
  ring_buffer->read_position  = ring_buffer->buffer;
  ring_buffer->write_position = ring_buffer->buffer;

  return ring_buffer;
}

void BNPPP_RingbufferDestroy(BNPPP_Ringbuffer* ring_buffer) {
  BNPPP_ERROR_IF(return , !ring_buffer, "bnppp_ring_buffer_destroy with null ring_buffer");

  if (ring_buffer->buffer) {
    free(ring_buffer->buffer);
  }

  free(ring_buffer);
}

#define BNPPP_RING_BUFFER_REWIND ~((uint64_t) 0)

BNPPP_RingbufferStatus BNPPP_RingbufferRead(BNPPP_Ringbuffer* ring_buffer, uint8_t** data_out, size_t* data_length_out) {
  BNPPP_RingbufferHeader* header = (BNPPP_RingbufferHeader*) ring_buffer->read_position;

  if (__sync_bool_compare_and_swap(&header->cycle_flag, ring_buffer->read_cycle, ring_buffer->read_cycle)) {
    if (header->data_length == BNPPP_RING_BUFFER_REWIND) {
      ring_buffer->read_position = ring_buffer->buffer;
      ring_buffer->read_cycle   += 2;

      __sync_add_and_fetch(&header->cycle_flag, 1);
      return BNPPP_RING_BUFFER_STATUS_RETRY_IMMEDIATELY;

    } else {
      *data_length_out = header->data_length;
      *data_out        = ring_buffer->read_position + sizeof(*header);

      return BNPPP_RING_BUFFER_STATUS_OK;
    }

  } else {
    return BNPPP_RING_BUFFER_STATUS_RETRY_LATER;
  }
}

void BNPPP_RingbufferReadCommit(BNPPP_Ringbuffer* ring_buffer, size_t const data_length) {
  BNPPP_RingbufferHeader* header = (BNPPP_RingbufferHeader*) ring_buffer->read_position;

  size_t const message_length = sizeof(BNPPP_RingbufferHeader) + data_length;

  while (!__sync_bool_compare_and_swap(&header->cycle_flag, ring_buffer->read_cycle, ring_buffer->read_cycle)) {}

  if (ring_buffer->read_position + message_length + sizeof(BNPPP_RingbufferHeader) > ring_buffer->buffer_end) {
    ring_buffer->read_position = ring_buffer->buffer;
    ring_buffer->read_cycle   += 2;
  } else {
    ring_buffer->read_position += message_length;
  }

  __sync_add_and_fetch(&header->cycle_flag, 1);
}

static void BNPPP_RingbufferRewindWrite(BNPPP_Ringbuffer* ring_buffer) {
  BNPPP_RingbufferHeader* header = (BNPPP_RingbufferHeader*) ring_buffer->write_position;

  header->data_length = BNPPP_RING_BUFFER_REWIND;
  __sync_add_and_fetch(&header->cycle_flag, 1);
  ring_buffer->write_position = ring_buffer->buffer;
  ring_buffer->write_cycle   += 2;
}

void BNPPP_RingbufferWrite(BNPPP_Ringbuffer* ring_buffer, uint8_t* const data, size_t const data_length) {
  size_t const            message_length = data_length + sizeof(BNPPP_RingbufferHeader);
  BNPPP_RingbufferHeader* header         = (BNPPP_RingbufferHeader*) ring_buffer->write_position;

  while (!__sync_bool_compare_and_swap(&header->cycle_flag, ring_buffer->write_cycle, ring_buffer->write_cycle)) {}

  if (ring_buffer->write_position + message_length > ring_buffer->buffer_end) {
    BNPPP_RingbufferRewindWrite(ring_buffer);
    header = (BNPPP_RingbufferHeader*) ring_buffer->write_position;
    while (!__sync_bool_compare_and_swap(&header->cycle_flag, ring_buffer->write_cycle, ring_buffer->write_cycle)) {}
  }

  header->data_length = data_length;
  memcpy(ring_buffer->write_position + sizeof(*header), (char const*) data, data_length);
  __sync_add_and_fetch(&header->cycle_flag, 1);

  ring_buffer->write_position += message_length;
  if (ring_buffer->write_position + sizeof(*header) > ring_buffer->buffer_end) {
    ring_buffer->write_position = ring_buffer->buffer;
    ring_buffer->write_cycle   += 2;
  }
}

#ifndef BNPPP_RING_BUFFER_TEST_COUNT
# define BNPPP_RING_BUFFER_TEST_COUNT 1000000
#endif /* ifndef BNPPP_RING_BUFFER_TEST_COUNT */

static void* Test_BNPPP_RingbufferThread(void* data) {
  BNPPP_Ringbuffer* ring_buffer = (BNPPP_Ringbuffer*) data;

  for (uint64_t i = 0; i < BNPPP_RING_BUFFER_TEST_COUNT; ++i) {
    uint8_t* data;
    size_t   data_length;
    while (BNPPP_RingbufferRead(ring_buffer, &data, &data_length) != BNPPP_RING_BUFFER_STATUS_OK) {}

    if (data_length != sizeof(uint64_t)) {
      L_ERROR("Message length %zu != expected %zu", data_length, sizeof(uint64_t));
    }

    uint64_t value = *(uint64_t*) data;
    if (value != i) {
      L_ERROR("Message data %" PRIu64 " != expected %" PRIu64 "", value, i);
      L_ERROR("Read cycle %" PRIu64 " pos %zu", ring_buffer->read_cycle, ring_buffer->read_position - ring_buffer->buffer);
      L_ERROR("Write cycle %" PRIu64 " pos %zu", ring_buffer->write_cycle, ring_buffer->write_position - ring_buffer->buffer);
    }

    BNPPP_RingbufferReadCommit(ring_buffer, data_length);
  }

  return 0;
}

void Test_BNPPP_Ringbuffer() {
  BNPPP_Ringbuffer* ring_buffer = BNPPP_RingbufferCreate(102345);

  pthread_t test_thread;

  pthread_create(&test_thread, NULL, &Test_BNPPP_RingbufferThread, ring_buffer);

  for (uint64_t i = 0; i < BNPPP_RING_BUFFER_TEST_COUNT; ++i) {
    BNPPP_RingbufferWrite(ring_buffer, (uint8_t*) &i, sizeof(uint64_t));
  }

  pthread_join(test_thread, NULL);

  BNPPP_RingbufferDestroy(ring_buffer);
}
