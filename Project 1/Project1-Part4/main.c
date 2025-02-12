// UARTTestMain.c
// Runs on LM4F120/TM4C123
// Used to test the UART.c driver
// Daniel Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

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
// Modified by Dr. Min He on 9/23/2023

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#include "PLL.h"
#include "UART0.h"
#include <stdint.h>
#include <stdio.h>

#define MAX_STR_LEN 50

// Subroutine to wait 0.5 sec
// Inputs: None
// Outputs: None
// Notes: ...
void Delay(void);

int main(void){
  uint8_t i;
  uint8_t str[MAX_STR_LEN];  

  PLL_Init();
  UART0_Init();              // initialize UART
  UART0_OutCRLF();
  for(i='A'; i<='Z'; i=i+1){// print the uppercase alphabet
    UART0_OutChar(i);
  }
  UART0_OutCRLF();
  UART0_OutChar(' ');
  for(i='a'; i<='z'; i=i+1){// print the lowercase alphabet
    UART0_OutChar(i);
  }
  UART0_OutCRLF();
  UART0_OutChar('-');
  UART0_OutChar('-');
  UART0_OutChar('>');
  UART0_OutCRLF();
	i=0;
  while(1){
		sprintf((char *)str, "Current number is: %d\n\r", i++);
    UART0_OutString(str); 
		Delay();
  }
}

// Subroutine to wait 0.5 sec
// Inputs: None
// Outputs: None
// Notes: ...
void Delay(void){unsigned long volatile time;
  time = 727240*100/91;  // 0.5 sec
  while(time){
		time--;
  }
	for (time=0;time<1000;time=time+10) {
	}
}
