#ifndef _H_SHT20Drv
#define _H_SHT20Drv

#include "main.h"
#include "stm32l4xx_hal.h"


#define SHT20ReadDelay_10ms  50   

#define SHT20_SCL(x)	(x? HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET): \
                         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET))


#define SHT20_SDA(x)	(x? HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET): \
                         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET))


#define SHT20_SDA_Read    HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)
#define SHT20_SCL_Read    HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9)

typedef unsigned short u16;
typedef unsigned char  u8;



//---------- Defines -----------------------------------------------------------


// sensor command
typedef enum{
  TRIG_T_MEASUREMENT_HM    = 0xE3, // command trig. temp meas. hold master
  TRIG_RH_MEASUREMENT_HM   = 0xE5, // command trig. humidity meas. hold master
  TRIG_T_MEASUREMENT_POLL  = 0xF3, // command trig. temp meas. no hold master
  TRIG_RH_MEASUREMENT_POLL = 0xF5, // command trig. humidity meas. no hold master
  USER_REG_W               = 0xE6, // command writing user register
  USER_REG_R               = 0xE7, // command reading user register
  SOFT_RESET               = 0xFE  // command soft reset
}etSHT2xCommand;

typedef enum {
  SHT2x_RES_12_14BIT       = 0x00, // RH=12bit, T=14bit
  SHT2x_RES_8_12BIT        = 0x01, // RH= 8bit, T=12bit
  SHT2x_RES_10_13BIT       = 0x80, // RH=10bit, T=13bit
  SHT2x_RES_11_11BIT       = 0x81, // RH=11bit, T=11bit
  SHT2x_RES_MASK           = 0x81  // Mask for res. bits (7,0) in user reg.
} etSHT2xResolution;

typedef enum {
  SHT2x_EOB_ON             = 0x40, // end of battery
  SHT2x_EOB_MASK           = 0x40, // Mask for EOB bit(6) in user reg.
} etSHT2xEob;

typedef enum {
  SHT2x_HEATER_ON          = 0x04, // heater on
  SHT2x_HEATER_OFF         = 0x00, // heater off
  SHT2x_HEATER_MASK        = 0x04, // Mask for Heater bit(2) in user reg.
} etSHT2xHeater;

// measurement signal selection
typedef enum{
  HUMIDITY,
  TEMP
}etSHT2xMeasureType;

typedef enum{
  I2C_ADR_W                = 128,   // sensor I2C address + write bit
  I2C_ADR_R                = 129    // sensor I2C address + read bit
}etI2cHeader;

typedef enum{
  ACK                      = 0,
  NO_ACK                   = 1,
}etI2cAck;

typedef enum{
  ACK_ERROR                = 0x01,
  TIME_OUT_ERROR           = 0x02,
  CHECKSUM_ERROR           = 0x04,
  UNIT_ERROR               = 0x08
}etError;

typedef struct{
  float HUMI;
  float TEMP;
}SHT20MeasureDef;


void SHT20Init(void);
void SHT20Process(void);

extern u16 CNT_10ms_SHT20;
extern u16 POLYNOMIAL;
extern u8 SHT20Regiter;
extern u16 test;
extern u16 CNT_10ms_SHT20;
extern u16 Fg_Item_SHT20;
extern u16 Fg_SHT20Reset;
extern SHT20MeasureDef SHT20Measure;




#endif


