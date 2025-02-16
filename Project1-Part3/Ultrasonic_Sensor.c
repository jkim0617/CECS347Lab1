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
#include "UART.h"

#include <stdint.h> // C99 data types
#include <stdio.h>





#define SW1             	(*((volatile uint32_t *)0x40025040))  // bit address for SW1 
#define LED             	(*((volatile uint32_t *)0x40025038))  // bit address for All LED
	
#define PF1       (*((volatile unsigned long *)0x40025008))			// bit address for Red LED
#define PF2       (*((volatile unsigned long *)0x40025010))			// bit address for blue LED
#define PF3       (*((volatile unsigned long *)0x40025020))		  // bit address for green LED



#define PORTF_MASK        0x1E
#define RGB								0x0E
#define SW1_MASK          0x10

#define RED								0x02
#define BLUE						  0x04
#define GREEN							0x08


//#define TRIGGER_PIN 		(*((volatile unsigned long *)0x40005080))  // PB5 is the trigger pin	
//#define TRIGGER_VALUE 	0x20   // trigger at bit 5
//#define ECHO_PIN 				(*((volatile unsigned long *)0x40005040))  // PB4 is the echo pin	
//#define ECHO_VALUE 			0x10   // echo at bit 4
//#define SENSOR_VALUE		0x30	

#define TRIGGER_PIN 		(*((volatile unsigned long *)0x40005200))  // PB7 is the trigger pin	
#define TRIGGER_VALUE 	0x80   // trigger at bit 7
#define ECHO_PIN 				(*((volatile unsigned long *)0x40005100))  // PB6 is the echo pin	
#define ECHO_VALUE 			0x40   // trigger at bit 6
#define SENSOR_VALUE		0xC0	


#define MC_LEN 					0.0625 // length of one machine cycle in microsecond for 16MHz clock
#define SOUND_SPEED 		0.0343 // centimeter per micro-second
#define PORTB_INT				0x00000002


#define MAX_STR_LEN 50

extern void EnableInterrupts(void);
extern void DisableInterrupts(void);
extern void GPIOPortB_Handler(void);
extern void GPIOPortF_Handler(void);
void PortF_Init(void);
void PortB_Init(void);
void Timer1A_Handler(void);
void Delay(void);

static volatile uint8_t done=0;
static volatile uint32_t distance=0;
int main(void){
	DisableInterrupts();
	Timer1A_Init(MAX_COUNT_VAL);
	PortB_Init();
	PortF_Init();
  UART0_Init();	
	PLL_Init();
  EnableInterrupts();
  while(1){
//		TRIGGER_PIN &= ~TRIGGER_VALUE; // send low to trigger
//		Timer1A_1us(2);
//		TRIGGER_PIN |= TRIGGER_VALUE; // send high to trigger
//		Timer1A_1us(10);
//		TRIGGER_PIN &= ~TRIGGER_VALUE; // send low to trigger
//		
//    while (!done);
//		done = 0;
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
  GPIO_PORTB_ICR_R 		 = ECHO_VALUE;          												// clear flag 6
  GPIO_PORTB_IM_R 		|= ECHO_VALUE;          												// arm interrupt on PB6
  NVIC_PRI0_R 				 = (NVIC_PRI0_R&0xFFFF1FFF)|0x00002000; 				// priority 1
  NVIC_EN0_R 					 = PORTB_INT;          													// enable Port B edge interrupt
}


//PortF_Init initializes PF1-3 as output LED while PF4 is set as an input switch
void PortF_Init(void) {
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;   // activate F clock
	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOF)!=SYSCTL_RCGC2_GPIOF){}   //Wait for the clock to be ready
	GPIO_PORTF_LOCK_R   = 0x4C4F434B;				//unlock port F
	GPIO_PORTF_CR_R     = PORTF_MASK;				//Allow changes to PF1-4
	GPIO_PORTF_AMSEL_R &= 0x00;     		    //Disable analog functionality on PF
	GPIO_PORTF_PCTL_R   = 0x000FFFF0; 			//PCTL GPIO on PF1-4
  GPIO_PORTF_DIR_R   |= RGB;   						//Make PF2 output (PF2 built-in LED)
	GPIO_PORTF_DIR_R   &= ~SW1_MASK;   			//Make PF4 input (PF4 built-in Switch)
	GPIO_PORTF_AFSEL_R  = 0x00;       			//Disable Alt function on PF0-7
	GPIO_PORTF_PUR_R    = SW1_MASK;					//Enable pull up resistor on PF4
	GPIO_PORTF_DEN_R    = PORTF_MASK;				//Enable Digital IO on PF1-4
	GPIO_PORTF_IS_R    &= ~0x10;						//PF4 is edge sensitive
	GPIO_PORTF_IBE_R   &= ~0x10;						//PF4 is not both edges 
	GPIO_PORTF_IEV_R   &= ~0x10;						//PF4 is edge sensitive
	GPIO_PORTF_ICR_R   |= 0x10;							//clear flag 4
	GPIO_PORTF_IM_R |= 0x10;								//arm interrupt on PF4
	NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF1FFFFF) | 0x00A00000;  //Priority 5
	NVIC_EN0_R |= 0x40000000;								//enable interrupt 30 in NVIC
}


void GPIOPortF_Handler(void){
	TRIGGER_PIN &= ~TRIGGER_VALUE; // send low to trigger
	Timer1A_1us(2);
	TRIGGER_PIN |= TRIGGER_VALUE; // send high to trigger
	Timer1A_1us(10);
	TRIGGER_PIN &= ~TRIGGER_VALUE; // send low to trigger
		
  while (!done);
	done = 0;
	
	
	
	int done1 = 1;
	TIMER1_CTL_R 	 &= ~TIMER_CTL_TAEN;									// Disable TIMER1A during setup
	GPIO_PORTF_ICR_R = 0x10;		//acknowlege flag 4, sw1 pressed
	uint8_t i;
  uint8_t str[MAX_STR_LEN];
	//uint8_t delay = 500000;
	//When the switch is pressed the system will read the distance valie and display it on the PC terminal
	if(distance > 100){
		LED = 0;  //turn LED off
		sprintf((char *)str, "The current distance is OUT OF RANGE\n\r");
    UART0_OutString(str); 	
	}
	//If the distance is "70cm < distance < 100cm" then turn PF2 on, all others off
	if(distance > 70 && distance < 100){
		if(LED != GREEN){
			LED ^= BLUE;
		}
		sprintf((char *)str, "The current distance is: %d cm\n\r", distance);
    UART0_OutString(str); 		
	}
	//If the distance is "10cm < distance < 70cm" then turn PF3 on, all others off
	if(distance > 10 && distance < 70){
		if(LED != GREEN){
			LED ^= GREEN;
		}
    sprintf((char *)str, "The current distance is: %d cm\n\r", distance);
    UART0_OutString(str); 		
	}
	
  //If the distance is "10cm > distance" then blink 
	if(distance < 10){
		LED = 0;
    sprintf((char *)str, "The current distance is: %d cm\n\r", distance);
    UART0_OutString(str);
    done1 = 0;		
		while(done1 != 100){
			SysTick_Wait(40000);
			SysTick_Wait(40000);

			//Timer1A_1us(2500000);  //wait 25ms
			LED ^= RED;               //turn on red LED
			SysTick_Wait(40000);
			SysTick_Wait(40000);
			done1++;
      //Timer1A_1us(2500000);  //wait 25ms
		}
	}
	TIMER1_CTL_R 	 |= TIMER_CTL_TAEN;									// Enable TIMER1A after interrupt finishes

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
		distance = (getTime()*(MY_PRESCALE+1)*MC_LEN*SOUND_SPEED)/2;		
 		done = 1;
	}
	GPIO_PORTB_ICR_R = ECHO_VALUE;      // acknowledge flag 4
}
void Timer1A_Handler(void){
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;	// acknowledge TIMER1A timeout
	TIMER1_CTL_R	 &= ~TIMER_CTL_TAEN;    								// disable TIMER1A
	TIMER1_IMR_R 	 &= ~TIMER_IMR_TATOIM;   							// disable timeout interrupt	
	distance = OUT_OF_RANGE;
	done = 1;
}
