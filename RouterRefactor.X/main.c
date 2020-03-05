#include "Init.h"
#include "timers.h"
//#include "commsReceive.h"
#include "mcc_generated_files/can1.h"
#include "FastTransfer_CAN.h"
#include "FastTransfer/ft.h"
#define FCY 30000000UL
#include <libpic30.h>
#include "uart1_config.h"
FT_t ft_handle;
FTC_t ftc_handle;
int main(void) {
    //timer_t checkCANTimer;
    init();
    //setTimerInterval(&checkCANTimer,100);
    LATBbits.LATB10 = 1;
    FT_Init(&ft_handle, ROUTER_CARD, uart1_put_c, uart1_get, uart1_rx_empty);
    FTC_Init(&ftc_handle, ROUTER_CARD, 1, CAN1_Initialize, CAN1_transmit, CAN1_receive);
    while (1) {
        FT_ToSend(&ft_handle, 0, 11);
        FT_ToSend(&ft_handle, 1, 0xffff);
        FT_Send(&ft_handle, 6);
        //ToSendCAN(4, 0x55AA);
        //sendDataCAN(6);
        FTC_ToSend(&ftc_handle, 4, 0x55AA);
        FTC_Send(&ftc_handle, 6);
        //LATBbits.LATB10 ^= 1;
        //__delay_ms(1000);
        //CommunicationsHandle();
    }
    return 0;
}
