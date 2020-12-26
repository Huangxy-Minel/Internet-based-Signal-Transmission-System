#ifndef _MYSPI_H
#define _MYSPI_H
#include "sys.h"

#define D0         HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_14)  
#define D1         HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_15)  
#define D2         HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_0)  
#define D3         HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_1) 

#define D4         HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_7)
#define D5         HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_8)
#define D6         HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_9)
#define D7         HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_10)
#define D8         HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_11)
#define D9         HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_12)
#define D10        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_13)
#define D11        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_14)
#define D12        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_15)

#define D13        HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)  
#define D14        HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9)  
#define D15        HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_10)  

#define D16        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_8) 
#define D17        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_9) 
#define D18        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_10) 
#define D19        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_11) 
#define D20        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_12) 
#define D21        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_13) 
#define D22        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_14) 
#define D23        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_15) 

#define D24        HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_0) 
#define D25        HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_1) 
#define D26        HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_2) 
#define D27        HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_3) 
#define D28        HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_6) 
#define D29        HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_7) 
#define D30        HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_9) 
#define D31        HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_10) 

void SPI_Init(void);

#endif




