#ifndef FT_H
#define FT_H
#ifdef __cplusplus
extern "C" {
#endif

#include "ft_config.h"
#ifndef SAFE
#include <stdbool.h>
#endif
#include <stdint.h>
//#include "buffer.h"
#include "ring_buffer.h"


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                    Handle Type for FastTransfer Instance                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


//struct FastTransferHandle {
typedef struct 
{
	// local data:
	uint8_t  address;
	uint16_t array [ARRAY_SZ];
	#ifndef SAFE
	bool     flags [ARRAY_SZ];
	#endif
	#ifdef SAFE
	int flags[ARRAY_SZ];
	#endif

	// buffers:
	//Buffer_t receive_buf;
	//Buffer_t transmit_buf;
	struct ring_buffer_t receive_buf;
	struct ring_buffer_t transmit_buf;

	// callbacks (for interacting with serial communication hardware):
	void(*put)(uint8_t);
	uint8_t(*get)();
	#ifndef SAFE
	bool(*empty)();
	#endif
	#ifdef SAFE
	int(*empty)();
	#endif
} FT_t;

//typedef struct FastTransferHandle FT_t;
///--------------------------------------------------------------------------
/// FT_t is an alias for a type representing a single fast-transfer handle.
///--------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                            FastTransfer Methods                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifndef SAFE
void FT_Init (FT_t * handle,
							uint8_t address,
							void(*f_put)(uint8_t), uint8_t(*f_get)(), bool(*f_empty)());
#endif
#ifdef SAFE
void FT_Init (FT_t * handle,
							uint8_t address,
							void(*f_put)(uint8_t), uint8_t(*f_get)(), int(*f_empty)());
#endif
///--------------------------------------------------------------------------
/// Initializes the FastTransfer instance associated with the given handle
/// by setting all values of the array to 0 and all modification flags
/// to TRUE.  Also connects the instance to serial transmit and receive
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

#ifndef SAFE
bool FT_Modified (FT_t * handle, uint8_t index);
#endif
#ifdef SAFE
int FT_Modified (FT_t * handle, uint8_t index);
#endif
///--------------------------------------------------------------------------
/// Read the modification flag, which is TRUE if and only if the data at the
/// given index has not already been read.
///--------------------------------------------------------------------------

unsigned int FT_Receive (FT_t * handle);
///--------------------------------------------------------------------------
/// Reads the bytes from the serial buffer, parses them, and updates the
/// local array if any valid messages were received.
///--------------------------------------------------------------------------

void FT_ToSend (FT_t * handle, uint8_t index, int16_t data);
///--------------------------------------------------------------------------
/// Adds the given data to the given handle's next packet to be transmitted.
///--------------------------------------------------------------------------

void FT_Send (FT_t * handle, uint8_t address);
///--------------------------------------------------------------------------
/// Transmits the handle's packet to the given address.
///--------------------------------------------------------------------------

void FT_ClearFlag(FT_t* handle, uint8_t index);

//uint16_t* FT_GetArray(void);

#ifdef __cplusplus
};
#endif
#endif // FT_H
