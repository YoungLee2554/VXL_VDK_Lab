/*
 * global.h
 *
 *  Created on: Oct 26, 2026
 *      Author: tftsp
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_
#include "main.h"
#include "timer.h"
#include "input_reading.h"
#include "led_display.h"
#include "input_processing.h"

#define MODE_1_NORMAL   1
#define MODE_2_RED      2
#define MODE_3_AMBER    3
#define MODE_4_GREEN    4

extern int mode;
extern int time_red;
extern int time_amber;
extern int time_green;


#endif /* INC_GLOBAL_H_ */
