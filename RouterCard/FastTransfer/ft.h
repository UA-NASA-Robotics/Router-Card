#ifndef FT_H
#define FT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "ft_config.h"
#include "ft_impl/buffer.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                    Handle Type for FastTransfer Instance                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

struct FastTransferHandle {
	// local data:
	uint8_t  address;
	uint16_t array [ARRAY_SZ];
	bool     flags [ARRAY_SZ];

	// buffers:
	Buffer_t receive_buf;
	Buffer_t transmit_buf;

	// callbacks (for interacting with serial communication hardware):
	void(*put)(uint8_t);
	uint8_t(*get)();
	bool(*empty)();
};

typedef struct FastTransferHandle FT_t;
///--------------------------------------------------------------------------
/// FT_t is an alias for a type representing a single fast-transfer handle.
///--------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                            FastTransfer Methods                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void FT_Init (FT_t * handle,
							uint8_t address,
							void(*f_put)(uint8_t), uint8_t(*f_get)(), bool(*f_empty)());
///--------------------------------------------------------------------------
/// Initializes the FastTransfer instance associated with the given handle
/// by setting all values of the array to 0 and all modification flags
/// to true.  Also connects the instance to serial transmit and receive
/// buffers via the three callback functions.
///
/// Parameters:
///   - handle:  Memory address of a handle to a FastTransfer instance.
///   - address: FastTransfer address of the node.
///   - f_put:   Callback function that adds a byte to a transmit buffer.
///   - f_get:   Callback function that reads a byte from a receive buffer.
///   - f_empty: Callback function that returns true iff receive buffer
///              contains 0 bytes.
///--------------------------------------------------------------------------

int16_t FT_Read (FT_t * handle, uint8_t index);
///--------------------------------------------------------------------------
/// Read a single value from the local fast-transfer array.
/// As a side-effect, this function unsets the modification flag for the
/// element at the given index.
///--------------------------------------------------------------------------

bool FT_Modified (FT_t * handle, uint8_t index);
///--------------------------------------------------------------------------
/// Read the modification flag, which is true if and only if the data at the
/// given index has not already been read.
///--------------------------------------------------------------------------

unsigned int FT_Receive (FT_t * handle);
///--------------------------------------------------------------------------
/// Reads the bytes from the serial buffer, parses them, and updates the
/// local array if any valid messages were received. returns the number of
/// bytes updated.
///--------------------------------------------------------------------------

void FT_ToSend (FT_t * handle, uint8_t index, int16_t data);
///--------------------------------------------------------------------------
/// Adds the given data to the given handle's next packet to be transmitted.
///--------------------------------------------------------------------------

void FT_Send (FT_t * handle, uint8_t address);
///--------------------------------------------------------------------------
/// Transmits the handle's packet to the given address.
///--------------------------------------------------------------------------

#ifdef __cplusplus
};
#endif
#endif // FT_H
