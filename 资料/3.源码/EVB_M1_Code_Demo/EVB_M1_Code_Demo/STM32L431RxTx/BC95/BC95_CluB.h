#ifndef __BC95_CLUB__
#define __BC95_CLUB__
#include "stm32l4xx_hal.h"
#include "main.h"
#include "usart.h"



#define BC95_UART (&huart2)
#define BUF_MAX_LENTH  100

static uint8_t BC95C_Query_Point_Func(const uint8_t *SendData, uint8_t SendLenth, const uint8_t* AT_Expect, uint8_t  AT_Expect_len,	uint8_t Retu_Max_lenth,	uint32_t wait_time);

static uint8_t BC95C_Custom_CSQ_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time);
static uint8_t BC95C_Custom_IP_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time);
static uint8_t BC95C_Custom_IMEI_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time);
static uint8_t BC95C_Custom_CCLK_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time);
static uint8_t BC95C_Custom_CSCON_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time);
static uint8_t BC95C_Custom_CEREG_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time);
static uint8_t BC95C_Custom_CGATT_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time);
static uint8_t BC95C_Custom_CFUN_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time);
static uint8_t BC95C_Custom_NSONMI_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time);
static uint8_t BC95C_Custom_NSORF_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time);



static uint8_t BC95_Query_Param_OK(const uint8_t* SendData, uint8_t SendLen, uint8_t MaxRetLen,
				 		  												const uint8_t *Param, uint8_t Param_lenth, 
					  													uint8_t *Result_Data, uint32_t waittime);

																			
																			
typedef uint8_t(*QueryFunction)(const uint8_t* , uint8_t, const uint8_t*, uint8_t, uint8_t, uint32_t);
typedef uint8_t(*CustomFunction)(const uint8_t*, uint8_t, const uint8_t*, uint8_t, char*, uint8_t, uint32_t);

typedef struct{
	const uint8_t* AT;
	int len;
	const uint8_t* AT_Expect;
	int len_expect;
	int retu_max_len;
	int wait_time;
	QueryFunction Call_Function;
}AT_Com_Type_Query;


typedef struct{
	const uint8_t* AT;
	int len;
	const uint8_t* AT_Expect;
	int len_expect;
	char *Result;
	int retu_max_len;
	int wait_time;
	CustomFunction Call_Function;
}AT_Com_Type_Custom;


#define DEF_COM_QUERY(TRANSMIT, EXPECT_RECV, RETU_MAX_LENTH, WAIT_TIME, POINT_FUNC)\
						{TRANSMIT, sizeof(TRANSMIT), EXPECT_RECV, sizeof(EXPECT_RECV), RETU_MAX_LENTH, WAIT_TIME, POINT_FUNC}
#define DEF_COM_Custom(TRANSMIT, EXPECT_RECV, RESULT,RETU_MAX_LENTH, WAIT_TIME, POINT_FUNC)\
						{TRANSMIT, sizeof(TRANSMIT), EXPECT_RECV, sizeof(EXPECT_RECV)-1,RESULT, RETU_MAX_LENTH, WAIT_TIME, POINT_FUNC}						
#define CLUB_AT_QUERY(XXXX)	XXXX.Call_Function(XXXX.AT, XXXX.len, XXXX.AT_Expect, XXXX.len_expect, XXXX.retu_max_len, XXXX.wait_time)
#define CLUB_AT_Custom(XXXX) XXXX.Call_Function(XXXX.AT, XXXX.len, XXXX.AT_Expect, XXXX.len_expect,XXXX.Result, XXXX.retu_max_len, XXXX.wait_time)

extern const AT_Com_Type_Query AT_COM_Test;
extern const AT_Com_Type_Query AT_COM_BAND;
extern const AT_Com_Type_Query AT_COM_NRB;
extern const AT_Com_Type_Query AT_COM_NSOCL;
extern const AT_Com_Type_Query AT_COM_NSOST;
extern const AT_Com_Type_Query AT_COM_NSOCR;
extern const AT_Com_Type_Query AT_COM_NCDP;
extern const AT_Com_Type_Query AT_COM_NMGS;
						
extern const AT_Com_Type_Custom AT_COM_Custom_CSQ;
extern const AT_Com_Type_Custom AT_COM_Custom_IP;
extern const AT_Com_Type_Custom AT_COM_Custom_IMEI;
extern const AT_Com_Type_Custom AT_COM_Custom_TIME;
extern const AT_Com_Type_Custom AT_COM_Custom_CSCON;
extern const AT_Com_Type_Custom AT_COM_Custom_CEREG;
extern const AT_Com_Type_Custom AT_COM_Custom_CGATT;
extern const AT_Com_Type_Custom AT_COM_Custom_CFUN;
extern const AT_Com_Type_Custom AT_COM_Custom_NSONMI;
extern const AT_Com_Type_Custom AT_COM_Custom_NSORF;

#endif

