#include "uart1_config.h"

struct ring_buffer_t u1rx_buffer;
struct ring_buffer_t u1tx_buffer;

/*
void uart1_init(int baud) {
    rbuffer_init(&u1rx_buffer);
    rbuffer_init(&u1tx_buffer);
    
    RPINR18bits.U1RXR = 0b0111000; // rx on rp56 (rx-0, RC9/pin5)
    RPOR7bits.RP57R = 1;           // tx on rp57 (tx-0, RC8/pin4)
    U1MODEbits.BRGH = 1;           // high speed 4x baud
    int BRGH_MODE = U1MODEbits.BRGH == (uint16_t)1 ? 4 : 16;
    //U1BRG = FCY/(BRGH_MODE*baud)-1;
    U1STAbits.UTXISEL0 = 1;        // interrupt on last bit received
    IFS0bits.U1TXIF = 0;           // clear interrupt flag
    IEC0bits.U1TXIE = 1;           // enable interrupts
    U1MODEbits.UARTEN = 1;         // enable UART
    U1STAbits.UTXEN = 1;           // enable tx
}
*/
void uart1_init(void) {
    rbuffer_init(&u1rx_buffer);
    rbuffer_init(&u1tx_buffer);
    
    RPINR18bits.U1RXR = 0b0111000; // rx on rp57 (rx-0, RC9/pin5)
    RPOR7bits.RP57R = 0b000001;    // tx on rp56 (tx-0, RC8/pin4)
    U1MODEbits.BRGH = 1;           // high speed 4x baud
    U1BRG = 130-1;                 // need to work on macro->mplab does not like
    U1STAbits.UTXISEL0 = 1;        // interrupt on last bit received
    IFS0bits.U1TXIF = 0;           // clear interrupt flag
    IEC0bits.U1TXIE = 1;           // enable interrupts
    U1MODEbits.UARTEN = 1;         // enable UART
    U1STAbits.UTXEN = 1;           // enable tx
}

void uart1_put(uint8_t val) {
    rbuffer_push(&u1tx_buffer, val);
}

uint8_t uart1_peek() {
    return rbuffer_peek(&u1rx_buffer);
}

uint8_t uart1_get() {
    return rbuffer_pop(&u1rx_buffer);
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    rbuffer_push(&u1rx_buffer, (uint8_t)U1RXREG);
    IFS0bits.U1RXIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    U1TXREG = rbuffer_pop(&u1tx_buffer);
    IFS0bits.U1TXIF = 0;
}