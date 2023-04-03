/*
 * File:   main.c
 * Author: Utsava
 * Description: Digital alarm clock project 
 * Created on 20 March, 2023, 7:58 AM
 */


#include "main.h"

#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)

unsigned char second, minute, hour;
unsigned char day, month, year;
unsigned char new_event[10], event;

static void init_config(void) {
    // initialize i2c 
    init_i2c(100000); //100K
    // initialize ds1307
    init_ds1307();
    // initialize digital keypad
    init_digital_keypad();
    // initialize clcd
    init_clcd();
    // initialize timer2
    init_timer2();
    // initialize buzzer
    init_buzzer();
  
    GIE = 1;
    PEIE = 1;
}

void main(void) {
    // Variable declaration
    unsigned char control_flag = SET_EVENT_FLAG, reset_flag = RESET_SET_EVENT;    // Dash Board Screen
    unsigned char key, key_level;
    unsigned char menu_pos;
    unsigned int delay1 = 0, delay2 = 0, delay3 = 0, delay4 = 0, delay5 = 0, delay6 = 0;
    init_config();
    while (1) {
        // Reading key press
        key = read_digital_keypad(STATE);
        key_level = read_digital_keypad(LEVEL);
        
        if((control_flag == DASH_BOARD_FLAG) && (key == SW4 || key == SW5))
        {
            // Config Screen
            clear_screen();
            control_flag = MENU1_FLAG;
            reset_flag = RESET_MENU1;
        }
        else if((control_flag == MENU1_FLAG) && (key_level == SW4) && (delay1++ == 20))
        {
            // Long press of up key to select a particular option from login menu
            delay1 = 0;
            switch(menu_pos)
            {
                case 0 :
                    // menu2
                    control_flag = MENU2_FLAG;
                    reset_flag = RESET_MENU2;
                    clear_screen();
                    break;
                case 1 :
                    // menu2
                    control_flag = MENU3_FLAG;
                    reset_flag = RESET_MENU3;
                    clear_screen();
                    break;
            }
        }
        else if((control_flag == MENU2_FLAG) && (key_level == SW4) && (delay2++ == 20))
        {
            // Long press of up key to select a particular option from login menu
            delay2 = 0;
            switch(menu_pos)
            {
                case 0 :
                    // menu2
                    control_flag = SET_EVENT_FLAG;
                    reset_flag = RESET_SET_EVENT;
                    clear_screen();
                    break;
                case 1 :
                    // menu2
                    control_flag = VIEW_EVENT_FLAG;
                    reset_flag = RESET_VIEW_EVENT;
                    clear_screen();
                    break;
            }
        }
        else if((control_flag == MENU3_FLAG) && (key_level == SW4) && (delay3++ == 20))
        {
            // Long press of up key to select a particular option from login menu
            delay3 = 0;
            switch(menu_pos)
            {
                case 0 :
                    // menu2
                    control_flag = SET_TIME_FLAG;
                    reset_flag = RESET_TIME;
                    clear_screen();
                    break;
                case 1 :
                    // menu2
                    control_flag = SET_DATE_FLAG;
                    reset_flag = RESET_DATE;
                    clear_screen();
                    break;
            }
        }
        else if((control_flag == MENU1_FLAG) && (key_level == SW5) && (delay4++ == 20))
        {
            // Long press of up key to select a particular option from login menu
            delay4 = 0;
            control_flag = DASH_BOARD_FLAG;
            reset_flag = RESET_DASH_SCREEN;
        }
        else if((control_flag == MENU2_FLAG) && (key_level == SW5) && (delay5++ == 20))
        {
            // Long press of up key to select a particular option from login menu
            delay5 = 0;
            control_flag = MENU1_FLAG;
            reset_flag = RESET_MENU1;
        }
        else if((control_flag == MENU3_FLAG) && (key_level == SW5) && (delay6++ == 20))
        {
            // Long press of up key to select a particular option from login menu
            delay6 = 0;
            control_flag = MENU1_FLAG;
            reset_flag = RESET_MENU1;
        }
        else if((control_flag == SET_TIME_FLAG) && (key_level == SW5) && (delay1++ == 20))
        {
            // Long press of up key to select a particular option from login menu
            delay1 = 0;
            control_flag = MENU3_FLAG;
            reset_flag = RESET_MENU3;
            // Setting new time
            write_ds1307(SEC_ADDR, dec2bcd(second));
            write_ds1307(MIN_ADDR, dec2bcd(minute));
            write_ds1307(HOUR_ADDR, dec2bcd(hour));
            clear_screen();
        }
        else if((control_flag == SET_DATE_FLAG) && (key_level == SW5) && (delay2++ == 20))
        {
            // Long press of up key to select a particular option from login menu
            delay2 = 0;
            control_flag = MENU3_FLAG;
            reset_flag = RESET_MENU3;
            // Setting new date
            write_ds1307(YEAR_ADDR, dec2bcd(year));
            write_ds1307(MONTH_ADDR, dec2bcd(month));
            write_ds1307(DATE_ADDR, dec2bcd(day));
            clear_screen();
        }
        else if((control_flag == SET_EVENT_FLAG) && (key_level == SW5) && (delay3++ == 20))
        {
            // Long press of up key to select a particular option from login menu
            delay3 = 0;
            control_flag = MENU2_FLAG;
            reset_flag = RESET_MENU2;
            alarm_event();
            clear_screen();
        }
        else if((control_flag == VIEW_EVENT_FLAG) && (key_level == SW5) && (delay1++ == 20))
        {
            // Long press of up key to select a particular option from login menu
            delay1 = 0;
            control_flag = MENU2_FLAG;
            reset_flag = RESET_MENU2;
            clear_screen();
        }
        
        // Selecting the particular operations depending on control flag
        switch(control_flag)
        {
            // Display dash board
            case DASH_BOARD_FLAG:
            {
                display_dash_screen(reset_flag);
                break;
            }
            // Display menu1
            case MENU1_FLAG :
            {
                /* Switching on the Timer2 */
                TMR2ON = 1;
                menu_pos = menu1(reset_flag, key);
                if(menu_pos == RETURN_BACK)
                {
                    control_flag = DASH_BOARD_FLAG;
                    reset_flag = RESET_DASH_SCREEN;
                    clear_screen();
                    continue;
                }
                break;
            }
            // Display menu2
            case MENU2_FLAG :
            {
                /* Switching on the Timer2 */
                TMR2ON = 1;
                menu_pos = menu2(reset_flag, key);
                if(menu_pos == RETURN_BACK)
                {
                    control_flag = MENU1_FLAG;
                    reset_flag = RESET_MENU1;
                    clear_screen();
                    continue;
                }
                break;
            }
            // Display menu3
            case MENU3_FLAG :
            {
                /* Switching on the Timer2 */
                TMR2ON = 1;
                menu_pos = menu3(reset_flag, key);
                if(menu_pos == RETURN_BACK)
                {
                    control_flag = MENU1_FLAG;
                    reset_flag = RESET_MENU1;
                    clear_screen();
                    continue;
                }
                break;
            }
            // Display Set Time Screen
            case SET_TIME_FLAG :
            {
                /* Switching off the Timer2 */
                TMR2ON = 0;
                set_time(reset_flag, key);
                break;
            }
            // Display Set Date Screen
            case SET_DATE_FLAG :
            {
                /* Switching off the Timer2 */
                TMR2ON = 0;
                set_date(reset_flag, key);
                break;
            }
            // Display Set Event Screen
            case SET_EVENT_FLAG :
            {
                /* Switching off the Timer2 */
                TMR2ON = 0;
                set_event(reset_flag, key);
                break;
            }
            // Display View Event Screen
            case VIEW_EVENT_FLAG :
            {
                /* Switching off the Timer2 */
                TMR2ON = 1;
                if(display_event(reset_flag) == RETURN_BACK)
                {
                    control_flag = MENU1_FLAG;
                    reset_flag = RESET_MENU1;
                    clear_screen();
                    continue;
                }
                break;
            }
        }
        reset_flag = RESET_NOTHING;
    }
    return;
}
