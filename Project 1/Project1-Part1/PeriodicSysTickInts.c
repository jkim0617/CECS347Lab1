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
#include "tm4c123gh6pm.h"
#include <stdint.h> // C99 data types

#define LED             	(*((volatile uint32_t *)0x40025038))    // bit address for red LED
#define RED 					  	0x02  																	// bit position for red LED onboard: PF1
#define RGB								0x0E
#define FIVE_HUNDRED_MS 	4000000U 															// number of clock cycles to generate 0.1s time interval. 
																																// Assume system clock is 16MHz: t=1600000*(1/f)=1600000*(1/16000000)=0.1s

// Function prototypes
// External functions from startup.s
extern void DisableInterrupts(void); // Disable interrupts
extern void EnableInterrupts(void);  // Enable interrupts
extern void WaitForInterrupt(void);  // low power mode

// functions defined in this file
void SysTick_Init(uint32_t period);
void PORTF_Init(void);

// global variables 
volatile uint32_t Counts = 0;

int main(void){
	DisableInterrupts();
	PLL_Init();
	PORTF_Init();
  Counts = 0;
  SysTick_Init(FIVE_HUNDRED_MS);  // initialize SysTick timer to generate 0.1s time interval	                              
  EnableInterrupts();

  while(1){                   // interrupts every 1ms, 500 Hz flash
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

void PLL_Init(void){
  // 0) configure the system to use RCC2 for advanced features
  //    such as 400 MHz PLL and non-integer System Clock Divisor
  SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;
  // 1) bypass PLL while initializing
  SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;
  // 2) select the crystal value and oscillator source
  SYSCTL_RCC_R &= ~SYSCTL_RCC_XTAL_M;   // clear XTAL field: 0x7C0=> 011111000000
  SYSCTL_RCC_R += SYSCTL_RCC_XTAL_16MHZ;// configure for 16 MHz crystal: 10101=>0x15
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;// clear oscillator source field
  SYSCTL_RCC2_R += SYSCTL_RCC2_OSCSRC2_MO;// configure for main oscillator source
  // 3) activate PLL by clearing PWRDN
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;
  // 4) set the desired system divider and the system divider least significant bit
  SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;  // use 400 MHz PLL
  SYSCTL_RCC2_R = (SYSCTL_RCC2_R&~0x1FC00000) // clear system clock divider field: setting up bit 22 to 28
	+ (SYSDIV2<<22);      // configure for 50 MHz clock: assign value 7 to 7 bits(0x0000111(hex), 7(dec)), then move it to the right field: bit 22 to 28
  // 5) wait for the PLL to lock by polling PLLLRIS
  while((SYSCTL_RIS_R&SYSCTL_RIS_PLLLRIS)==0){};
  // 6) enable use of PLL by clearing BYPASS
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;
}

// **************SysTick_Init*********************
// Initialize SysTick periodic interrupts
// Input: interrupt period
//        Units of period are 62.5ns (assuming 16 MHz clock)
//        Maximum is 2^24-1
//        Minimum is determined by length of ISR
// Output: none
void SysTick_Init(uint32_t period){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value: if period = 4
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x1FFFFFFF)|0x40000000; // priority 2
  NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC|NVIC_ST_CTRL_INTEN|NVIC_ST_CTRL_ENABLE; // enable SysTick with core clock and interrupts      
}

// Interrupt service routine
// Executed every 62.5ns*(period)
void SysTick_Handler(void){
  LED ^= RED;       // toggle blue LED
}

