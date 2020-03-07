#include "uart2_config.h"

struct ring_buffer_t u2rx_buffer;
struct ring_buffer_t u2tx_buffer;
bool tx2_stall = true;


void uart2_init(void) {
    rbuffer_init(&u2rx_buffer);
    rbuffer_init(&u2tx_buffer);
//    RPINR19bits.U2RXR = 0b0110111; // rx on rp55 (rx1, pin3)
//    RPOR6bits.RP54R = 0b000011;    // tx on rp54 (tx1, pin2)
    U2MODEbits.BRGH = 0;           
    //int BRGH_MODE = U2MODEbits.BRGH == (uint16_t)1 ? 4 : 16;
    U2BRG = 16 - 1;
    U2STAbits.UTXISEL0 = 1;        // interrupt on last bit transmitted
    IFS1bits.U2TXIF = 0;           // clear tx interrupt flag
    IFS1bits.U2RXIF = 0;           // clear rx int flag
    IEC1bits.U2TXIE = 1;           // enable tx interrupts
    IEC1bits.U2RXIE = 1;           // enable rx interrupts
    U2MODEbits.UARTEN = 1;         // enable UART
    U2STAbits.UTXEN = 1;           // enable tx
}

void uart2_put(uint8_t val) {
    rbuffer_push(&u2tx_buffer, val);
    if (tx2_stall == true)
    {
        tx2_stall = false;
        U2TXREG = rbuffer_pop(&u2tx_buffer);
    }  
}

void uart2_put_c(uint8_t val) {
    IEC1bits.U2TXIE = 0;
    rbuffer_push(&u2tx_buffer, val);
    if (tx2_stall == true)
    {
        tx2_stall = false;
        U2TXREG = rbuffer_pop(&u2tx_buffer);
    }  
    IEC1bits.U2TXIE = 1;
}

uint8_t uart2_peek() {
    return rbuffer_peek(&u2rx_buffer);
}

uint8_t uart2_get() {
    return rbuffer_pop(&u2rx_buffer);
}

bool uart2_rx_empty() {
    return rbuffer_empty(&u2rx_buffer);
}

uint8_t* uart2_rx_getarray() {
    return rbuffer_getarray(&u2rx_buffer);
}

void uart2_enable(void) {
    IFS1bits.U2TXIF = 0;
    IFS1bits.U2RXIF = 0;
    IEC1bits.U2TXIE = 1;
    IEC1bits.U2RXIE = 1;
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;
}
void uart2_disable(void) {
    IFS1bits.U2TXIF = 0;
    IFS1bits.U2RXIF = 0;
    IEC1bits.U2TXIE = 0;
    IEC1bits.U2RXIE = 0;
    U2MODEbits.UARTEN = 0;
    U2STAbits.UTXEN = 0;
}
bool uart2_isenabled(void) {
    if (U2MODEbits.UARTEN == 1)
        return true;
    else
        return false;
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void) {
    rbuffer_push(&u2rx_buffer, (uint8_t)U2RXREG);
    IFS1bits.U2RXIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void) {
    if (rbuffer_size(&u2tx_buffer) > 0)
        U2TXREG = rbuffer_pop(&u2tx_buffer);
    else
        tx2_stall = true;
    IFS1bits.U2TXIF = 0;
}
