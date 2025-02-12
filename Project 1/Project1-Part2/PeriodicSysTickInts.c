// PeriodicSysTickInts.c
// Runs on LM4F120 or TM4C123
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano
// September 14, 2013
// Modified by Min He on 3/14/2024

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
   Volume 1, Program 9.6
   
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013
   Volume 2, Program 5.12, section 5.7

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// oscilloscope or LED connected to PF2 for period measurement

#include "PLL.h"
#include "GPMTTimer1A.h"
#include "SysTick.h"
#include "tm4c123gh6pm.h"
#include <stdint.h> // C99 data types

#define LED             	(*((volatile uint32_t *)0x40025038))    // bit address for red LED
#define RED 					  	0x02  																	// bit position for red LED onboard: PF1
#define RGB								0x0E
#define MY_PRESCALE				62																		// 62: 0.25/(2^16/16MHz) = 0.004, 0.25/0.004 = 63, PRESCALE = 63-1
#define MAX_COUNT_VAL			63492																	
#define FIVE_HUNDRED_MS 	4000000U 															// number of clock cycles to generate 0.1s time interval. 
																																// Assume system clock is 16MHz: t=1600000*(1/f)=1600000*(1/16000000)=0.1s

// Function prototypes
// External functions from startup.s
extern void DisableInterrupts(void); // Disable interrupts
extern void EnableInterrupts(void);  // Enable interrupts
extern void WaitForInterrupt(void);  // low power mode

void PORTF_Init(void);

void UserTask(void){
  LED ^= RED;
}


int main(void){
	DisableInterrupts();
	PLL_Init();
	PORTF_Init();                       
	//Timer0A_Init((&UserTask), MAX_COUNT_VAL);
	Timer1A_Init(&UserTask, MAX_COUNT_VAL);	// initialize timer1 (1 Hz)
	EnableInterrupts();

  while(1){
		// do something else
    WaitForInterrupt();
  }
}

void PORTF_Init(void) {
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;     	// activate F clock
	while ((SYSCTL_RCGCGPIO_R&SYSCTL_RCGCGPIO_R5)!=SYSCTL_RCGCGPIO_R5){} // wait for the clock to be ready
		
  GPIO_PORTF_DIR_R |= RGB;   // make PF2 output (PF2 built-in LED)
  GPIO_PORTF_AFSEL_R &= ~RGB;// disable alt funct on PF2
  GPIO_PORTF_DEN_R |= RGB;   // enable digital I/O on PF2
                              // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R &= ~0x0000FFF0;
  GPIO_PORTF_AMSEL_R &= ~RGB;     // disable analog functionality on PF
}
