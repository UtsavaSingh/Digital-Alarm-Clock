#include <xc.h>
#include "main.h"

extern char sec, min, dur_sec;
int return_time;

void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1)
    {
        if (++count == 1250)
        {
            count = 0;
            if(sec > 0)
                sec--;
            if(dur_sec > 0)
                dur_sec--;
            if(sec == 0 && return_time > 0)
                return_time--;
            if(min > 0 && dur_sec == 0)
            {
                min--;
                if(min > 0)
                    dur_sec = 60;
                else
                    dur_sec = 0;
            }
        }
        
        TMR2IF = 0;
    }
}