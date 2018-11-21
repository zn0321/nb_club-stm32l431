#define _C_SHT20Drv
#include "stm32l4xx_hal.h"
#include "SHT20Drv.h"




u16 POLYNOMIAL = 0x131;

u8 SHT20Regiter;
u16 test;
u16 CNT_10ms_SHT20;
u16 Fg_Item_SHT20;
u16 Fg_SHT20Reset;
SHT20MeasureDef SHT20Measure;



//==============================================================================
void I2c_StartCondition ()
//==============================================================================
{
  /*
  SDA=HIGH;
  SCL=HIGH;
  SDA=LOW;
  DelayMicroSeconds(10);  // hold time start condition (t_HD;STA)
  SCL=LOW;
  DelayMicroSeconds(10);
  */
  SHT20_SDA(1);
  SHT20_SCL(1);
  SHT20_SDA(0);
  HAL_Delay(10);
  SHT20_SCL(0);
  HAL_Delay(10);
}
//==============================================================================
void I2c_StopCondition ()
//==============================================================================
{
  /*
  SDA=LOW;
  SCL=LOW;
  SCL=HIGH;
  DelayMicroSeconds(10);  // set-up time stop condition (t_SU;STO)
  SDA=HIGH;
  DelayMicroSeconds(10);
  */
  SHT20_SDA(0);
  SHT20_SCL(0);
  SHT20_SCL(1);
  HAL_Delay(10);
  SHT20_SDA(1);
  HAL_Delay(10);
  
}

//==============================================================================
u8 I2c_WriteByte (u8 txByte)
//==============================================================================
{
  u8 mask,error=0;
  for (mask=0x80; mask>0; mask>>=1)   //shift bit for masking (8 times)
  { if ((mask & txByte) == 0) SHT20_SDA(0);//SDA=LOW;//masking txByte, write bit to SDA-Line
    else SHT20_SDA(1);//SDA=HIGH;
    HAL_Delay(1);             //data set-up time (t_SU;DAT)
    SHT20_SCL(1);//SCL=HIGH;                         //generate clock pulse on SCL
    HAL_Delay(5);             //SCL high time (t_HIGH)
    SHT20_SCL(0);//SCL=LOW;
    HAL_Delay(1);             //data hold time(t_HD;DAT)
  }
  SHT20_SDA(1);//SDA=HIGH;                           //release SDA-line
  SHT20_SCL(1);//SCL=HIGH;                           //clk #9 for ack
  HAL_Delay(1);               //data set-up time (t_SU;DAT)
  //if(SDA_CONF==HIGH) error=ACK_ERROR; //check ack from i2c slave
  if(SHT20_SDA_Read) error= ACK_ERROR;
  SHT20_SCL(0);//SCL=LOW;
  HAL_Delay(20);              //wait time to see byte package on scope
  return error;                       //return error code
}
//==============================================================================
u8 I2c_ReadByte (etI2cAck ack)
//==============================================================================
{
  u8 mask,rxByte=0;
  SHT20_SDA(1);//SDA=HIGH;                           //release SDA-line
  for (mask=0x80; mask>0; mask>>=1)   //shift bit for masking (8 times)
  { SHT20_SCL(1);//SCL=HIGH;                         //start clock on SCL-line
    HAL_Delay(1);             //data set-up time (t_SU;DAT)
    HAL_Delay(3);             //SCL high time (t_HIGH)
    //if (SDA_CONF==1) rxByte=(rxByte | mask); //read bit
    if (SHT20_SDA_Read) rxByte=(rxByte | mask);
    SHT20_SCL(0);//SCL=LOW;
    HAL_Delay(1);             //data hold time(t_HD;DAT)
  }
  SHT20_SDA(0);//SDA=ack;                            //send acknowledge if necessary
  HAL_Delay(1);               //data set-up time (t_SU;DAT)
  SHT20_SCL(1);//SCL=HIGH;                           //clk #9 for ack
  HAL_Delay(5);               //SCL high time (t_HIGH)
  SHT20_SCL(0);//SCL=LOW;
  SHT20_SDA(1);//SDA=HIGH;                           //release SDA-line
  HAL_Delay(20);              //wait time to see byte package on scope
  return rxByte;                      //return error code
}


//==============================================================================
u8 SHT2x_CheckCrc(u8 data[], u8 nbrOfBytes, u8 checksum)
//==============================================================================
{
  u8 crc = 0;	
  u8 byteCtr;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  { crc ^= (data[byteCtr]);
    for (u8 bit = 8; bit > 0; --bit)
    { if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
      else crc = (crc << 1);
    }
  }
  if (crc != checksum) return CHECKSUM_ERROR;
  else return 0;
}


//===========================================================================
u8 SHT2x_ReadUserRegister(u8 *pRegisterValue)
//===========================================================================
{
  u8 checksum;   //variable for checksum byte
  u8 error=0;    //variable for error code

  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR_W);
  error |= I2c_WriteByte (USER_REG_R);
  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR_R);
  *pRegisterValue = I2c_ReadByte(ACK);
  checksum=I2c_ReadByte(NO_ACK);
  error |= SHT2x_CheckCrc (pRegisterValue,1,checksum);
  I2c_StopCondition();
  return error;
}

//===========================================================================
u8 SHT2x_WriteUserRegister(u8 *pRegisterValue)
//===========================================================================
{
  u8 error=0;   //variable for error code

  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR_W);
  error |= I2c_WriteByte (USER_REG_W);
  error |= I2c_WriteByte (*pRegisterValue);
  I2c_StopCondition();
  return error;
}

//===========================================================================
u8 SHT2x_SoftReset()
//===========================================================================
{
  u8  error=0;           //error variable

  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR_W); // I2C Adr
  error |= I2c_WriteByte (SOFT_RESET);                            // Command
  I2c_StopCondition();

  HAL_Delay(15000); // wait till sensor has restarted

  return error;
}

//==============================================================================
float SHT2x_CalcRH(u16 u16sRH)
//==============================================================================
{
  float humidityRH;              // variable for result

  u16sRH &= ~0x0003;          // clear bits [1..0] (status bits)
  //-- calculate relative humidity [%RH] --

  humidityRH = -6.0 + 125.0/65536 * (float)u16sRH; // RH= -6 + 125 * SRH/2^16
  return humidityRH;
}



//==============================================================================
float SHT2x_CalcTemperatureC(u16 u16sT)
//==============================================================================
{
  float temperatureC;            // variable for result

  u16sT &= ~0x0003;           // clear bits [1..0] (status bits)

  //-- calculate temperature [°C] --
  temperatureC= -46.85 + 175.72/65536 *(float)u16sT; //T= -46.85 + 175.72 * ST/2^16
  return temperatureC;
}
//==============================================================================
u8 SHT2x_GetSerialNumber(u8 u8SerialNumber[])
//==============================================================================
{
  u8  error=0;                          //error variable

  //Read from memory location 1
  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR_W);    //I2C address
  error |= I2c_WriteByte (0xFA);         //Command for readout on-chip memory
  error |= I2c_WriteByte (0x0F);         //on-chip memory address
  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR_R);    //I2C address
  u8SerialNumber[5] = I2c_ReadByte(ACK); //Read SNB_3
  I2c_ReadByte(ACK);                     //Read CRC SNB_3 (CRC is not analyzed)
  u8SerialNumber[4] = I2c_ReadByte(ACK); //Read SNB_2
  I2c_ReadByte(ACK);                     //Read CRC SNB_2 (CRC is not analyzed)
  u8SerialNumber[3] = I2c_ReadByte(ACK); //Read SNB_1
  I2c_ReadByte(ACK);                     //Read CRC SNB_1 (CRC is not analyzed)
  u8SerialNumber[2] = I2c_ReadByte(ACK); //Read SNB_0
  I2c_ReadByte(NO_ACK);                  //Read CRC SNB_0 (CRC is not analyzed)
  I2c_StopCondition();

  //Read from memory location 2
  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR_W);    //I2C address
  error |= I2c_WriteByte (0xFC);         //Command for readout on-chip memory
  error |= I2c_WriteByte (0xC9);         //on-chip memory address
  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR_R);    //I2C address
  u8SerialNumber[1] = I2c_ReadByte(ACK); //Read SNC_1
  u8SerialNumber[0] = I2c_ReadByte(ACK); //Read SNC_0
  I2c_ReadByte(ACK);                     //Read CRC SNC0/1 (CRC is not analyzed)
  u8SerialNumber[7] = I2c_ReadByte(ACK); //Read SNA_1
  u8SerialNumber[6] = I2c_ReadByte(ACK); //Read SNA_0
  I2c_ReadByte(NO_ACK);                  //Read CRC SNA0/1 (CRC is not analyzed)
  I2c_StopCondition();

  return error;
}


u8 SetSHT20Itme(etSHT2xMeasureType eSHT2xMeasureType)
{
  u8 error=0;
  I2c_StartCondition();
  error |= I2c_WriteByte (I2C_ADR_W); // I2C Adr
  switch(eSHT2xMeasureType)
  { 
    case HUMIDITY: error |= I2c_WriteByte (TRIG_RH_MEASUREMENT_POLL); break;
    case TEMP    : error |= I2c_WriteByte (TRIG_T_MEASUREMENT_POLL);  break;    
  }
  return error;
}


void SHT20Init(void)
{
  //SHT2x_ReadUserRegister(&SHT20Regiter);  
  //SetSHT20Itme(HUMIDITY);
  CNT_10ms_SHT20 = 0;
  Fg_Item_SHT20 = 0;
  Fg_SHT20Reset = 0;
}
void SHT20Process(void)
{
  u8 data[2],checksum;
  u16 Measure;
  
  if(CNT_10ms_SHT20 >= SHT20ReadDelay_10ms || 1)
  {
    CNT_10ms_SHT20 = 0;
    if(Fg_SHT20Reset == 1)
    {
      SHT20Init();
    }
    else
    {
      I2c_StartCondition();
      if(I2c_WriteByte (I2C_ADR_R) == ACK_ERROR)
      {
        SHT2x_SoftReset();
        Fg_SHT20Reset = 1;
      }
      else
      {
        data[0] = I2c_ReadByte(ACK);
        data[1] = I2c_ReadByte(ACK);
        Measure = data[0];
        Measure<<=8;
        Measure+=data[1];
        
        checksum=I2c_ReadByte(NO_ACK);
        I2c_StopCondition();
        if(SHT2x_CheckCrc (data,2,checksum)==0)
        {
          if(Measure&0x0002)
          {//Êª¶È
            SHT20Measure.HUMI = SHT2x_CalcRH(Measure);
          }
          else
          {//ÎÂ¶È
            SHT20Measure.TEMP = SHT2x_CalcTemperatureC(Measure);
          }
        }        
        Fg_Item_SHT20++;
        if(Fg_Item_SHT20 ==1 )
        {
          SetSHT20Itme(TEMP);
        }
        else
        {
          Fg_Item_SHT20 = 0;
          SetSHT20Itme(HUMIDITY);
        }
        
      }      
    }
  }
}
