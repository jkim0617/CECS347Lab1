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

#include <stdint.h>
#include "SysTick.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "GPMTTimer1A.h"

#define TRIGGER_PIN 		(*((volatile unsigned long *)0x40005080))  // PB5 is the trigger pin	
#define TRIGGER_VALUE 	0x20   // trigger at bit 5
#define ECHO_PIN 				(*((volatile unsigned long *)0x40005040))  // PB4 is the echo pin	
#define ECHO_VALUE 			0x10   // echo at bit 4
#define SENSOR_VALUE		0x30	

#define MC_LEN 					0.0625 // length of one machine cycle in microsecond for 16MHz clock
#define SOUND_SPEED 		0.0343 // centimeter per micro-second
#define MAX_COUNT_VAL		46400		// Value for our max distance we want from our Ultrasonic Sensor
#define PORTB_INT				0x00000002

extern void EnableInterrupts(void);
extern void GPIOPortB_Handler(void);
void PortB_Init(void);
void Timer1A_Handler(void);
void Delay(void);

static volatile uint8_t done=0;
static volatile uint32_t distance=0;
unsigned long time_elapsed=0;

int main(void){
	Timer1A_Init(MAX_COUNT_VAL);
	PortB_Init();
	PLL_Init();
  EnableInterrupts();
  while(1){
		done = 0;
		distance = 0;
		TRIGGER_PIN &= ~TRIGGER_VALUE; // send low to trigger
		Timer1A_1us(2);
		TRIGGER_PIN |= TRIGGER_VALUE; // send high to trigger
		Timer1A_1us(10);
		TRIGGER_PIN &= ~TRIGGER_VALUE; // send low to trigger
		
    while (!done);
  }
}

void PortB_Init(void){ 
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;           										// activate clock for Port b
	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOB)!=SYSCTL_RCGC2_GPIOB){}; 	// wait for clock to start
  
  GPIO_PORTB_PCTL_R 	&= ~0x00FF0000; 																// regular GPIO
  GPIO_PORTB_AMSEL_R 	&= (uint32_t)~SENSOR_VALUE;      								// disable analog function on PB4-5
  GPIO_PORTB_DIR_R 		&= ~ECHO_VALUE;        													// PB4:echo pin, input
  GPIO_PORTB_DIR_R 		|= TRIGGER_VALUE;      													// PB5:trigger pin, output
  GPIO_PORTB_AFSEL_R 	&= ~SENSOR_VALUE;    														// regular port function
  GPIO_PORTB_DEN_R 		|= SENSOR_VALUE;       													// enable digital port
  GPIO_PORTB_IS_R 		&= ~ECHO_VALUE;         												// PB4 is edge-sensitive
  GPIO_PORTB_IBE_R 		|= ECHO_VALUE;         													// PB4 is both edges
  GPIO_PORTB_IEV_R 		&= ~ECHO_VALUE;        													// PB4 both edge event
  GPIO_PORTB_ICR_R 		 = ECHO_VALUE;          												// clear flag 6
  GPIO_PORTB_IM_R 		|= ECHO_VALUE;          												// arm interrupt on PB6
  NVIC_PRI0_R 				 = (NVIC_PRI0_R&0xFFFF1FFF)|0x00002000; 				// priority 1
  NVIC_EN0_R 					 = PORTB_INT;          													// enable Port B edge interrupt
}

void GPIOPortB_Handler(void){
	
	if (ECHO_PIN==ECHO_VALUE){  // echo pin rising edge is detected, start timing
		Timer1A_Start();
	}
	else { // echo pin falling edge is detected, end timing and calculate distance.
    // The following code is based on the fact that the HCSR04 ultrasonic sensor 
    // echo pin will always go low after a trigger with bouncing back
    // or after a timeout. The maximum distance can be detected is 400cm.
		// The speed of sound is approximately 340 meters per second, 
		// or  .0343 c/µS.
    // Distance = (echo pulse width * 0.0343)/2; = ((# of mc)*MC_LEN*SOUND_SPEED)/2
		Timer1A_Stop();
		distance = (uint32_t)(((MY_PRESCALE+1)*(MAX_COUNT_VAL-TIMER1_TAR_R))*MC_LEN*SOUND_SPEED)/2;		
 		done = 1;
	}
	GPIO_PORTB_ICR_R = ECHO_VALUE;      // acknowledge flag 4
}

void Timer1A_Handler(void){
//	distance = OUT_OF_RANGE;
//	done = 1;
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;	// acknowledge TIMER1A timeout
}
