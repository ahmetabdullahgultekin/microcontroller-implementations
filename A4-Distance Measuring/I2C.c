#include "I2C.h"

extern void Error_Handler(void);

// Inter-integrated Circuit Interface (I2C)
// up to 100 Kbit/s in the standard mode, 
// up to 400 Kbit/s in the fast mode, and 
// up to 3.4 Mbit/s in the high-speed mode.

// Recommended external pull-up resistance is 
// 4.7 kOmh for low speed, 
// 3.0 kOmh for the standard mode, and 
// 1.0 kOmh for the fast mode
	
//===============================================================================
//                          I2C Initialization
//===============================================================================
// Configure I2C1_SCL (PB8) Pin as : Alternate function, High Speed, Open drain, Pull up 
// Configure I2C1_SDA (PB9) Pin as : Alternate function, High Speed, Open drain, Pull up
// PB 8 <--> AF4 (I2C1_SCL)
// PB 9 <--> AF4 (I2C1_SDA)

void I2C_GPIO_init(void){
	
	RCC->AHB2ENR  |=  RCC_AHB2ENR_GPIOBEN;
	
	// GPIO Mode: Input(00, reset), Output(01), AlterFunc(10), Analog(11, reset)
	I2C_PORT->MODER   &= ~( 3U<<(2*I2C_SCL_PIN) | 3U<<(2*I2C_SDA_PIN) );  // Clear Mode	
	I2C_PORT->MODER   |=    2U<<(2*I2C_SCL_PIN) | 2U<<(2*I2C_SDA_PIN);    // Alternative Function
	
	I2C_PORT->AFR[1]  &= ~0x000000FF;   // Alternative Function 4 = I2C1
	I2C_PORT->AFR[1]  |=  0x00000044;   // Alternative Function 4 = I2C1
	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	I2C_PORT->OSPEEDR &= ~(3U<<(2*I2C_SCL_PIN) | 3U<<(2*I2C_SDA_PIN));   // Clear bits
	I2C_PORT->OSPEEDR |=   1U<<(2*I2C_SCL_PIN) | 1U<<(2*I2C_SDA_PIN);    // Medium Speed
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	I2C_PORT->PUPDR   &= ~( 3U<<(2*I2C_SCL_PIN) | 3U<<(2*I2C_SDA_PIN) );
	I2C_PORT->PUPDR   |=    1U<<(2*I2C_SCL_PIN) | 1U<<(2*I2C_SDA_PIN) ;   // Pull-up
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
	I2C_PORT->OTYPER  |= 1U<<I2C_SCL_PIN | 1U<<7;  // Open Drain 
}




void I2C_Initialization(I2C_TypeDef * I2Cx){

	uint32_t OwnAddr = 0x52;
	
//	NVIC_EnableIRQ(I2C1_EV_IRQn);       // Enable I2C1 Event Interrupt
//	NVIC_SetPriority(I2C1_EV_IRQn, 0);  
//	
//	NVIC_EnableIRQ(I2C1_ER_IRQn);       // Enable I2C1 Error Interrupt
//	NVIC_SetPriority(I2C1_ER_IRQn, 0);
	
	// Enable the clock of I2C
	if (I2Cx == I2C1) {
		RCC->APB1ENR1	|= RCC_APB1ENR1_I2C1EN;		  // I2C2 clock enable
		RCC->CCIPR &= ~RCC_CCIPR_I2C1SEL;         // 00 = PCLK, 01 = SYSCLK, 10 = HSI16, 11 = Reserved
		RCC->CCIPR |=  RCC_CCIPR_I2C1SEL_0;       // Set I2C1 clock source as SYSCLK
		RCC->APB1RSTR1 |=  RCC_APB1RSTR1_I2C1RST;
		RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C1RST;				
	} else if (I2Cx == I2C2) {
		RCC->APB1ENR1	|= RCC_APB1ENR1_I2C2EN;	    // I2C3 clock enable
		RCC->CCIPR &= ~RCC_CCIPR_I2C2SEL;         // 00 = PCLK, 01 = SYSCLK, 10 = HSI16, 11 = Reserved
		RCC->CCIPR |=  RCC_CCIPR_I2C2SEL_0;       // Set I2C2 clock source as SYSCLK
		RCC->APB1RSTR1 |=  RCC_APB1RSTR1_I2C2RST;
		RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C2RST; 
	} else if (I2Cx == I2C3) {
		RCC->APB1ENR1	|= RCC_APB1ENR1_I2C3EN;    	// I2C3 clock enable
		RCC->CCIPR &= ~RCC_CCIPR_I2C3SEL;         // 00 = PCLK, 01 = SYSCLK, 10 = HSI16, 11 = Reserved
		RCC->CCIPR |=  RCC_CCIPR_I2C3SEL_0;       // Set I2C3 clock source as SYSCLK
		RCC->APB1RSTR1 |=  RCC_APB1RSTR1_I2C3RST;
		RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C3RST;
	}
	
  /*---------------------------- I2Cx CR1 Configuration ----------------------*/		
	// When the I2C is disabled (PE=0), the I2C performs a software reset.
	I2Cx->CR1 &= ~I2C_CR1_PE;       // Diable I2C
	
	// Noise filters
	I2Cx->CR1 &= ~I2C_CR1_ANFOFF;   // 0: Analog noise filter enabled
	I2Cx->CR1 &= ~I2C_CR1_DNF;      // 0000: Digital filter disabled
	
	// I2Cx->CR1 |= I2C_CR1_TXDMAEN;   // DMA transmission requests enable
	// I2Cx->CR1 |= I2C_CR1_RXDMAEN;   // DMA reception requests enable
	// I2Cx->CR1 |= I2C_CR1_TCIE;      // Transfer complete interrupt enable
	// I2Cx->CR1 |= I2C_CR1_STOPIE;    // STOP detection interrupt enable
	// I2Cx->CR1 |= I2C_CR1_TXIE;      // TX interrupt enable 
	// I2Cx->CR1 |= I2C_CR1_RXIE;      // RX interrupt enable 
	// I2Cx->CR1 |= I2C_CR1_ADDRIE;    // Address match interrupt enable
	// I2Cx->CR1 |= I2C_CR1_NACKIE;    // NACK received interrupt enable 
	I2Cx->CR1 |= I2C_CR1_ERRIE;        // Errors interrupt enable
		
	// System Management Bus (SMBus)
	// I2Cx->CR1 &= ~I2C_CR1_SMBUS;				 // SMBus Mode: 0 = I2C mode;  1 = SMBus mode
	
	/*---------------------------- I2Cx TIMINGR Configuration ------------------*/
	I2Cx->TIMINGR = 0;	
	// Configure clock prescaler
	// SysTimer = 80 MHz, PRESC = 7,  80MHz/(1 + 7) = 10 MHz
	I2Cx->TIMINGR &= ~I2C_TIMINGR_PRESC; // Clear the prescaler 
	I2Cx->TIMINGR |= 7U << 28;           // Set clock prescaler to 7
	// tSCL = tSYNC1 + tSYNC2 + {[(SCLH+1) + (SCLL+1)] x (PRESC+1) x tI2CCLK}
	// SCL Frequency = 8 MHz / 100 = 80 kHz
	// Configure SCL high, low period
	// For SSD1306 LCD: t_cycle > 2.5us
	I2Cx->TIMINGR |= 14U;       // SCLL: SCL low period (master mode) > 1.4 us
	I2Cx->TIMINGR |= 14U << 8;  // SCLH: SCL high period (master mode) > 1.4 us	 
	// Configure SDA setup, and hold time
	I2Cx->TIMINGR |= 1U << 20;  // SCLDEL: Data setup time > 0.1 us
	I2Cx->TIMINGR |= 2U << 16;  // SDADEL: Data hold time  > 1.25 us	

	/*---------------------------- Own address 1 register (I2C_OAR1) -----------*/
	// I2C Own address1 and ack own address1 mode
	// Before STM32 sends its start sequence, it listens to the I2C lines waiting for its address
	// This is helpful if STM32 is used as slave
	I2Cx->OAR1 &= ~I2C_OAR1_OA1EN;
	I2Cx->OAR1 = I2C_OAR1_OA1EN | OwnAddr; // 7-bit own address
	// 10-bit address mode
	// I2Cx->OAR1 = I2C_OAR1_OA1EN | I2C_OAR1_OA1MODE | OwnAddr; 
	
	// Enable clock stretching
	I2Cx->CR1 &= ~I2C_CR1_NOSTRETCH;
	
	/*---------------------------- I2Cx OAR2 Configuration ---------------------*/
	// Disable own address 2
	I2Cx->OAR1 &= ~I2C_OAR2_OA2EN;
	
	/*---------------------------- I2Cx CR2 Configuration ----------------------*/
	I2Cx->CR2 &= ~I2C_CR2_ADD10;  // 0 = 7-bit mode, 1 = 10-bit mode
	
	// Enable the AUTOEND by default, and enable NACK (should be disable only during Slave process 
	// 1: Automatic end mode: a STOP condition is automatically sent when NBYTES data are transferred.
	// The AUTOEND bit has no effect when the RELOAD bit is set.
	I2Cx->CR2 |= I2C_CR2_AUTOEND;
	
	// For slave mode: set NACK
	// The bit is set by software, cleared by hardware when the NACK is sent, or when a STOP
	// condition or an Address Matched is received, or when PE=0.
	I2Cx->CR2 |= I2C_CR2_NACK;
	
	I2Cx->CR1 |= I2C_CR1_PE;								// Enable I2C1
}

//===============================================================================
//                           I2C Start
// Master generates START condition:
//    -- Slave address: 7 bits
//    -- Automatically generate a STOP condition after all bytes have been transmitted 
// Direction = 0: Master requests a write transfer
// Direction = 1: Master requests a read transfer
//=============================================================================== 

int8_t I2C_Start(I2C_TypeDef * I2Cx, uint32_t DevAddress, uint8_t Size, uint8_t Direction) {	
	
	// Direction = 0: Master requests a write transfer
	// Direction = 1: Master requests a read transfer
	
	uint32_t tmpreg = 0;
	
	// This bit is set by software, and cleared by hardware after the Start followed by the address
	// sequence is sent, by an arbitration loss, by a timeout error detection, or when PE = 0.
	tmpreg = I2Cx->CR2;
	
	tmpreg &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));
	
	if (Direction == READ_FROM_SLAVE)
		tmpreg |= I2C_CR2_RD_WRN;  // Read from Slave
	else
		tmpreg &= ~I2C_CR2_RD_WRN; // Write to Slave
		
  tmpreg |= (uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) | (((uint32_t)Size << 16 ) & I2C_CR2_NBYTES));
	
	tmpreg |= I2C_CR2_START;
	
	// AUTOEND
	// 0: software end mode: TC flag is set when NBYTES data are transferred, stretching SCL low.
	// 1: Automatic end mode: a STOP condition is automatically sent when NBYTES data are transferred.
	// No need to generate the STOP condition if AUTOEND mode is enabled
  // Generate the STOP condition only in case of SOFTEND mode is enabled
	// tmpreg |= I2C_CR2_AUTOEND;
	
	// RELOAD
	// 0: The transfer is completed after the NBYTES data transfer (STOP or RESTART will follow).
	// 1: The transfer is not completed after the NBYTES data transfer (NBYTES will be reloaded).
	// tmpreg |= I2C_CR2_RELOAD;
	
	I2Cx->CR2 = tmpreg; 
	
	// Wait until TXIS flag is set 
//	if (Direction != READ_FROM_SLAVE) {
//		while((I2Cx->ISR & I2C_ISR_TXIS) == 0 && ((I2Cx->ISR & I2C_ISR_NACKF) == 0) ); 
//	}
//	
//	if ( (I2Cx->ISR & I2C_ISR_NACKF) == I2C_ISR_NACKF )
//		return -1;  // Failed
	
   	return 0;  // Success
}

//===============================================================================
//                           I2C Stop
//=============================================================================== 
void I2C_Stop(I2C_TypeDef * I2Cx){
	// Master: Generate STOP bit after the current byte has been transferred 
	I2Cx->CR2 |= I2C_CR2_STOP;								
	// Wait until STOPF flag is reset
	while( (I2Cx->ISR & I2C_ISR_STOPF) == 0 ); 
}

//===============================================================================
//                           Wait for the bus is ready
//=============================================================================== 
void I2C_WaitLineIdle(I2C_TypeDef * I2Cx){
	// Wait until I2C bus is ready
	while( (I2Cx->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY );	// If busy, wait
}

//===============================================================================
//                           I2C Send Data
//=============================================================================== 
int8_t I2C_SendData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if (Size <= 0 || pData == NULL) 
		return -1;
	
	I2C_WaitLineIdle(I2Cx);
	
	if (I2C_Start(I2Cx, DeviceAddress, Size, WRITE_TO_SLAVE) < 0 )
		return -1;

	// Send Data
	// Write the first data in DR register
	// while((I2Cx->ISR & I2C_ISR_TXE) == 0);
	// I2Cx->TXDR = pData[0] & I2C_TXDR_TXDATA;  

	for (i = 0; i < Size; i++) {
		// TXE is set by hardware when the I2C_TXDR register is empty. It is cleared when the next
		// data to be sent is written in the I2C_TXDR register.
		while( (I2Cx->ISR & I2C_ISR_TXE) == 0 ); 

		// TXIS bit is set by hardware when the I2C_TXDR register is empty and the data to be
		// transmitted must be written in the I2C_TXDR register. It is cleared when the next data to be
		// sent is written in the I2C_TXDR register.
		// The TXIS flag is not set when a NACK is received.
		// while((I2Cx->ISR & I2C_ISR_TXIS) == 0 );
		I2Cx->TXDR = pData[i] & I2C_TXDR_TXDATA;  // TXE is cleared by writing to the TXDR register.
	}
	
	// Wait until TC flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0 && (I2Cx->ISR & I2C_ISR_NACKF) == 0);
	
	if ( (I2Cx->ISR & I2C_ISR_NACKF) != 0 )
		return -1;

	I2C_Stop(I2Cx);
	return 0;
}


//===============================================================================
//                           I2C Receive Data
//=============================================================================== 
int8_t I2C_ReceiveData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if (Size <= 0 || pData == NULL) 
		return -1;

	I2C_WaitLineIdle(I2Cx);

	I2C_Start(I2Cx, DeviceAddress, Size, READ_FROM_SLAVE); // 0 = sending data to the slave, 1 = receiving data from the slave
						  	
  for (i = 0; i < Size; i++) {
		// Wait until RXNE flag is set 	
		while( (I2Cx->ISR & I2C_ISR_RXNE) == 0 );
		pData[i] = I2Cx->RXDR & I2C_RXDR_RXDATA;
	}
	
	// Wait until TCR flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0);
	
	I2C_Stop(I2Cx);
	
	return 0;
}


void I2C1_EV_IRQHandler(void){
	// I2C1->ICR |= 
}

void I2C1_ER_IRQHandler(void){
}


