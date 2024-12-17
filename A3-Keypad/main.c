#include <string.h>
#include <stdio.h>
#include "stm32l476xx.h"
#include "SysClock.h"
#include "I2C.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "keypad.h"

//variables
#define NO_KEYPRESS 0xFF
#define SIZE_OF_BUFFER 16




uint8_t Data_Receive[6];
uint8_t Data_Send[6];

volatile int32_t TimeDelay;

void I2C_GPIO_init(void);

// we initialize the SystICK Handler
void SysTick_Initialize (uint32_t ticks) {
	SysTick->CTRL = 0; // Firstly we disable SysTick
	SysTick->LOAD = ticks - 1; // Then we set reload register
	
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0; // Here reseting the SysTick counter value
	// Select processor clock: 1 = processor clock; 0 = external clock
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	// Enabling SysTick interrupt, 1 = Enable, 0 = Disable
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	// Enable SysTick
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler (void) { // SysTick interrupt 
	if (TimeDelay > 0) 
	TimeDelay--; // TimeDelay variable is volatile here
}

void Delay (uint32_t nTime) {
	// nTime: specifies the delay time length
	TimeDelay = nTime; // TimeDelay is  volatile
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
	
	/* For a single char
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
	
	// Here we first initialize the Pins
	Keypad_Pin_Init();
	
	char key;
  char buffer[SIZE_OF_BUFFER] = {' '}; // Keypress buffer, initialized with spaces 
  int bufferIndex = 0;
  char last_key = NO_KEYPRESS;
	
	while (1) {
        key = Keypad_Scan(); // Scan the keypad

        if (key != last_key) { // Only process new key presses
            last_key = key;

            if (key != NO_KEYPRESS) { //if any key press before
                if (key == '*') { // Handle the deletion
                    if (bufferIndex > 0) {
                        bufferIndex--; //delete char by char
                        buffer[bufferIndex] = ' ';
                    }
                } else if (bufferIndex < SIZE_OF_BUFFER - 1) { // Add key to buffer
                    buffer[bufferIndex] = key;
                    bufferIndex++;
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

