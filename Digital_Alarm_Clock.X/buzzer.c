/*
 * File:   buzzer.c
 * Author: Utsava
 *
 * Created on 21 March, 2023, 8:58 AM
 */


#include "main.h"

void init_buzzer(void)
{
    BUZZER_DDR = BUZZER_DDR & 0xFD;
}

void alarm(void)
{
    BUZZER = BUZZER | 0x02;
}

void stop_alarm(void)
{
    BUZZER = BUZZER & 0xFD;
}