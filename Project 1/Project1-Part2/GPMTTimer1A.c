#include "tm4c123gh6pm.h"
#include "GPMTTimer1A.h"

#define TIMER1_DISABLE 	0x00000000
#define TIMER1_ENABLE 	0x00000001
#define TIMER1_16_BIT 	0x00000004
#define TIMER1_PER_TIM 	0x00000002
#define ONE							1
#define TIMER1_ACT			0x02
#define MY_PRESCALE			62						// 62: 0.25/(2^16/16MHz) = 0.004, 0.25/0.004 = 63, PRESCALE = 63-1

void (*PeriodicTask)(void);   // user function

// ***************** Timer0_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1A_Init(void(*task)(void), unsigned long period){
  SYSCTL_RCGCTIMER_R |= TIMER1_ACT;	// 0) activate TIMER0
  PeriodicTask 		= task;         	// user function
  TIMER1_CTL_R 		= TIMER1_DISABLE;  	// 1) disable TIMER0A during setup
  TIMER1_CFG_R 		= TIMER1_16_BIT;  // 2) configure for 16-bit mode
  TIMER1_TAMR_R 	= TIMER1_PER_TIM; // 3) configure for periodic down-count mode
  TIMER1_TAILR_R 	= period-1;    		// 4) reload value
  TIMER1_TAPR_R 	= MY_PRESCALE;         		// 5) bus clock prescale
  TIMER1_ICR_R 		= TIMER1_ENABLE;   	// 6) clear TIMER0A timeout flag
  TIMER1_IMR_R 		= TIMER1_ENABLE;   	// 7) arm timeout interrupt
	
	
//  NVIC_PRI4_R 	= (NVIC_PRI4_R&0x0FFF0FFF)|0x00006000; // 8) priority 3
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
//  NVIC_EN0_R 		= 1<<21;          // 9) enable IRQ 21 in NVIC
//  TIMER1_CTL_R 	= TIMER1_ENABLE;    	// 10) enable TIMER0A
}



void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER0A timeout
  (*PeriodicTask)();                // execute user task
}