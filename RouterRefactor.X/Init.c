#include "Init.h"
#include "timers.h"
#include "uart1_config.h"
//#include "uart2_config.h"
#include "mcc_generated_files/system.h"
#include "commsReceive.h"

void init(void) {
    SYSTEM_Initialize();
    timer1_init();
    uart1_init();
    TRISBbits.TRISB10 = 0;
    //uart2_init();
    //initCOMs();
    CAN1_TransmitEnable();
    CAN1_ReceiveEnable();
}