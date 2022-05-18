#include "Init.h"
#include "commsReceive.h"

int main(void) {
    init();
    initCOMs();
    
    
    while(1)
    {
        CommunicationsHandle();
    }
    return 0;
}
