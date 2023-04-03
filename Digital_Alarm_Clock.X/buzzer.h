/* 
 * File:   buzzer.h
 * Author: Utsava
 *
 * Created on 21 March, 2023, 8:56 AM
 */

#ifndef BUZZER_H
#define	BUZZER_H

#define BUZZER_DDR                 TRISC
#define BUZZER                     PORTC

void init_buzzer(void);
void alarm(void);
void stop_alarm(void);

#endif	/* BUZZER_H */

