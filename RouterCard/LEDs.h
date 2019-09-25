/*
 * LEDs.h
 *
 * Created: 5/3/2017 11:13:56 PM
 *  Author: Zac
 */


#ifndef LEDS_H_
#define LEDS_H_
#include "Config.h"
#include "light_ws2812.h"
#include "ws2812_config.h"
#include <stdbool.h>

#define ON  true
#define OFF false

#ifdef NEW_MAIN

#define NUMBER_OF_LEDS 12


// #define Test1 3
// #define Test2 4
// #define Test3 5
// #define Test4 6
// #define Test5 7
//
// #define Err1 1
// #define Err2 2
// #define Err3 3
// #define Err4 4
// #define Err5 5
// #define Err6 6

typedef enum {
	LED1=1,
	LED2,
	LED3,
	LED4,
	LED5,
	LED6,
	LED7,
	LED8,
	LED9,
	LED10,
	LED11,
	LED12
} LEDs_ENUMED_t;

//'Top LEDs'
#define LED1_P_HIGH 0x10  //4 //F4  - 1
#define LED2_P_HIGH 0x40  //6 //F6  - 2
#define LED3_P_HIGH 0x01  //0 //A0  - 3
#define LED4_P_HIGH 0x04  //2 //A2  - 4
#define LED5_P_HIGH 0x10  //4 //A4  - 5
#define LED6_P_HIGH 0x40  //6 //A6  - 6

//'Bottom LEDs'
#define LED12_P_HIGH  0x08  //3 //F3  - 7
#define LED11_P_HIGH  0x20  //5 //F5  - 8
#define LED10_P_HIGH  0x80  //7 //F7  - 9
#define LED9_P_HIGH 0x02  //1 //A1  - 10
#define LED8_P_HIGH 0x08  //3 //A3  - 11
#define LED7_P_HIGH 0x20  //5 //A5  - 12

#define LED1_P_LOW  0xEF  //4 //F4  - 1
#define LED2_P_LOW  0xBF  //6 //F6  - 2
#define LED3_P_LOW  0xFE  //0 //A0  - 3
#define LED4_P_LOW  0xFB  //2 //A2  - 4
#define LED5_P_LOW  0xEF  //4 //A4  - 5
#define LED6_P_LOW  0xBF  //6 //A6  - 6

//'Bottom LEDs'
#define LED12_P_LOW 0xF7  //3 //F3  - 7
#define LED11_P_LOW 0xDF  //5 //F5  - 8
#define LED10_P_LOW 0x7F  //7 //F7  - 9
#define LED9_P_LOW  0xFD  //1 //A1  - 10
#define LED8_P_LOW  0xF7  //3 //A3  - 11
#define LED7_P_LOW  0xDF  //5 //A5  - 12

#else
#define NUMBER_OF_LEDS 4

typedef enum {
	TEST1=1,
	TEST2,
	ERROR1,
	ERROR2
} LEDs_ENUMED_t;


#define TEST1_P_HIGH  0x08    //A3  
#define TEST1_P_LOW   0xF7
#define TEST2_P_HIGH  0x10    //A4
#define TEST2_P_LOW   0xEF
#define ERROR1_P_HIGH 0x20    //A5
#define ERROR1_P_LOW  0xDF
#define ERROR2_P_HIGH 0x40    //A6
#define ERROR2_P_LOW  0xBF


#endif
void initLED();
void rainBow();
void SetNeoPixRGB(unsigned int R, unsigned G, unsigned int B);
void setLED(LEDs_ENUMED_t ledNumber, bool state);
void toggleLED(LEDs_ENUMED_t ledNumber);
void flashLedColors(cRGB_t cRGB_1,cRGB_t cRGB_2);

#endif /* LEDS_H_ */