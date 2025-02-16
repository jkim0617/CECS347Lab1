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

#include "tm4c123gh6pm.h"
#include "GPMTTimer1A.h"

#define MC_LEN 						0.0625 	// length of one machine cycle in microsecond for 16MHz clock
#define ONE_uS						8

// ***************** Timer1_Init ****************
// Activate TIMER1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1A_Init(unsigned long period){
  SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;					//activate TIMER1
	while ((SYSCTL_RCGCTIMER_R&SYSCTL_RCGCTIMER_R1)!= SYSCTL_RCGCTIMER_R1){};
		
  TIMER1_CTL_R 	 &= ~TIMER_CTL_TAEN;  								// disable TIMER1A during setup
	TIMER1_CFG_R 		= TIMER_CFG_16_BIT;  								// configure for 16-bit mode
	TIMER1_TAMR_R  |= TIMER_TAMR_TAMR_PERIOD;						// configure for periodic count mode
	TIMER1_TAMR_R  &= ~TIMER_TAMR_TACDIR;								// configure for count down
  TIMER1_TAPR_R 	= MY_PRESCALE;      								// bus clock prescalen
	                                                    
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF1FFF)|0x00008000; 	// priority 3
  NVIC_EN0_R			= 1<<21;          								 	// enable IRQ 21 in NVIC,vector number 35, interrupt number 21
}                                                     
                                                      
void Timer1A_1us(uint8_t second){                     
	TIMER1_CTL_R 	 &= ~TIMER_CTL_TAEN;									// Disable TIMER1A during setup
	TIMER1_TAILR_R 	= (ONE_uS*second)-1;								// Tick Value for 1uS
  TIMER1_IMR_R 	 &= ~TIMER_IMR_TATOIM;   							// disable timeout interrupt
	TIMER1_CTL_R	 |= TIMER_CTL_TAEN;										// Enable TIMER1A
	while (TIMER1_TAR_R != 0);        
	TIMER1_CTL_R 	 &= ~TIMER_CTL_TAEN;									// Disable TIMER1A during setup
}                                                     
                                                      
void Timer1A_Start(void){                             
	TIMER1_CTL_R 	 &= ~TIMER_CTL_TAEN;									// Disable TIMER1A during setup
	TIMER1_TAILR_R 	= (MAX_COUNT_VAL)-1;								// Tick Value for 1uS
	TIMER1_ICR_R    = TIMER_ICR_TATOCINT;    						// Clear timeout flag
  TIMER1_IMR_R 	 |= TIMER_IMR_TATOIM;   							// arm timeout interrupt
	TIMER1_CTL_R	 |= TIMER_CTL_TAEN;										// Enable TIMER1A
}                                                     
                                                      
void Timer1A_Stop(void){                              
	TIMER1_CTL_R 	 &= ~TIMER_CTL_TAEN;									// Disable TIMER1A during setup
	TIMER1_IMR_R 	 &= ~TIMER_IMR_TATOIM;   							// disable timeout interrupt
}

uint32_t getTime(void){
	return (TIMER1_TAILR_R - TIMER1_TAR_R);
}
