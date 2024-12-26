#include "stm32l476xx.h"
#include <time.h>

volatile int32_t TimeDelay;

void GPIO_Init(void);
void TIM3_Init(void); // Trigger
void TIM4_Init(void); // Echo
void TIM4_IRQHandler(void);

volatile uint32_t overflow_count = 0;
volatile uint32_t rising_edge = 0;
volatile uint32_t falling_edge = 0;
volatile uint32_t distance = 0;


// User HSI (high-speed internal) as the processor clock
void enable_HSI(){
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 ) {;}
	
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;  //01: HSI16 oscillator used as system clock

  // Wait till HSI is used as system clock source 
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 ) {;}
}


void GPIO_Init(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; // Enable GPIOB clock

    // Configure PB5 (Trigger, TIM3_CH2) as alternate function
    GPIOB->MODER &= ~(3UL << 10);       // Clear mode for pin 5
    GPIOB->MODER |= (2UL << 10);        // Set mode to Alternate Function
    GPIOB->AFR[0] |= (2UL << 20);       // Set AF2 for TIM3_CH2

    // Configure PB6 (Echo, TIM4_CH1) as alternate function
    GPIOB->MODER &= ~(3UL << 12);       // Clear mode for pin 6
    GPIOB->MODER |= (2UL << 12);        // Set mode to Alternate Function
    GPIOB->AFR[0] |= (2UL << 24);       // Set AF2 for TIM4_CH1
}

void TIM3_Init(void) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN; // Enable TIM3 clock

    TIM3->PSC = 15;                     // Prescaler for 1MHz clock
    TIM3->ARR = 0xFFFF;                 // Auto-reload register max value
    TIM3->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2; // PWM Mode 1
    TIM3->CCER |= TIM_CCER_CC2E;        // Enable output for channel 2
    TIM3->CR1 |= TIM_CR1_CEN;           // Enable TIM3 counter
}

void TIM4_Init(void) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN; // Enable TIM4 clock

    TIM4->PSC = 15;                     // Prescaler for 1MHz clock
    TIM4->ARR = 0xFFFF;                 // Auto-reload register max value
    TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;    // Input capture on channel 1
    TIM4->CCER |= TIM_CCER_CC1E;        // Enable capture for channel 1
    TIM4->DIER |= TIM_DIER_CC1IE | TIM_DIER_UIE; // Enable interrupts
    NVIC_SetPriority(TIM4_IRQn, 0);     // Set interrupt priority
    NVIC_EnableIRQ(TIM4_IRQn);          // Enable TIM4 interrupt
    TIM4->CR1 |= TIM_CR1_CEN;           // Enable TIM4 counter
}

void TIM4_IRQHandler(void) {
    if (TIM4->SR & TIM_SR_UIF) {        // Overflow interrupt
        overflow_count++;
        TIM4->SR &= ~TIM_SR_UIF;        // Clear UIF flag
    }
    if (TIM4->SR & TIM_SR_CC1IF) {      // Capture event
        if (TIM4->CCER & TIM_CCER_CC1P) {
            // Falling edge detected
            falling_edge = TIM4->CCR1;
        } else {
            // Rising edge detected
            rising_edge = TIM4->CCR1;
            overflow_count = 0;         // Reset overflow count
        }
        TIM4->CCER ^= TIM_CCER_CC1P;    // Toggle polarity for next edge
        TIM4->SR &= ~TIM_SR_CC1IF;      // Clear CC1IF flag
    }
}


// Input: ticks = number of ticks between two interrupts
void SysTick_Initialize (uint32_t ticks) {
	SysTick->CTRL = 0; // Disable SysTick
	SysTick->LOAD = ticks - 1; // Set reload register
	// Set interrupt priority of SysTick to least urgency (i.e., largest priority value)
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0; // Reset the SysTick counter value
	// Select processor clock: 1 = processor clock; 0 = external clock
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	// Enables SysTick interrupt, 1 = Enable, 0 = Disable
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	// Enable SysTick
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void initialize_controller(){
	enable_HSI();
	SysTick_Initialize(16000); // Interrupt period = 16000 cycles
  GPIO_Init();         // Initialize GPIO pins
  TIM3_Init();         // Initialize TIM3 for trigger
  TIM4_Init();         // Initialize TIM4 for echo capture
}

void SysTick_Handler (void) { // SysTick interrupt service routine
	if (TimeDelay > 0) // Prevent it from being negative
	TimeDelay--; // TimeDelay is a global volatile variable
}

void Delay (uint32_t nTime) {
	// nTime: specifies the delay time length
	TimeDelay = nTime; // TimeDelay must be declared as volatile
	while(TimeDelay != 0); // Busy wait
}

void delay_ms(int milli_seconds){
		int i;
    for(i=0; i<milli_seconds*1000; i++); // Wait milli-seconds
	/* not working (probably because of clock();)
		// Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds) 
		{}
	*/
}

int main(void){
	// Configurations
	initialize_controller();
	// Variables
	uint32_t input;
	
	// Infinite loop
	while(1){
		// Read pin 13
		input = (GPIOC->IDR & GPIO_IDR_IDR_13);
		if (input == 0) {
		// Button is pressed
			//light_sos_message();
		}
	}
}
