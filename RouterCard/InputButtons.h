/*
 * InputButtons.h
 *
 * Created: 5/4/2017 9:57:21 AM
 *  Author: Zac
 */ 


#ifndef INPUTBUTTONS_H_
#define INPUTBUTTONS_H_

#include <stdbool.h>
#include "Init.h"

typedef enum{
	SPECIAL,		//C0
	DWN,			//C1
	UP,				//C2
	BCK=7			//A7
	}buttonList_t;

bool buttonPressed(buttonList_t b);
void buttonsInputTest(void);



#endif /* INPUTBUTTONS_H_ */