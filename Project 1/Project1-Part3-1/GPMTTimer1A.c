#include "tm4c123gh6pm.h"
#include "GPMTTimer1A.h"

#define ONE								1
#define MC_LEN 						0.0625 	// length of one machine cycle in microsecond for 16MHz clock
#define DEFAULT_PRESCALE 	0
#define ONE_uS						4

unsigned long period1=0;

// ***************** Timer1_Init ****************
// Activate TIMER1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1A_Init(unsigned long period){
  SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;			// 0) activate TIMER0
	while ((SYSCTL_RCGCTIMER_R&SYSCTL_RCGCTIMER_R1)!= SYSCTL_RCGCTIMER_R1){};
	period1 				= period;   										// user function
  TIMER1_CTL_R 	 &= ~TIMER_CTL_TAEN;  								// 1) disable TIMER0A during setup
	//TIMER1_CFG_R 		= TIMER_CFG_16_BIT;  							// 2) configure for 16-bit mode
	TIMER1_CFG_R 		= TIMER_CFG_32_BIT_TIMER;						// 2) configure for 32-bit mode
	TIMER1_TAMR_R  |= TIMER_TAMR_TAMR_PERIOD;						// 3) configure for periodic down-count mode
  TIMER1_TAILR_R 	= period-1;    											// 4) reload value
  TIMER1_TAPR_R 	= MY_PRESCALE;      								// 5) bus clock prescale
  TIMER1_ICR_R 	 |= TIMER_ICR_TATOCINT; 							// 6) clear TIMER0A timeout flag
  TIMER1_IMR_R 	 |= TIMER_IMR_TATOIM;   							// 7) arm timeout interrupt
	
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF1FFF)|0x00006000; 	// 8) priority 3
  NVIC_EN0_R			= 1<<21;          								 	// 9) enable IRQ 21 in NVIC,vector number 35, interrupt number 21
  TIMER1_CTL_R	 |= TIMER_CTL_TAEN;    								// 10) enable TIMER0A
}

void Timer1A_1us(uint8_t second){
	TIMER1_CTL_R 	 &= ~TIMER_CTL_TAEN;									// Disable TIMER1A during setup
	TIMER1_CFG_R 		= TIMER_CFG_16_BIT;  								// configure for 16-bit mode
	//TIMER1_CFG_R 		= TIMER_CFG_32_BIT_TIMER;						// 2) configure for 32-bit mode
  TIMER1_TAMR_R   = TIMER_TAMR_TAMR_PERIOD; 					// Periodic Timer mode
	TIMER1_TAILR_R 	= (ONE_uS*second)-1;								// Tick Value for 1uS
	TIMER1_ICR_R    = TIMER_ICR_TATOCINT;    						// Clear timeout flag
	TIMER1_CTL_R	 |= TIMER_CTL_TAEN;										// Enable TIMER1A
	while ((TIMER1_RIS_R & TIMER_CTL_TAEN) == 0);
	TIMER1_ICR_R 		= TIMER_ICR_TATOCINT;
}

void Timer1A_Start(void){
	TIMER1_CTL_R 	 &= ~TIMER_CTL_TAEN;									// Disable TIMER1A during setup
	TIMER1_CFG_R 		= TIMER_CFG_16_BIT;  								// configure for 16-bit mode
	//TIMER1_CFG_R 		= TIMER_CFG_32_BIT_TIMER;					// configure for 32-bit mode
  TIMER1_TAMR_R   = TIMER_TAMR_TAMR_PERIOD; 					// Periodic Timer mode
	TIMER1_TAILR_R 	= (period1)-1;											// Tick Value for 1uS
	TIMER1_ICR_R    = TIMER_ICR_TATOCINT;    						// Clear timeout flag
	TIMER1_CTL_R	 |= TIMER_CTL_TAEN;										// Enable TIMER1A
}

void Timer1A_Stop(void){
	TIMER1_CTL_R 	 &= ~TIMER_CTL_TAEN;									// Disable TIMER1A during setup
}

void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;	// acknowledge TIMER0A timeout
}