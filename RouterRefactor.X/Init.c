#include "Init.h"
#include "timers.h"
#include "uart1_config.h"
#include "uart2_config.h"
#include <libpic30.h>

void init(void) {
    SYSTEM_Initialize();
    timer1_init();
    uart1_init();
    uart2_init();
}