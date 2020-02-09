#include "uart_config.h"
#include <math.h>
#include <stdint.h>
void UART_INIT(int con, int sta, int baud) {
    int BRGH_MODE = 16;
    U2MODE = con;
    U2STA = sta;
    if (U2MODEbits.BRGH == (uint16_t)1)
        BRGH_MODE = 4;
    else
        BRGH_MODE = 16;
    U2BRG = floor((FCY/(BRGH_MODE*baud)) - 1);
}

void uart_init(int baud) {
    
    // rx/tx on rp56/57
    RPINR18bits.U1RXR = 0b0111000; // rx on rp56 (rx-0)
    RPOR7bits.RP57R = 1;           // tx on rp57 (tx-0))
    U1MODEbits.BRGH = 1;           // high speed 4x baud
    int BRGH_MODE = U1MODEbits.BRGH == (uint16_t)1 ? 4 : 16;
    U1BRG = FCY/(BRGH_MODE*baud)-1;
    U1STAbits.URXISEL = 0b01;      // interrupt on last bit received
    IFS0bits.U1TXIF = 0;           // 
    IEC0bits.U1TXIE = 1;
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
}
