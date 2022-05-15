//#ifndef FT_BUFFER_H
//#define FT_BUFFER_H
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#include <stdbool.h>
//#include <stdint.h>
//#include <stddef.h>
//
//#include "ft_config.h"
//
//typedef struct {
//	uint8_t data [MAX_PCKT_SZ];
//	unsigned int size;
//	unsigned int head;
//	unsigned int tail;
//} Buffer_t;
//
//void Buffer_init (Buffer_t *);
///// Initializes the given buffer.
//
//size_t Buffer_size (Buffer_t *);
///// Returns the number of elements in the ring buffer.
//
//bool Buffer_full (Buffer_t *);
///// Returns true if and only if the number of elements in the ring buffer is
///// equal to the total capacity of the ring buffer.
//
//bool Buffer_empty (Buffer_t *);
///// Returns true if and only if there are 0 elements in the ring buffer.
//
//void Buffer_push (Buffer_t *, uint8_t);
///// Adds given element to the back of the ring buffer queue.
//
//uint8_t Buffer_pop (Buffer_t *);
///// Pops and returns an element from the front of the ring buffer queue.
//
//uint8_t Buffer_get (Buffer_t *, unsigned int);
///// Random access to the elements of the ring buffer. The given index shall be
///// less than the current size of the given buffer.
//
//uint8_t * Buffer_get_array (Buffer_t *);
///// Returns a pointer to the first element in the underlying array.
//void Buffer_clear(Buffer_t *buf);
//
//#ifdef __cplusplus
//};
//#endif
//#endif // FT_BUFFER_H
