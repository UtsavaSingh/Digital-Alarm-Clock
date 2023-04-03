/*
 * File:   digital_alarm_clock_def.c
 * Author: Utsava
 *
 * Created on 20 March, 2023, 8:07 AM
 */

#include "main.h"

// variable declaration
char clock_reg[3]; // HH MM SS 
char date_reg[4];  // DD MM YY
char *menu_1[] = {"SET/VIEW EVENT", "SET TIME/DATE"};
char *menu_2[] = {"SET EVENT", "VIEW EVENT"};
char *menu_3[] = {"SET TIME", "SET DATE"};
const char times[] = "ODW";
char time[9], second_time[3];  // "HH:MM:SS"
char date[10];  // "DD-MM-YY"
char log[11];
char sec = 0, dur_sec = 0, min = 0, PM;
extern unsigned char new_event[12], event;
extern int return_time;
extern unsigned char second, minute, hour;
extern unsigned char day, month, year;

/*Function to get time from ds1307*/
void get_time(void)
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
}

/*Function to get date from ds1307*/
void get_date(void)
{
    date_reg[0] = read_ds1307(DATE_ADDR); // DD -> BCD 
    date_reg[1] = read_ds1307(MONTH_ADDR); // MM -> BCD 
    date_reg[2] = read_ds1307(YEAR_ADDR); // YY -> BCD
    date_reg[3] = read_ds1307(DAY_ADDR); // YY -> BCD
}

/*Function to display time on the clcd display*/
void display_time(void)
{
    get_time(); // HH MM SS  
   
    /* To store the time in HH:MM:SS format */
    // HH -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    
    hour = (time[0]-'0')*10+(time[1]-'0');
    if(hour > 12)
    {
        hour = hour - 12;
        time[0] = hour/10 + '0';
        time[1] = hour%10 + '0';
        PM = 1;
    }
    else
    {
        PM = 0;
    }
    time[2] = ':';
    // MM 
    time[3] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[4] = (clock_reg[1] & 0x0F) + '0';
    time[5] = ' ';
    // AM/PM
    if(PM)
    {
        time[6] = 'P';
        time[7] = 'M';
    }
    else
    {
        time[6] = 'A';
        time[7] = 'M';
    }
    time[8] = '\0';
    // SS 
    second_time[0] = ((clock_reg[2] >> 4) & 0x07) + '0';
    second_time[1] = (clock_reg[2] & 0x0F) + '0';
    second_time[2] = '\0';
}

void display_date(void)
{
    get_date(); // DD MM YY  
   
    /* To store the date in DD:MM:YY format */
    // DD -> 
    date[0] = ((date_reg[0] >> 4) & 0x03) + '0';
    date[1] = (date_reg[0] & 0x0F) + '0';
    
    date[2] = '-';
    // MM 
    date[3] = ((date_reg[1] >> 4) & 0x01) + '0';
    date[4] = (date_reg[1] & 0x0F) + '0';
    
    date[5] = '-';
    // YY 
    date[6] = ((date_reg[2] >> 4) & 0x0F) + '0';
    date[7] = (date_reg[2] & 0x0F) + '0';
    date[8] = '\0';
    date[9] = (date_reg[3] & 0x07) + '0';
}

/*Function to display Dash Board Screen*/
void display_dash_screen(unsigned char reset_flag)
{
    if(reset_flag == RESET_DASH_SCREEN)
    {
        return_time = 5;
        sec = 0;
        TMR2ON = 1;
        clear_screen();
        new_event[0] = read_EEPROM(0x01); 
        new_event[1] = read_EEPROM(0x02); 
        new_event[2] = ':'; 
        new_event[3] = read_EEPROM(0x03); 
        new_event[4] = read_EEPROM(0x04);
        new_event[5] = ' ';
        new_event[6] = read_EEPROM(0x05); 
        new_event[7] = read_EEPROM(0x06); 
        new_event[8] = ' ';
        new_event[9] = read_EEPROM(0x09);
    }
    event = read_EEPROM(0x00) - '0'; 
    if(event)
    {
        if(return_time == 0)
        {
            sec = 2;
            return_time = 5;
        }
        
        if(sec)
        {
            clcd_print("TIME  ", LINE1(0));
            // HH:MM:SS
            clcd_print(time, LINE1(6));
            clcd_print("EVENT ", LINE2(0));
             
            clcd_print(new_event, LINE2(6));
        }
        else
        {
            clcd_print("DATE  ", LINE1(0));
            // DD:MM:YY
            clcd_print(date, LINE1(6));
            clcd_print("TIME  ", LINE2(0));
            // HH:MM:SS
            clcd_print(time, LINE2(6));
            clcd_putch(' ', LINE2(15));
        }      
    }
    else
    {
        clcd_print("DATE  ", LINE1(0));
        // DD:MM:YY
        clcd_print(date, LINE1(6));
        clcd_print("TIME  ", LINE2(0));
        // HH:MM:SS
        clcd_print(time, LINE2(6));
        clcd_putch(' ', LINE2(15));
    }
    display_time(); // HH:MM:SS --> 13:14:15
    display_date(); // DD:MM:YY --> 27:03:23
    if(event)
        ring_alarm();
}

/*Function to Show login menu*/
unsigned char menu1(unsigned char reset_flag, unsigned char key)
{
    static char menu_pos;
    if(reset_flag == RESET_MENU1)
    {
        menu_pos = 0;
        clear_screen();
        return_time = 5;
        key = ALL_RELEASED;
    }
    
    if(return_time == 0)
        return RETURN_BACK;
    
    if(key == SW5 && menu_pos < 1)
    {
        menu_pos++;
        return_time = 5;
    }
    else if(key == SW4 && menu_pos > 0)
    {
        menu_pos--;
        return_time = 5;
    }
    if(menu_pos == 0)
    {
        clcd_putch(0x3E, LINE1(0));
        clcd_putch(' ', LINE2(0));
    }
    else if(menu_pos == 1)
    {
        clcd_putch(' ', LINE1(0));
        clcd_putch(0x3E, LINE2(0));
    }
    clcd_print(menu_1[0], LINE1(2));
    clcd_print(menu_1[1], LINE2(2));
    return menu_pos;
}

/*Function to Show login menu*/
unsigned char menu2(unsigned char reset_flag, unsigned char key)
{
    static char menu_pos;
    if(reset_flag == RESET_MENU2)
    {
        menu_pos = 0;
        clear_screen();
        return_time = 5;
        key = ALL_RELEASED;
    }
    
    if(return_time == 0)
        return RETURN_BACK;
    
    if(key == SW5 && menu_pos < 1)
    {
        menu_pos++;
        return_time = 5;
    }
    else if(key == SW4 && menu_pos > 0)
    {
        menu_pos--;
        return_time = 5;
    }
    if(menu_pos == 0)
    {
        clcd_putch(0x3E, LINE1(0));
        clcd_putch(' ', LINE2(0));
    }
    else if(menu_pos == 1)
    {
        clcd_putch(' ', LINE1(0));
        clcd_putch(0x3E, LINE2(0));
    }
    clcd_print(menu_2[0], LINE1(2));
    clcd_print(menu_2[1], LINE2(2));
    return menu_pos;
}

/*Function to Show login menu*/
unsigned char menu3(unsigned char reset_flag, unsigned char key)
{
    static char menu_pos;
    if(reset_flag == RESET_MENU3)
    {
        menu_pos = 0;
        clear_screen();
        return_time = 5;
        key = ALL_RELEASED;
    }
    
    if(return_time == 0)
        return RETURN_BACK;
    
    if(key == SW5 && menu_pos < 1)
    {
        menu_pos++;
        return_time = 5;
    }
    else if(key == SW4 && menu_pos > 0)
    {
        menu_pos--;
        return_time = 5;
    }
    if(menu_pos == 0)
    {
        clcd_putch(0x3E, LINE1(0));
        clcd_putch(' ', LINE2(0));
    }
    else if(menu_pos == 1)
    {
        clcd_putch(' ', LINE1(0));
        clcd_putch(0x3E, LINE2(0));
    }
    clcd_print(menu_3[0], LINE1(2));
    clcd_print(menu_3[1], LINE2(2));
    return menu_pos;
}

/*Function to set new time*/
void set_time(unsigned char reset_flag, unsigned char key)
{
    static unsigned char second_flag, minute_flag, hour_flag, delay, flag;
    
    static char new_time[7];  // "HH:MM:SS"
    if(reset_flag == RESET_TIME)
    {
        key = ALL_RELEASED;
        second_flag = 1;
        minute_flag = 0;
        hour_flag = 0;
        delay = 0;
        clear_screen();
        clcd_print("    HH:MM:SS    ", LINE1(0));
        get_time(); // HH MM SS  
   
        /* To store the time in HH:MM:SS format */
        // HH -> 
        time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
        time[1] = (clock_reg[0] & 0x0F) + '0';
    
        // MM 
        time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
        time[3] = (clock_reg[1] & 0x0F) + '0';
    
        // SS
        time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
        time[5] = (clock_reg[2] & 0x0F) + '0';
        time[6] = '\0';
        // HH:MM:SS
        clcd_putch(time[0], LINE2(4)); 
        clcd_putch(time[1], LINE2(5));
        clcd_putch(':', LINE2(6)); 
        clcd_putch(time[2], LINE2(7)); 
        clcd_putch(time[3], LINE2(8));
        clcd_putch(':', LINE2(9)); 
        clcd_putch(time[4], LINE2(10)); 
        clcd_putch(time[5], LINE2(11));
        hour = (time[0]-'0')*10+(time[1]-'0');
        minute = (time[2]-'0')*10+(time[3]-'0');
        second = (time[4]-'0')*10+(time[5]-'0');
    }
    
    
    // Increment
    if(key == SW4)
    {
        if(minute_flag)
        {
            ++minute;
            if(minute > 60)
                minute = 0;
        }
        else if(hour_flag)
        {
            ++hour;
            if(hour > 24)
                hour = 0;
        }
        else if(second_flag)
        {
            ++second;
            if(second > 60)
                second = 0;
        }
    }
    // Choose field
    else if(key == SW5)
    {
        if(second_flag)
        {
            hour_flag = 0;
            minute_flag = 1;
            second_flag = 0;
        }
        else if(minute_flag)
        {
            hour_flag = 1;
            minute_flag = 0;
            second_flag = 0;
        }
        else if(hour_flag)
        {
            hour_flag = 0;
            minute_flag = 0;
            second_flag = 1;
        }
    }
        
    // set mode
    if(second_flag)
    {
        if(flag)
        {
            new_time[4] = (second / 10) % 10 + '0';
            new_time[5] = (second / 1) % 10 + '0';
        }
        else
        {
            new_time[4] =  ' ';
            new_time[5] =  ' ';
        }
    }
    else
    {
        new_time[4] = (second / 10) % 10 + '0';
        new_time[5] = (second / 1) % 10 + '0';
    }
    if(minute_flag)
    {
        if(flag)
        {
            new_time[2] = (minute / 10) % 10 + '0';
            new_time[3] = (minute / 1) % 10 + '0';
        }
        else
        {
            new_time[2] =  ' ';
            new_time[3] =  ' ';
        }
    }
    else
    {
        new_time[2] = (minute / 10) % 10 + '0';
        new_time[3] = (minute / 1) % 10 + '0';
    }
            
    if(hour_flag)
    {
        if(flag)
        {
            new_time[0] = (hour / 10) % 10 + '0';
            new_time[1] = (hour / 1) % 10 + '0';
        }
        else
        {
            new_time[0] =  ' ';
            new_time[1] =  ' ';
        }
    }
    else
    {
        new_time[0] =  (hour / 10) % 10 + '0';
        new_time[1] =  (hour / 1) % 10 + '0';
    }
    
    // HH:MM:SS
    clcd_putch(new_time[0], LINE2(4)); 
    clcd_putch(new_time[1], LINE2(5));
    clcd_putch(':', LINE2(6)); 
    clcd_putch(new_time[2], LINE2(7)); 
    clcd_putch(new_time[3], LINE2(8));
    clcd_putch(':', LINE2(9)); 
    clcd_putch(new_time[4], LINE2(10)); 
    clcd_putch(new_time[5], LINE2(11));
    
    // logic to blink selected field
    if(delay++ == 8)
    {
        delay = 0;
        flag = !flag;
    }
}

/*Function to set new date*/
void set_date(unsigned char reset_flag, unsigned char key)
{
    static unsigned char day_flag, month_flag, year_flag, delay, flag;
    
    static char new_date[7];  // "DD:MM:YY"
    if(reset_flag == RESET_DATE)
    {
        key = ALL_RELEASED;
        day_flag = 1;
        month_flag = 0;
        year_flag = 0;
        delay = 0;
        clear_screen();
        clcd_print("    DD-MM-YY    ", LINE1(0));
        get_date(); // DD MM YY  
   
        /* To store the time in HH:MM:SS format */
        // HH -> 
        date[0] = ((date_reg[0] >> 4) & 0x03) + '0';
        date[1] = (date_reg[0] & 0x0F) + '0';
    
        // MM 
        date[2] = ((date_reg[1] >> 4) & 0x07) + '0';
        date[3] = (date_reg[1] & 0x0F) + '0';
    
        // SS
        date[4] = ((date_reg[2] >> 4) & 0x07) + '0';
        date[5] = (date_reg[2] & 0x0F) + '0';
        date[6] = '\0';
        // HH:MM:SS
        clcd_putch(date[0], LINE2(4)); 
        clcd_putch(date[1], LINE2(5));
        clcd_putch('-', LINE2(6)); 
        clcd_putch(date[2], LINE2(7)); 
        clcd_putch(date[3], LINE2(8));
        clcd_putch('-', LINE2(9)); 
        clcd_putch(date[4], LINE2(10)); 
        clcd_putch(date[5], LINE2(11));
        day = (date[0]-'0')*10+(date[1]-'0');
        month = (date[2]-'0')*10+(date[3]-'0');
        year = (date[4]-'0')*10+(date[5]-'0');
    }
    
    
    // Increment
    if(key == SW4)
    {
        if(month_flag)
        {
            ++month;
            if(month > 12)
                month = 0;
        }
        else if(day_flag)
        {
            ++day;
            if(day > 31)
                day = 0;
        }
        else if(year_flag)
        {
            ++year;
            if(year > 99)
                year = 0;
        }
    }
    // Choose field
    else if(key == SW5)
    {
        if(year_flag)
        {
            day_flag = 1;
            month_flag = 0;
            year_flag = 0;
        }
        else if(month_flag)
        {
            day_flag = 0;
            month_flag = 0;
            year_flag = 1;
        }
        else if(day_flag)
        {
            day_flag = 0;
            month_flag = 1;
            year_flag = 0;
        }
    }
        
    // set mode
    if(year_flag)
    {
        if(flag)
        {
            new_date[4] = (year / 10) % 10 + '0';
            new_date[5] = (year / 1) % 10 + '0';
        }
        else
        {
            new_date[4] =  ' ';
            new_date[5] =  ' ';
        }
    }
    else
    {
        new_date[4] = (year / 10) % 10 + '0';
        new_date[5] = (year / 1) % 10 + '0';
    }
    if(month_flag)
    {
        if(flag)
        {
            new_date[2] = (month / 10) % 10 + '0';
            new_date[3] = (month / 1) % 10 + '0';
        }
        else
        {
            new_date[2] =  ' ';
            new_date[3] =  ' ';
        }
    }
    else
    {
        new_date[2] = (month / 10) % 10 + '0';
        new_date[3] = (month / 1) % 10 + '0';
    }
            
    if(day_flag)
    {
        if(flag)
        {
            new_date[0] = (day / 10) % 10 + '0';
            new_date[1] = (day / 1) % 10 + '0';
        }
        else
        {
            new_date[0] =  ' ';
            new_date[1] =  ' ';
        }
    }
    else
    {
        new_date[0] =  (day / 10) % 10 + '0';
        new_date[1] =  (day / 1) % 10 + '0';
    }
    
    // HH:MM:SS
    clcd_putch(new_date[0], LINE2(4)); 
    clcd_putch(new_date[1], LINE2(5));
    clcd_putch('-', LINE2(6)); 
    clcd_putch(new_date[2], LINE2(7)); 
    clcd_putch(new_date[3], LINE2(8));
    clcd_putch('-', LINE2(9)); 
    clcd_putch(new_date[4], LINE2(10)); 
    clcd_putch(new_date[5], LINE2(11));
    
    // logic to blink selected field
    if(delay++ == 8)
    {
        delay = 0;
        flag = !flag;
    }
}

/*Function to set new event*/
void set_event(unsigned char reset_flag, unsigned char key)
{
    static unsigned char am_pm_flag, minute_flag, hour_flag, delay, flag;
    static unsigned char duration_flag, times_flag, i, duration;
    
    if(reset_flag == RESET_SET_EVENT)
    {
        key = ALL_RELEASED;
        am_pm_flag = 0;
        minute_flag = 0;
        hour_flag = 1;
        duration_flag = 0;
        times_flag = 0;
        delay = 0;
        clear_screen();
        clcd_print("TIME- ", LINE1(0));
        clcd_print("DUR - ", LINE2(0));
        get_time(); // HH MM SS  
   
        /* To store the time in HH:MM:SS format */
        // HH -> 
        time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
        time[1] = (clock_reg[0] & 0x0F) + '0';
        
        hour = (time[0]-'0')*10+(time[1]-'0');
        if(hour > 12)
        {
            hour = hour - 12;
            time[0] = hour/10 + '0';
            time[1] = hour%10 + '0';
            PM = 1;
        }
        else
        {
            PM = 0;
        }
    
        // MM 
        time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
        time[3] = (clock_reg[1] & 0x0F) + '0';
    
        // AM/PM
        if(PM)
        {
            time[4] = 'P';
            time[5] = 'M';
        }
        else
        {
            time[4] = 'A';
            time[5] = 'M';
        }
        time[6] = '\0';
        
        hour = (time[0]-'0')*10+(time[1]-'0');
        minute = (time[2]-'0')*10+(time[3]-'0');
    }
    
    
    // Increment
    if(key == SW4)
    {
        if(minute_flag)
        {
            ++minute;
            if(minute > 60)
                minute = 0;
        }
        else if(hour_flag)
        {
            ++hour;
            if(hour > 12)
                hour = 0;
        }
        else if(am_pm_flag)
        {
            PM = !PM;
        }
        else if(duration_flag)
        {
            ++duration;
            if(duration > 60)
                duration = 0;
        }
        else if(times_flag)
        {
            ++i;
            if(i > 2)
                i = 0;
        }
    }
    // Choose field
    else if(key == SW5)
    {
        if(am_pm_flag)
        {
            hour_flag = 0;
            minute_flag = 0;
            am_pm_flag = 0;
            duration_flag = 1;
            times_flag = 0;
        }
        else if(minute_flag)
        {
            hour_flag = 0;
            minute_flag = 0;
            am_pm_flag = 1;
            duration_flag = 0;
            times_flag = 0;
        }
        else if(hour_flag)
        {
            hour_flag = 0;
            minute_flag = 1;
            am_pm_flag = 0;
            duration_flag = 0;
            times_flag = 0;
        }
        else if(duration_flag)
        {
            hour_flag = 0;
            minute_flag = 0;
            am_pm_flag = 0;
            duration_flag = 0;
            times_flag = 1;
        }
        else if(times_flag)
        {
            hour_flag = 1;
            minute_flag = 0;
            am_pm_flag = 0;
            duration_flag = 0;
            times_flag = 0;
        }
    }
        
    // set mode
    if(am_pm_flag)
    {
        if(flag)
        {
            // AM/PM
            if(PM)
            {
                new_event[4] = 'P';
                new_event[5] = 'M';
            }
            else
            {
                new_event[4] = 'A';
                new_event[5] = 'M';
            }
            clcd_putch(new_event[4], LINE1(12)); 
            clcd_putch(new_event[5], LINE1(13));
        }
        else
        {
            clcd_putch(' ', LINE1(12)); 
            clcd_putch(' ', LINE1(13));
        }
    }
    else
    {
        // AM/PM
        if(PM)
        {
            new_event[4] = 'P';
            new_event[5] = 'M';
        }
        else
        {
            new_event[4] = 'A';
            new_event[5] = 'M';
        }
        clcd_putch(new_event[4], LINE1(12)); 
        clcd_putch(new_event[5], LINE1(13));
    }
    if(minute_flag)
    {
        if(flag)
        {
            new_event[2] = (minute / 10) % 10 + '0';
            new_event[3] = (minute / 1) % 10 + '0';
            clcd_putch(new_event[2], LINE1(9)); 
            clcd_putch(new_event[3], LINE1(10));
        }
        else
        {
            clcd_putch(' ', LINE1(9)); 
            clcd_putch(' ', LINE1(10));
        }
    }
    else
    {
        new_event[2] = (minute / 10) % 10 + '0';
        new_event[3] = (minute / 1) % 10 + '0';
        clcd_putch(new_event[2], LINE1(9)); 
        clcd_putch(new_event[3], LINE1(10));
    }
            
    if(hour_flag)
    {
        if(flag)
        {
            new_event[0] = (hour / 10) % 10 + '0';
            new_event[1] = (hour / 1) % 10 + '0';
            clcd_putch(new_event[0], LINE1(6)); 
            clcd_putch(new_event[1], LINE1(7));
        }
        else
        {
            clcd_putch(' ', LINE1(6)); 
            clcd_putch(' ', LINE1(7));
        }
    }
    else
    {
        new_event[0] =  (hour / 10) % 10 + '0';
        new_event[1] =  (hour / 1) % 10 + '0';
        clcd_putch(new_event[0], LINE1(6)); 
        clcd_putch(new_event[1], LINE1(7));
    }
    
    if(duration_flag)
    {
        if(flag)
        {
            new_event[6] = (duration / 10) % 10 + '0';
            new_event[7] = (duration / 1) % 10 + '0';
            clcd_putch(new_event[6], LINE2(9)); 
            clcd_putch(new_event[7], LINE2(10));
        }
        else
        {
            clcd_putch(' ', LINE2(9)); 
            clcd_putch(' ', LINE2(10));
        }
    }
    else
    {
        new_event[6] =  (duration / 10) % 10 + '0';
        new_event[7] =  (duration / 1) % 10 + '0';
        clcd_putch(new_event[6], LINE2(9)); 
        clcd_putch(new_event[7], LINE2(10));
    }
    
    if(times_flag)
    {
        if(flag)
        {
            new_event[8] = times[i];
            clcd_putch(new_event[8], LINE2(15));
        }
        else
        {
            clcd_putch(' ', LINE2(15));
        }
    }
    else
    {
        new_event[8] =  times[i];
        clcd_putch(new_event[8], LINE2(15));
    }
    new_event[9] = date[9];
    new_event[10] = '\0';
    
    // HH:MM:AM/PM
    
    clcd_putch(':', LINE1(8)); 
    clcd_putch(' ', LINE1(11)); 
    clcd_putch(0x2F, LINE1(15));
    clcd_putch('0', LINE2(6)); 
    clcd_putch('0', LINE2(7));
    clcd_putch(':', LINE2(8)); 
    clcd_putch(':', LINE2(11)); 
    clcd_putch('0', LINE2(12)); 
    clcd_putch('0', LINE2(13));
    
    
    // logic to blink selected field
    if(delay++ == 8)
    {
        delay = 0;
        flag = !flag;
    }
}

/*Function to store alarm event into eeprom*/
void alarm_event(void)
{
    byte_write_EEPROM(0x00, '1');
    str_write_EEPROM(0x01, new_event); 
}

unsigned char display_event(unsigned char reset_flag)
{
    if(reset_flag == RESET_VIEW_EVENT)
    {
        return_time = 5;
        clear_screen();
        clcd_print("# T - ", LINE1(0));
        clcd_print("0 D - ", LINE2(0));
        clcd_putch(read_EEPROM(0x01), LINE1(6));
        clcd_putch(read_EEPROM(0x02), LINE1(7));
        clcd_putch(':', LINE1(8));
        clcd_putch(read_EEPROM(0x03), LINE1(9));
        clcd_putch(read_EEPROM(0x04), LINE1(10));
        clcd_putch(' ', LINE1(11));
        clcd_putch(read_EEPROM(0x05), LINE1(12));
        clcd_putch(read_EEPROM(0x06), LINE1(13));
        clcd_putch('/', LINE1(15));
        
        clcd_putch('0', LINE2(6));
        clcd_putch('0', LINE2(7));
        clcd_putch(':', LINE2(8));
        clcd_putch(read_EEPROM(0x07), LINE2(9));
        clcd_putch(read_EEPROM(0x08), LINE2(10));
        clcd_putch(':', LINE2(11));
        clcd_putch('0', LINE2(12));
        clcd_putch('0', LINE2(13));
        clcd_putch(read_EEPROM(0x09), LINE2(15));
    }
    
    if(return_time == 0)
        return RETURN_BACK;
    
    return NOTHING;
}

/*Function to ring alarm*/
void ring_alarm(void)
{
    static char alarm_flag = 0;
    if((strncmp(new_event, time, 7) == 0) && (strncmp(second_time, "00", 2) == 0))
    {
        dur_sec = 60;
        min = (read_EEPROM(0x07)-'0')*10+(read_EEPROM(0x08)-'0');
        alarm_flag = 1;
    }
    
    
    
    if(alarm_flag)
    {
        if(new_event[9] == 'D')
        {
            if(min)
                alarm();
            else
            {
                stop_alarm();
                alarm_flag = 0;
            }
        }
        else if(new_event[9] == 'O')
        {
            if(min)
                alarm();
            else
            {
                stop_alarm();
                byte_write_EEPROM(0x00, '0');
                alarm_flag = 0;
            }
        }
        else if(new_event[9] == 'W')
        {
            if(date[9] == read_EEPROM(0x10))
            {
                if(min)
                    alarm();
                else
                {
                    stop_alarm();
                    alarm_flag = 0;
                }
            }
        }
    }
}

/*Function to clear clcd screen*/
void clear_screen(void)
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
}

/*Function to convert decimal number into BCD*/
unsigned char dec2bcd(unsigned char num)
{
    unsigned char ones = 0;
    unsigned char tens = 0;
    unsigned char temp = 0;
    
    ones = num % 10;
    temp = num / 10;
    tens = temp<<4;
    
    return (tens + ones);
}
