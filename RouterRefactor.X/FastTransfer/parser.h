#ifndef FT_PARSER_H
#define FT_PARSER_H
#ifdef __cplusplus
extern "C" {
#endif

//#include "FastTransfer/ring_buffer.h"

#include "ring_buffer.h"
    
unsigned int parse (struct ring_buffer_t * buf, uint16_t * arr, bool* flags, int arr_size, uint8_t address);

#ifdef __cplusplus
};
#endif
#endif // FT_PARSER_H
