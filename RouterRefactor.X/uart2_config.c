#include "uart2_config.h"

struct ring_buffer_t u2rx_buffer;
struct ring_buffer_t u2tx_buffer;

/*
void uart2_init(int baud) {
    RPINR18bits.U2RXR = 0b0111000; // rx on rp56 (rx-0, RC9/pin5)
    RPOR7bits.RP57R = 1;           // tx on rp57 (tx-0, RC8/pin4)
    U2MODEbits.BRGH = 1;           // high speed 4x baud
    int BRGH_MODE = U2MODEbits.BRGH == (uint16_t)1 ? 4 : 16;
    U2BRG = FCY/(BRGH_MODE*baud)-1;
    U2STAbits.UTXISEL0 = 1;        // interrupt on last bit transmitted
    IFS1bits.U2TXIF = 0;           // clear interrupt flag
    IEC1bits.U2TXIE = 1;           // enable interrupts
    U2MODEbits.UARTEN = 1;         // enable UART
    U2STAbits.UTXEN = 1;           // enable tx
}
*/

void uart2_init(void) {
    RPINR19bits.U2RXR = 0b0110111; // rx on rp55 (rx1, pin3)
    RPOR6bits.RP54R = 0b000011;           // tx on rp54 (tx1, pin2)
    U2MODEbits.BRGH = 1;           // high speed 4x baud
    int BRGH_MODE = U2MODEbits.BRGH == (uint16_t)1 ? 4 : 16;
    U2BRG = 130 - 1;
    U2STAbits.UTXISEL0 = 1;        // interrupt on last bit transmitted
    IFS1bits.U2TXIF = 0;           // clear interrupt flag
    IEC1bits.U2TXIE = 1;           // enable interrupts
    U2MODEbits.UARTEN = 1;         // enable UART
    U2STAbits.UTXEN = 1;           // enable tx
}

void uart2_put(uint8_t val) {
    rbuffer_push(&u2tx_buffer, val);
}

uint8_t uart2_peek() {
    return rbuffer_peek(&u2rx_buffer);
}

uint8_t uart2_get() {
    return rbuffer_pop(&u2rx_buffer);
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void) {
    rbuffer_push(&u2rx_buffer, (uint8_t)U2RXREG);
    IFS1bits.U2RXIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void) {
    U2TXREG = rbuffer_pop(&u2tx_buffer);
    IFS1bits.U2TXIF = 0;
}
