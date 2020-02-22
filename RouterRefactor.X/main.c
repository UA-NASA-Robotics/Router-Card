#include "Init.h"
#include "timers.h"
#include "commsReceive.h"
//#include "p33EP512GM604.h"

int main(void) {
    timer_t checkCANTimer;
    init();
    setTimerInterval(&checkCANTimer,100);
    while(1)
    {
        CommunicationsHandle();
    }
    return 0;
}
