#ifndef __TIMER1INTS_H__ // do not include more than once
#define __TIMER1INTS_H__

#include <stdint.h> // C99 data types	

#define MY_PRESCALE				2			 	// 62: 0.25/(2^16/16MHz) = 0.004, 0.25/0.004 = 63, PRESCALE = 63-1

// ***************** Timer0_Init ****************
// Activate Timer0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1A_Init(unsigned long period);
void Timer1A_Start(void);
void Timer1A_Stop(void);
void Timer1A_1us(uint8_t second);

#endif // __TIMER2INTS_H__