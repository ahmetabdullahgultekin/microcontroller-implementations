#include "stm32l476xx.h"
#include "SysClock.h"
#include "EXTI.h"
#include "LED.h"

volatile int32_t i;

// Push user blue push button and toggle the green LED
int main(void){
	i = -1;
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	EXTI_Init();
	while(1);
}

