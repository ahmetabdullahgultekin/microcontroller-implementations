// keypad.c
#include "keypad.h"

#define NO_KEY_PRESSED 0xFF

// Keymap for the 4x4 keypad
const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Initializes GPIOC pins for keypad
void Keypad_Pin_Init(void) {
    // Enable GPIOC clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // Set PC0, PC1, PC2, PC3 as output (Rows)
    GPIOC->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
    GPIOC->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0);

    // Set PC4, PC10, PC11, PC12 as input (Columns)
    GPIOC->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE10 | GPIO_MODER_MODE11 | GPIO_MODER_MODE12);

    // Set no pull-up, no pull-down for inputs
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR4 | GPIO_PUPDR_PUPDR10 | GPIO_PUPDR_PUPDR11 | GPIO_PUPDR_PUPDR12);
}

// Scans the keypad for a pressed key
char Keypad_Scan(void) {
    uint32_t input;
    uint8_t row_pins[4] = {0, 1, 2, 3}; // Row pin numbers
    uint8_t col_pins[4] = {4, 10, 11, 12}; // Column pin numbers

    for (int row = 0; row < 4; row++) {
        // Set all rows high
        GPIOC->ODR |= (1 << row_pins[0]) | (1 << row_pins[1]) | (1 << row_pins[2]) | (1 << row_pins[3]);

        // Pull the current row low
        GPIOC->ODR &= ~(1 << row_pins[row]);

        // Small delay for stabilization
        for (volatile int i = 0; i < 1000; i++);

        // Check columns
        for (int col = 0; col < 4; col++) {
            input = (GPIOC->IDR & (1 << col_pins[col]));
            if (input == 0) {
                // Button is pressed; return the corresponding key
                return keymap[row][col];
            }
        }
    }

    // No key pressed
    return NO_KEY_PRESSED;
}