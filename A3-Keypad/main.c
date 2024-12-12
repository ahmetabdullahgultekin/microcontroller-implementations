#include <string.h>
#include <stdio.h>
#include "stm32l476xx.h"
#include "SysClock.h"
#include "I2C.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "keypad.h"

#define NO_KEY_PRESSED 0xFF
#define BUFFER_SIZE 16


// Note: The code gets Warning L6989W
// Report: this is an known issue for STM32L4xx.
// https://www.keil.com/support/docs/3853.htm
// We can ignore this warning because we are not using
// external memory.

uint8_t Data_Receive[6];
uint8_t Data_Send[6];

volatile int32_t TimeDelay;

void I2C_GPIO_init(void);

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

void SysTick_Handler (void) { // SysTick interrupt service routine
	if (TimeDelay > 0) // Prevent it from being negative
	TimeDelay--; // TimeDelay is a global volatile variable
}

void Delay (uint32_t nTime) {
	// nTime: specifies the delay time length
	TimeDelay = nTime; // TimeDelay must be declared as volatile
	while(TimeDelay != 0); // Busy wait
}
	
int main(void){
	volatile int i;
	int count = 0;
	
	int a = 0;
	int n = 0;
	
	char message[64] = "Enter:";
	char chr;
	char str[64];
	
	SysTick_Initialize(16000); // Interrupt period = 16000 cycles
	System_Clock_Init(); // Switch System Clock = 80 MHz
	I2C_GPIO_init();
	I2C_Initialization(I2C1);

	//ssd1306_TestAll();
	ssd1306_Init();
	ssd1306_Fill(Black);
	ssd1306_SetCursor(2,0);
	ssd1306_WriteString(message, Font_16x26, White);
	ssd1306_UpdateScreen();	
	
	/* FOR SINGLE CHAR
		Keypad_Pin_Init();
	while (1) {
        char key = Keypad_Scan(); // Scan for key presses
        if (key != NO_KEY_PRESSED) { // If a key is pressed
            snprintf(str, sizeof(str), "Key: %c", key); // Format the key for display
            ssd1306_Fill(White); // Clear the display
            ssd1306_SetCursor(0, 0); // Reset cursor position
            ssd1306_WriteString(str, Font_11x18, Black); // Display the key
            ssd1306_UpdateScreen(); // Refresh the screen
        }
  }
	*/
	
	// Initialize Pins
	Keypad_Pin_Init();
	
	char key;
  char buffer[BUFFER_SIZE] = {' '}; // Keypress buffer, initialized with spaces
  int buffer_index = 0;
  char last_key = NO_KEY_PRESSED;
	
	while (1) {
        key = Keypad_Scan(); // Scan the keypad

        if (key != last_key) { // Only process new key presses
            last_key = key;

            if (key != NO_KEY_PRESSED) {
                if (key == '*') { // Handle deletion
                    if (buffer_index > 0) {
                        buffer_index--;
                        buffer[buffer_index] = ' ';
                    }
                } else if (buffer_index < BUFFER_SIZE - 1) { // Add key to buffer
                    buffer[buffer_index] = key;
                    buffer_index++;
                }

                // Display the buffer on the OLED
                ssd1306_Fill(Black);
                ssd1306_SetCursor(0, 0);
                ssd1306_WriteString(buffer, Font_11x18, White);
                ssd1306_UpdateScreen();
            }
        }

        // Small delay to prevent noise
        Delay(400);
    }
	
	
	while(1);	 // Deadloop
}

