/*
 * File:   Digital_alarm_clock.h
 * Author: Utsava
 *
 * Created on 14 March, 2023, 10:05 AM
 */

#ifndef DIGITAL_ALARM_CLOCK_H
#define	DIGITAL_ALARM_CLOCK_H

void display_dash_screen(unsigned char reset_flag);
unsigned char menu1(unsigned char reset_flag, unsigned char key);
unsigned char menu2(unsigned char reset_flag, unsigned char key);
unsigned char menu3(unsigned char reset_flag, unsigned char key);
void set_time(unsigned char reset_flag, unsigned char key);
void set_date(unsigned char reset_flag, unsigned char key);
void set_event(unsigned char reset_flag, unsigned char key);
void alarm_event(void);
unsigned char display_event(unsigned char reset_flag);
void ring_alarm(void);
void clear_screen(void);
unsigned char dec2bcd(unsigned char num);

#endif	/* DIGITAL_ALARM_CLOCK_H */