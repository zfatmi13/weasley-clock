/*
 * Copyright (c) 2017, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    Project_Complete.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL43Z4.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
void PORTC_PORTD_IRQHandler(void);
void PORTA_IRQHandler(void);
void move1(int target);
void move2(int target);
void clockwise1(void);
void anticlockwise1(void);
void clockwise2(void);
void anticlockwise2(void);
void delay(int no);
volatile bool check1 = true;
volatile bool check2 = true;
volatile int count1; // 2784
volatile int count2; // 2784
volatile static int current1 = 1;
volatile static int current2 = 1;


/*
 * @brief   Application entry point.
 */
int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    SIM->SCGC5 |= (1 << 9) | (1 << 12) | (1 << 10);



	// ---------- MOTORS ----------------------

	// PWM
	PORTA->PCR[5] = 0x300; // TPM0 CH2
	PORTA->PCR[12] = 0x300; // TPM1 CH0


	SIM->SCGC6 = SIM_SCGC6_TPM0_MASK;
	SIM->SOPT2 = 0x01000000; // 24 and 25 bits set to 01 for IRC48M clock -- 48 MHz (5-1 in ref. man)
	// check the |=
	TPM0->SC = 0; // bits 4 and 3 must be set to zero -- disable to change
	TPM0->CONTROLS[2].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	// MSA and MSB are modes ... select edge PWM or or center PWM
	TPM0->MOD = 7500; // 20ms period
	TPM0->CONTROLS[2].CnV = 562.5; // 1.5/20 duty cycle -- stops the motor
	TPM0->SC = 0b00001111; // enable bit via TPM source clock (3-4 bits) and 64 pre-scaler (0-2 bits)

	SIM->SCGC6 = SIM_SCGC6_TPM1_MASK;
	SIM->SOPT2 = 0x01000000; // 24 and 25 bits set to 01 for IRC48M clock -- 48 MHz (5-1 in ref. man)
	// check the |=
	TPM1->SC = 0; // bits 4 and 3 must be set to zero -- disable to change
	TPM1->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	// MSA and MSB are modes ... select edge PWM or or center PWM
	TPM1->MOD = 7500; // 20ms period
	TPM1->CONTROLS[0].CnV = 562.5; // 1.5/20 duty cycle -- stops the motor
	TPM1->SC = 0b00001111; // enable bit via TPM source clock (3-4 bits) and 64 pre-scaler (0-2 bits)


    // ----------- SENSORS ----------------

    // Sensors are on Ports A and D

	// Port A 2 13
	PORTA->PCR[2] = 0x103;
	PORTA->PCR[13] = 0x103;
	// Port D 2 3 4 6
	PORTD->PCR[6] = 0x103;
	PORTD->PCR[2] = 0x103;
	PORTD->PCR[3] = 0x103;
	PORTD->PCR[4] = 0x103;

	// Outputs
	PTD->PDDR |= 1 << 4;
	PTD->PDDR |= 1 << 6;
	// Inputs
	PTA->PDDR &= ~(1 << 2);
	PTA->PDDR &= ~(1 << 13);
	PTD->PDDR &= ~(1 << 2);
	PTD->PDDR &= ~(1 << 3);

	__disable_irq();

	NVIC_DisableIRQ(PORTC_PORTD_IRQn);
	PORTD->PCR[3] &= ~0xF0000; // clear interrupt selection
	PORTD->PCR[3] |= 0xA0000; // enable falling edge interrupt
	PORTD->PCR[2] &= ~0xF0000; // clear interrupt selection
	PORTD->PCR[2] |= 0xA0000; // enable falling edge interrupt
	//Enable interrupt
	NVIC_SetPriority(PORTC_PORTD_IRQn ,128);
	NVIC_ClearPendingIRQ(PORTC_PORTD_IRQn);
	NVIC_EnableIRQ(PORTC_PORTD_IRQn);

	NVIC_DisableIRQ(PORTA_IRQn);
	PORTA->PCR[2] &= ~0xF0000; // clear interrupt selection
	PORTA->PCR[2] |= 0x90000; // enable rising edge interrupt
	PORTA->PCR[13] &= ~0xF0000; // clear interrupt selection
	PORTA->PCR[13] |= 0x90000; // enable rising edge interrupt
	// Enable Interrupts
	NVIC_SetPriority(PORTA_IRQn ,128);
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTA_IRQn);

	SysTick->CTRL = 0;
	SysTick->VAL = 0;
	SysTick->LOAD = 0xFFFFFF; // reload register with max value

	__enable_irq();



    // ----------- LIGHT ----------------

    // LDR is on PORT B0
	// LEDs are on PORT D7
	// ADC0 for PORT B0
	SIM->SCGC6 |= 1 << 27;

	// GPIO and Pull up
	PORTD->PCR[7] = 0x103;
	// ADC
	PORTB->PCR[0] = 0x0;

	// Input
	PTB->PDDR &= ~(0b1);
	// Output
	PTD->PDDR |= 1 << 7;

	// Setup ADC0 module
	ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00; // configuration 1 register
	// -- software triggering, 12 bits?, long sample time, clock to run ADC at
	ADC0->SC2 &= 0x40; // status and control register -- software triggering

	volatile static int result = 0;


    /* Force the counter to be placed into memory. */
    volatile static int position1 = 5 ;
    volatile static int position2 = 5 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {

		// -------------- LIGHT -------------

		// Read channel 8 of ADC0
		ADC0->SC1[0] = 8;

		// Wait for conversion to complete
		while (!(ADC0->SC1[0] & 0x80));

		result = ADC0->R[0];
		//printf("%d\n", result);
		if (result < 200)
			PTD->PSOR = 1 << 7; // turn on
		else
			PTD->PCOR = 1 << 7; // turn off


    	// -------------- SENSORS -------------

    	// Trigger on Port D 4
		PTD->PSOR |= 1 << 4;
		delay(1);
		PTD->PCOR |= 1 << 4;
		// Echo
		while(check2);
		check2 = true;
		if(count2 < 41760)
			position1 = 1; // printf("	Home\n");
		else if(count2 < 83520)
			position1 = 2; // printf("	School\n");
		else if(count2 < 125280)
			position1 = 3; // printf("	Work\n");
		else if(count2 < 167040)
			position1 = 4; // printf("	Library\n");
		else
			position1 = 5; // printf("	Lost\n");

		delay(110);

		// Trigger on Port D 6
		PTD->PSOR |= 1 << 6;
		delay(1);
		PTD->PCOR |= 1 << 6;
		// Echo
		while(check1);
		check1 = true;
		if(count1 < 83520)
			position2 = 1; // printf("Home\n");
		else if(count1 < 167040)
			position2 = 2; // printf("School\n");
		else if(count1 < 250560)
			position2 = 3; // printf("Work\n");
		else if(count1 < 334080)
			position2 = 4; // printf("Library\n");
		else
			position2 = 5; // printf("Lost\n");

		delay(110);

		// -------------- MOTOR -------------

		move1(position1);
		delay(110);
		move2(position2);
		delay(110);

    }
    return 0 ;
}


void PORTC_PORTD_IRQHandler(void) {
	switch(PORTD->ISFR)
	{
		case(1<<3): // Port D 3
			PORTD->PCR[3]|= PORT_PCR_ISF_MASK; // clear IRQ flag
			// printf("Falling Edge\n");
			count1 = (0xFFFFFF - SysTick->VAL);
			check1 = false;
			SysTick->CTRL = 0;
			SysTick->VAL = 0;
			break;
		case(1<<2): // Port D 3
			PORTD->PCR[2]|= PORT_PCR_ISF_MASK; // clear IRQ flag
			// printf("Falling Edge\n");
			count2 = (0xFFFFFF - SysTick->VAL);
			check2 = false;
			SysTick->CTRL = 0;
			SysTick->VAL = 0;
			break;
		default: // IRQ was something else
			PORTD->ISFR |= (0xFFFFFFFF); // clear all IRQ flags
			break;
	}
}


void PORTA_IRQHandler(void) {
	switch(PORTA->ISFR)
	{
		case(1<<4): // Port A 12
			PORTA->PCR[2]|= PORT_PCR_ISF_MASK; // clear IRQ flag
			// printf("Rising Edge\n");
			// enable the system tick, no interrupts.
			SysTick->CTRL = 0x5;
			break;
		case(1<<13): // PTA13
			PORTA->PCR[13]|= PORT_PCR_ISF_MASK;
			SysTick->CTRL = 0x5;
			break;
		default: // IRQ was something else
			PORTA->ISFR |= (0xFFFFFFFF); // clear all IRQ flags
			break;
	}
}


void move1(int target)
{
	if (target < current1)
	{
		for(int j = target; j < current1; ++j)
		{
			clockwise1();
			delay(8000);
		}
	}
	else if (target > current1)
	{
		for(int j = current1; j < target; ++j)
		{
			anticlockwise1();
			delay(8000);
		}
	}
	current1 = target;
}


void move2(int target)
{
	if (target < current2)
	{
		for(int j = target; j < current2; ++j)
		{
			clockwise2();
			delay(8000);
		}
	}
	else if (target > current2)
	{
		for(int j = current2; j < target; ++j)
		{
			anticlockwise2();
			delay(8000);
		}
	}
	current2 = target;
}


void clockwise1(void)
{
	TPM0->CONTROLS[2].CnV = 530;
	// if you want it to go faster, make this number smaller, but not smaller than around 0x177
	delay(85000);
	TPM0->CONTROLS[2].CnV = 562.5; // disable
}

void anticlockwise1(void)
{
	TPM0->CONTROLS[2].CnV = 576;
	// if you want it to go faster, make this number bigger, but not bigger than around 0x2EE
	delay(85000);
	TPM0->CONTROLS[2].CnV = 562.5; // disable
}

void clockwise2(void)
{
	TPM1->CONTROLS[0].CnV = 530;
	// if you want it to go faster, make this number smaller, but not smaller than around 0x177
	delay(85000);
	TPM1->CONTROLS[0].CnV = 562.5; // disable
}

void anticlockwise2(void)
{
	TPM1->CONTROLS[0].CnV = 576;
	// if you want it to go faster, make this number bigger, but not bigger than around 0x2EE
	delay(85000);
	TPM1->CONTROLS[0].CnV = 562.5; // disable
}


void delay(int no) {
	volatile int i = 0;
	volatile int j = 0;
	for (j = 0; j < no; ++j) {
		for (i = 0; i < 42; ++i) {
			__asm("NOP");
		}
	}
}
