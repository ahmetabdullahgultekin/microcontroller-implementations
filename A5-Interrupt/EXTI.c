#include "EXTI.h"
#include "LED.h"

// Push button = PC 13 
#define EXTI_PIN 13

void EXTI_Init(void){
	// 1. GPIO Configuration (Enable clock)
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; // Enable clock for GPIOC
	
	// 2. GPIO Mode: Input(00, reset), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOC->MODER &= ~(3UL<<(2*EXTI_PIN));  
	//GPIOC->MODER |=   0UL<<(2*EXTI_PIN);      // Output(01)

	// 3. GPIO PUPD: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOC->PUPDR  &= ~(3<<(2*EXTI_PIN));  // No pull-up, no pull-down
	
	// 4. GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)	
	GPIOC->OSPEEDR &= ~(3<<(2*EXTI_PIN));
	GPIOC->OSPEEDR |=   2<<(2*EXTI_PIN);  // Fast speed
	
	// 5. GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
	//GPIOC->OTYPER &= ~(1<<EXTI_PIN);      // Push-pull
	
	// 6. EXTI Interrupt Enable
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI15_10_IRQn, 0);
	
	// 7. Connect External Line to the GPIO
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;
	
	// 8. Interrupt Mask Register (IMR)
	EXTI->IMR1 |= EXTI_IMR1_IM13;
	
	// 9. Falling trigger selection register (FTSR)
	EXTI->FTSR1 |= EXTI_FTSR1_FT13;

}

void EXTI15_10_IRQHandler(void) {  
			//EXTI->PR1 |= EXTI_PR1_PIF13;
	// Check the slides for the interrupt handler!
	if ((EXTI->PR1 & EXTI_PR1_PIF13) != 0) {
		
		GPIOA->ODR ^= 1<<5;
		
		EXTI->PR1 |= EXTI_PR1_PIF13;
	
	}
}
