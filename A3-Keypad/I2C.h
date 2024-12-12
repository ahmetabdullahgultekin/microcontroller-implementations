#ifndef __STM32L476G_DISCOVERY_I2C_H
#define __STM32L476G_DISCOVERY_I2C_H

#include <stdint.h>
#include <stddef.h>
#include "stm32l476xx.h"

// PB 8 <--> AF4 (I2C1_SCL)
// PB 9 <--> AF4 (I2C1_SDA)
#define I2C_SCL_PIN  8
#define I2C_SDA_PIN  9
#define I2C_PORT     GPIOB

#define READ_FROM_SLAVE 1
#define WRITE_TO_SLAVE  0

void I2C_GPIO_init(void);
int8_t I2C_Start(I2C_TypeDef * I2Cx, uint32_t DevAddress, uint8_t Size, uint8_t Direction);
void I2C_Stop(I2C_TypeDef * I2Cx);
void I2C_WaitLineIdle(I2C_TypeDef * I2Cx);
int8_t I2C_SendData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size);
int8_t I2C_ReceiveData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size);
void I2C_Initialization(I2C_TypeDef * I2Cx);
void CODEC_Initialization(void);


#endif /* __STM32L476G_DISCOVERY_I2C_H */
