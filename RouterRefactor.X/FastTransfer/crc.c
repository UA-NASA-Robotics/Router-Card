#include "crc.h"

/**
 * @brief   Calculates the CRC code for a packet.
 *
 * @param   arr     Pointer to the first element in a FastTransfer packet's
 *                  data segment.
 * @param   size    Number of bytes in the packet's data segment.
 *
 * @return  Returns the CRC code for the given packet.
 */
uint8_t crc (uint8_t const* arr, uint8_t size) {

	const uint8_t POLYNOMIAL = 0x8C;
	uint8_t value = 0x00;
	uint8_t i;
	for (i = 0; i < size; ++i) {
		uint8_t data = arr[i];
		uint8_t j;
		for (j = 0; j < 8; ++j) {
			uint8_t sum = (value ^ data) & (uint8_t)0x01;
			value >>= 1;
			if (sum)
				value ^= POLYNOMIAL;
			data >>= 1;
		}

	}

	return value;
}
