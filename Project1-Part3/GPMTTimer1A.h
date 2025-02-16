// Ultrasonic_Sensor.c
// Course number: CECS 347
// Assignment:	Project 1 Part 3
// Project Group #9
// Team Members: Jose Ambriz, Bronson Garel, Jonathan Kim, Kyle Wyckoff
// Date: 02/10/2025
// System Requirements: Windows
// Use the ultra sonic sensor to detect distance up to 100cm.
// The Echo pin is represented on PF4
// The Trigger pin is represented on PF5

#ifndef __TIMER1INTS_H__ // do not include more than once
#define __TIMER1INTS_H__

#include <stdint.h> // C99 data types	

#define MY_PRESCALE			1		 	// 62: 0.25/(2^16/16MHz) = 0.004, 0.25/0.004 = 63, PRESCALE = 63-1
#define OUT_OF_RANGE		200
#define MAX_COUNT_VAL		46635		// Value for our max distance we want from our Ultrasonic Sensor

// ***************** Timer0_Init ****************
// Activate Timer0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1A_Init(unsigned long period);
void Timer1A_Start(void);
void Timer1A_Stop(void);
void Timer1A_1us(uint8_t second);
uint32_t getTime(void);

#endif // __TIMER2INTS_H__