#include "parser.h"
#include "buffer.h"
#include "../ft_config.h"
#include "../../AT90CAN_UART.h"
#include "../../LEDs.h"
#include "crc.h"
#include <stdio.h>
/*
 * Symbolic constants to represent status of parser.
 */

#define PARSE_STATUS_GOOD            1
#define PARSE_STATUS_NOT_ENOUGH_DATA 2
#define PARSE_STATUS_BAD_FIRST_BYTE  3
#define PARSE_STATUS_BAD_SECOND_BYTE 4
#define PARSE_STATUS_BAD_ADDRESS     5
#define PARSE_STATUS_BAD_MSG_LENGTH  6
#define PARSE_STATUS_BAD_CRC_CODE    7

/*
 * Module State
 */

static Buffer_t * m_buf;
static uint16_t *  m_arr;
static bool *     m_flags;
static int        m_arr_size;
static uint8_t    m_address;
static uint8_t    m_sender;
static uint8_t    m_length;
static unsigned int        m_lookahead;
static unsigned int        m_status;

/*
 * Helper Functions
 */

void Parser_init (Buffer_t * buf, uint16_t * arr, bool * flags, int arr_size, uint8_t address)
{
	m_buf = buf;
	m_arr = arr;
	m_flags = flags;
	m_arr_size = arr_size;
	m_address = address;
	m_length = 0;
	m_lookahead = 0;
	m_status = PARSE_STATUS_GOOD;
}

void Parser_first_byte ()
{
	// verify that there is enough data in the buffer to continue parsing:
	if (m_lookahead >= Buffer_size(m_buf))
	{
		m_status = PARSE_STATUS_NOT_ENOUGH_DATA;
		return;
	}

	// verify that first byte has correct value:
	if (Buffer_get(m_buf, m_lookahead) != 0x06)
	{
		m_status = PARSE_STATUS_BAD_FIRST_BYTE;
		return;
	}

	// correct first byte:
	++m_lookahead;
}

void Parser_second_byte ()
{
	// verify that there is enough data in the buffer to continue parsing:
	if (m_lookahead >= Buffer_size(m_buf))
	{
		m_status = PARSE_STATUS_NOT_ENOUGH_DATA;
		return;
	}

	// verify that second byte has correct value:
	if (Buffer_get(m_buf, m_lookahead) != 0x85)
	{
		m_status = PARSE_STATUS_BAD_SECOND_BYTE;
		return;
	}

	// correct second byte:
	++m_lookahead;
}

void Parser_sender()
{
	// verify that there is enough data in the buffer to continue parsing:
	if (m_lookahead >= Buffer_size(m_buf))
	{
		m_status = PARSE_STATUS_NOT_ENOUGH_DATA;
		return;
	}

	// store the sender's address:
	m_sender = Buffer_get(m_buf, m_lookahead);
	++m_lookahead;
}

void Parser_receiver ()
{
	// verify that there is enough data in the buffer to continue parsing:
	if (m_lookahead >= Buffer_size(m_buf))
	{
		m_status = PARSE_STATUS_NOT_ENOUGH_DATA;
		return;
	}

	// validate the receiver address:
	if (Buffer_get(m_buf, m_lookahead) != m_address)
	{
		m_status = PARSE_STATUS_BAD_ADDRESS;
		return;
	}

	// correct address:
	++m_lookahead;
}

void Parser_message_length ()
{
	// verify that there is enough data in the buffer to continue parsing:
	if (m_lookahead >= Buffer_size(m_buf))
	{
		m_status = PARSE_STATUS_NOT_ENOUGH_DATA;
		return;
	}

	// validate the message length:
	m_length = Buffer_get(m_buf, m_lookahead);
	if (m_length > MAX_PCKT_SZ || m_length % 3 != 0)
	{
		m_status = PARSE_STATUS_BAD_MSG_LENGTH;
		return;
	}

	// good message length:
	++m_lookahead;
}

void Parser_message_content ()
{
	// verify that there is enough data in the buffer to continue parsing:
	if (m_lookahead + m_length - 1 >= Buffer_size(m_buf))
	{

		m_status = PARSE_STATUS_NOT_ENOUGH_DATA;
		return;
	}

	m_lookahead += m_length;
}

void Parser_crc ()
{
	// verify that there is enough data in the buffer to continue parsing:
	if (m_lookahead >= Buffer_size(m_buf))
	{
		m_status = PARSE_STATUS_NOT_ENOUGH_DATA;
		return;
	}

	// calculate the CRC code:
	// - future improvement: remove this unnecessary copying
	uint8_t message [MAX_PCKT_SZ];
	int i;
	for (i = 0; i < m_length; ++i) {
		message[i] = Buffer_get(m_buf, i + 5);
	}
	uint8_t correct_crc = crc(message, m_length);

	// compare calculated CRC code to received CRC code:
	if (Buffer_get(m_buf, m_lookahead) != correct_crc)
	{

		m_status = PARSE_STATUS_BAD_CRC_CODE;
		return;
	}

	// correct CRC
	++m_lookahead;
}

int Parser_status ()
{
	return m_status;
}

void Parser_trash_bytes (int num_bytes)
{
	while (num_bytes-- > 0)
		Buffer_pop(m_buf);
}

void Parser_message ()
{
	Parser_first_byte();
	if (m_status != PARSE_STATUS_GOOD) return;

	Parser_second_byte();
	if (m_status != PARSE_STATUS_GOOD) return;

	Parser_receiver();
	if (m_status != PARSE_STATUS_GOOD) return;

	Parser_sender();
	if (m_status != PARSE_STATUS_GOOD) return;

	Parser_message_length();
	if (m_status != PARSE_STATUS_GOOD) return;

	Parser_message_content();
	if (m_status != PARSE_STATUS_GOOD) return;

	Parser_crc();
}

void Parser_write_message_data ()
{
	// Write the message into the array
	const int MSG_BEGIN = 5;
	const int MSG_END = MSG_BEGIN + m_length;
	int i;
	for (i = MSG_BEGIN; i < MSG_END; i += 3)
	{
		uint8_t index = Buffer_get(m_buf, i);
		uint16_t lsb = Buffer_get(m_buf, i + 1);
		uint16_t msb = Buffer_get(m_buf, i + 2);
		uint16_t data = (msb << 8) | lsb;

		if (index < m_arr_size) {
			m_arr[index] = data;
			m_flags[index] = true;


		}
	}

	// Pop the bytes from the buffer
	int n = m_length + 6;
	while (n-- > 0)
		Buffer_pop(m_buf);
}

/*
 * API Function
 */
int byteWriteCount = 0;
unsigned int parse (Buffer_t * buf, uint16_t * arr, bool * flags, int arr_size, uint8_t address)
{
	byteWriteCount = 0;
	while (1)
	{
		// reset the parsing module
		Parser_init(buf, arr, flags, arr_size, address);

		if (Buffer_empty(m_buf)) {

			// exit because no more data in buffer

			return byteWriteCount;
		}

		Parser_message();
		int status = Parser_status();

		switch (status)
		{
		case PARSE_STATUS_GOOD:


			Parser_write_message_data();
			toggleLED(8);
			byteWriteCount++;
			break;

		case PARSE_STATUS_NOT_ENOUGH_DATA:
			if(Buffer_full(m_buf))
			{
				Buffer_clear(m_buf);
			}
			// exit because there is not enough data in the buffer
			// to finish the message
			toggleLED(9);
			return byteWriteCount;

		case PARSE_STATUS_BAD_FIRST_BYTE:
		case PARSE_STATUS_BAD_SECOND_BYTE:
		case PARSE_STATUS_BAD_ADDRESS:
		case PARSE_STATUS_BAD_MSG_LENGTH:
		case PARSE_STATUS_BAD_CRC_CODE:
		default:

			Parser_trash_bytes(1);
			//if(status != PARSE_STATUS_BAD_FIRST_BYTE)
			//printf(": %d ",status);
			toggleLED(10);
			break;
		}
	}
}
