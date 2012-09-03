#ifndef BNPPP_RINGBUFFER_H_
#define BNPPP_RINGBUFFER_H_

#define BNPPP_ERROR_IF(...) do {} while (0)
#define L_ERROR(...) do {} while (0)

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct _BNPPP_Ringbuffer BNPPP_Ringbuffer;

typedef enum {
  BNPPP_RING_BUFFER_STATUS_OK, BNPPP_RING_BUFFER_STATUS_RETRY_LATER, BNPPP_RING_BUFFER_STATUS_RETRY_IMMEDIATELY,
} BNPPP_RingbufferStatus;

BNPPP_Ringbuffer* BNPPP_RingbufferCreate(size_t const capacity);
void              BNPPP_RingbufferDestroy(BNPPP_Ringbuffer* ring_buffer);

BNPPP_RingbufferStatus BNPPP_RingbufferRead(BNPPP_Ringbuffer* ring_buffer, uint8_t** data_out, size_t* data_length_out);
void                   BNPPP_RingbufferReadCommit(BNPPP_Ringbuffer* ring_buffer, size_t const data_length);
void                   BNPPP_RingbufferWrite(BNPPP_Ringbuffer* ring_buffer, uint8_t* const data, size_t const data_length);

void Test_BNPPP_Ringbuffer();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BNPPP_RINGBUFFER_H_ */
