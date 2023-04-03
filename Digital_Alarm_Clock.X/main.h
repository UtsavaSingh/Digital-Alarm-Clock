/* 
 * File:   main.h
 * Author: Utsava
 *
 * Created on 14 March, 2023, 10:17 AM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include "i2c.h"
#include"ds1307.h"
#include "clcd.h"
#include "EEPROM.h"
#include "digital_keypad.h"
#include "digital_alarm_clock.h"
#include "timers.h"
#include "buzzer.h"
#include <string.h>

#define RETURN_BACK                 0x22
#define TASK_SUCCESS                0x23
#define TASK_FAIL                   0x24

#define DASH_BOARD_FLAG             0x01
#define MENU1_FLAG                  0x02
#define MENU2_FLAG                  0x03
#define MENU3_FLAG                  0x04
#define SET_EVENT_FLAG              0x05
#define VIEW_EVENT_FLAG             0x06
#define SET_TIME_FLAG               0x07
#define SET_DATE_FLAG               0x08
#define RESET_NOTHING               0x09
#define RESET_MENU1                 0x10
#define RESET_MENU2                 0x11
#define RESET_MENU3                 0x12
#define RESET_SET_EVENT             0x13
#define RESET_TIME                  0x14
#define RESET_VIEW_EVENT            0x15
#define RESET_DATE                  0x16
#define RESET_DASH_SCREEN           0x17
#define NOTHING                     0x30                        

#endif	/* MAIN_H */

