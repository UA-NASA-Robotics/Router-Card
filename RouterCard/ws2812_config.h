/*
 * light_ws2812_config.h
 * v2.4 - Nov 27, 2016
 * User Configuration file for the light_ws2812_lib
 */


#ifndef WS2812_CONFIG_H_
#define WS2812_CONFIG_H_

// Define Reset time in µs.
// WS2813 needs 300 µs reset time
// WS2812 and clones only need 50 µs
#define ws2812_port C     // Data port 

#define ws2812_resettime  300




#define ws2812_pin1 3    // Data out pin
#define ws2812_pin2 4    // Data out pin
#define ws2812_pin3 5    // Data out pin
#define ws2812_pin4 6    // Data out pin
#define LEDSTRIP_CONNECT_PENDING_1    (cRGB_t){0,50,0}
#define LEDSTRIP_CONNECT_PENDING_2    (cRGB_t){0,0,50}
#define LEDSTRIP_CONNECTED         0,15,0

#define  LEDSTRIP_MACRO_RUNNING  50, 0, 50
#define LEDSTRIP_Macro_PENDING_1     (cRGB_t){LEDSTRIP_CONNECTED}
#define LEDSTRIP_Macro_PENDING_2    (cRGB_t){LEDSTRIP_MACRO_RUNNING}



#endif /* WS2812_CONFIG_H_ */