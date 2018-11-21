/*!
    \file  bc95.c
    \brief NEUL BC95 IoT model driver 
*/

/*
    Copyright (C) 2016 GigaDevice

    2016-10-19, V1.0.0, demo for GD32F4xx
*/
#include <string.h>
#include <stdio.h>
#include <ctype.h> 
#include "stm32l4xx_hal.h"
#include "bc95.h"
#include "usart.h"

#define NEUL_MAX_BUF_SIZE 1000
#define NEUL_MAX_SOCKETS 1 // the max udp socket links
#define NEUL_IP_LEN 16
#define NEUL_MANUFACT_LEN 40

#define BC95_UART  huart2

static char bc95_buf[NEUL_MAX_BUF_SIZE];
static char bc95_wbuf[NEUL_MAX_BUF_SIZE];
static char bc95_tmpbuf[NEUL_MAX_BUF_SIZE];
static unsigned char readbuf[100];
static unsigned char time[100];
extern msg_for_oled_show	msg_oled_show;
//static int uart_data_read(char *buf, int maxrlen, int mode);
//static int uart_data_write(const char *buf, int writelen, int mode);
int uart_data_read(char *buf, int maxrlen, int mode, int timeout);
int uart_data_write(char *buf, int writelen, int mode);


static neul_dev_operation_t neul_ops = {
    uart_data_read,
    uart_data_write
};
static remote_info udp_socket[NEUL_MAX_SOCKETS] = {
	{0, -1, {0}}
};
static neul_dev_info_t neul_dev = {
    bc95_buf,
    bc95_wbuf,
    0,
    udp_socket,
    &neul_ops,
    {0}
};


/* ============================================================
func name   :   bc95_set_upd_param
discription :   this func just called after the socket created
                set the remote ip address and port the socket sendto or receive fro
param       :   
             rip  @ input param, remote client/server ip address
             port @ input param, remote client/server port
             socket @ input param, local socket handle
return      :  0 mean ok, !0 means param err
Revision    : 
=============================================================== */
int bc95_set_upd_param(const char *rip, const unsigned short port, const int socket)
{
    if (NULL == rip || 0 == port)
    {
        return -1;
    }
    if (strlen(rip) >= NEUL_IP_LEN)
    {
        return -2;
    }
    neul_dev.remotecount++;
    (neul_dev.addrinfo+socket)->port = port;
    (neul_dev.addrinfo+socket)->socket = socket;
    memcpy((neul_dev.addrinfo+socket)->ip, rip, strlen(rip));
    return 0;
}

/* ============================================================
func name   :   bc95_set_upd_param
discription :   this func just called after the socket created
                set the remote ip address and port the socket sendto or receive fro
param       :   
             socket @ input param, local socket handle that need to reset
return      :  0 mean ok, !0 means param err
Revision    : 
=============================================================== */
static int bc95_reset_upd_param(const int socket)
{
    if (socket < 0)
    {
        return -1;
    }
    neul_dev.remotecount--;
    (neul_dev.addrinfo+socket)->port = 0;
    (neul_dev.addrinfo+socket)->socket = -1;
    memset((neul_dev.addrinfo+socket)->ip, 0, NEUL_IP_LEN);
    return 0;
}

/* ============================================================
func name   :   bc95_set_cdp_info
discription :   set cdp server ip address
param       :   
             cdp  @ input param, cdp server's ip address
return      :  0 mean ok, !0 means param err
Revision    : 
=============================================================== */
static int bc95_set_cdp_info(const char *cdp)
{
    if(NULL == cdp)
    {
        return -1;
    }
    if (strlen(cdp) >= NEUL_IP_LEN)
    {
        return -2;
    }
    memcpy(neul_dev.cdpip, cdp, strlen(cdp));
    return 0;
}

/* ============================================================
func name   :  bc95_hw_detect
discription :  detect bc95 hardware 
param       :  None 
return      :  0 bc95 is connectted to mcu ok , !0 means not ok
Revision    : 
=============================================================== */
int bc95_hw_detect(void)   //查看模块型号
{
    int ret = 0;
    char *p = "AT+CGMM\r";
    char *str = NULL;
    ret = neul_dev.ops->dev_write(p, strlen(p), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -1;
    }
    if (NULL == neul_dev.rbuf)
    {
        return -2;
    }
    //bc95_sleep(10);
		
    memset(neul_dev.rbuf, 0, NEUL_MANUFACT_LEN);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, NEUL_MANUFACT_LEN, 0, 500);
		
		if (ret <= 0)
    {
        //read bc95 manufacturer info failed
        return -3;
    }
    str = strstr(neul_dev.rbuf,"BC95HB-02-STD_850");
    if (!str)
    {
        //can't find bc95 manufacturer info
        return -4;
    }
    return 0;
		
}
/* ============================================================
func name   :  bc95_set_band
discription :  set bc95 work band 
param       :  
               band @ input param, bc95 work band BC95-CM set to 8, 
                      BC95-SL set to 5, BC95-VF set to 20
return      :  0 set ok , !0 not ok
Revision    : 
=============================================================== */
int bc95_set_band(int band) //设置波特率
{
    char *p = "AT+NBAND=";
    int ret = 0;
    char *str = NULL;
    memset(neul_dev.wbuf, 0, 16);
    if (band == 8 || band == 5 || band == 20)
    {
        sprintf(neul_dev.wbuf, "%s%d%c", p, band, '\r');
    }
    else
    {
        //error band for bc95
        return -1;
    }
    ret = neul_dev.ops->dev_write(neul_dev.wbuf, strlen(neul_dev.wbuf), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -2;
    }
    memset(neul_dev.rbuf, 0, 16);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 16, 0, 200);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -3;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (NULL == str)
    {
        return -4;
    }
    return 0;
}
/* ============================================================
func name   :  bc95_get_scq
discription :  Get Signal Strength Indicator  
param       :  None 
return      :  scq value  <0 error  
Revision    : 
=============================================================== */
int bc95_get_scq(void)   //信号强度
{
		int scq_i;
    int ret = 0;
	  int scq = 0;
    char *p = "AT+CSQ\r";
    char *scq_str = NULL;
    ret = neul_dev.ops->dev_write(p, strlen(p), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -1;
    }
    if (NULL == neul_dev.rbuf)
    {
        return -2;
    }
    //bc95_sleep(10);
    memset(neul_dev.rbuf, 0, 100);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 100, 0, 200);
    if (ret <= 0)
    {
        //read bc95 manufacturer info failed
        return -3;
    }
    scq_str = strstr(neul_dev.rbuf,"+CSQ:");
    if (!scq_str)
    {
        //can't find bc95 manufacturer info
        return -4;
    }
		scq=(*(scq_str+5)-'0')*10+((*(scq_str+6)-'0'));
		//printf("CSQ:%d\r\n",scq); 
    return scq;
}
/* ============================================================
func name   :  bc95_get_cfunstat
discription :  get bc95's Phone Functionality status 
               
param       :  None
return      :  0 Minimum functionality  , 1 Full  functionality , <1 error
Revision    : 
=============================================================== */
int bc95_get_cfunstat(void) //查询射频开关状态
{
    char *cmd = "AT+CFUN?\r";
    char *str = NULL;
    int ret = 0;
   
    ret = neul_dev.ops->dev_write(cmd, strlen(cmd), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -1;
    }
    memset(neul_dev.rbuf, 0, 50);
		ret = neul_dev.ops->dev_read(neul_dev.rbuf, 50, 0, 500);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -2;
    }
    str = strstr(neul_dev.rbuf,"+CFUN:");
    if (!str)
    {
        return -3;
    }
    
    return (*(str+6)-'0');   
}
/* ============================================================
func name   :  bc95_get_netstat
discription :  get bc95's network status 
               
param       :  None
return      :  0 Detached , 1 Attached , <1 error
Revision    : 
=============================================================== */
int bc95_get_netstat(void) //查询网络激活状态
{
    char *cmd = "AT+CGATT?\r";
    char *str = NULL;
    int ret = 0;
   
    ret = neul_dev.ops->dev_write(cmd, strlen(cmd), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -1;
    }
    memset(neul_dev.rbuf, 0, 50);
		ret = neul_dev.ops->dev_read(neul_dev.rbuf, 50, 0, 500);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -2;
    }
    str = strstr(neul_dev.rbuf,"+CGATT:");
    if (!str)
    {
        return -3;
    }
    
    return (*(str+7)-'0');   //0:Detached , 1:Attached
}
/* ============================================================
func name   :  bc95_get_regstat
discription :  Get EPS Network Registration Status  
param       :  None 
return      :  Status  value  1 means registered on network, 2 means searching the network, <0 error
Revision    : 
=============================================================== */
int bc95_get_regstat(void)   //查询网络注册状态
{
    int ret = 0;
    char *p = "AT+CEREG?\r";
    char *str = NULL;
    ret = neul_dev.ops->dev_write(p, strlen(p), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -1;
    }
    if (NULL == neul_dev.rbuf)
    {
        return -2;
    }
    //bc95_sleep(10);
    memset(neul_dev.rbuf, 0, 100);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 100, 0, 200);
    if (ret <= 0)
    {
        //read bc95 manufacturer info failed
        return -3;
    }
    str = strstr(neul_dev.rbuf,"+CEREG:");
    if (!str)
    {
        //can't find bc95 manufacturer info
        return -4;
    }
    return (*(str+9)-'0');
}
/* ============================================================
func name   :  bc95_get_connetstat
discription :  Get Signalling Connection Status  
param       :  None 
return      :  connetstat  value   1 means Connected  0 means Idle  <0 error
Revision    : 
=============================================================== */
int bc95_get_connetstat(void)   //查询信号连接状态
{
    int ret = 0;
    char *p = "AT+CSCON?\r";
    char *str = NULL;
    ret = neul_dev.ops->dev_write(p, strlen(p), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -1;
    }
    if (NULL == neul_dev.rbuf)
    {
        return -2;
    }
    //bc95_sleep(10);
    memset(neul_dev.rbuf, 0, 100);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 100, 0, 200);
    if (ret <= 0)
    {
        //read bc95 manufacturer info failed
        return -3;
    }
    str = strstr(neul_dev.rbuf,"+CSCON:");
    if (!str)
    {
        //can't find bc95 manufacturer info
        return -4;
    }
    return (*(str+9)-'0');
}
/* ============================================================
func name   :  bc95_set_imei
discription :  set bc95 imei info 
param       :  
               imei @ input param, the imei data set to bc95
               len @ input param, lenght of the data
return      :  0 set ok , !0 not ok
Revision    : 
=============================================================== */
int bc95_set_imei(const char *imei, int len)  //设置模块的IMEI号
{
    int ret = 0;
    char *p = "AT+NTSETID=1,";
    char *p2 = "AT+CGSN=1\r\n";
    char *str = NULL;
    //int cmdlen = 0;
    if (NULL == imei || len <= 0)
    {
        return -1;
    }
    if (NULL == neul_dev.wbuf)
    {
        return -2;
    }
    memset(neul_dev.wbuf, 0, 100);
    //cmdlen = strlen(p);
    //memcpy(neul_dev.wbuf, p, cmdlen);
    //memcpy(neul_dev.wbuf+cmdlen, imei, len);
    sprintf(neul_dev.wbuf, "%s%s%c", p, imei, '\r');
    ret = neul_dev.ops->dev_write(neul_dev.wbuf, strlen(neul_dev.wbuf), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -3;
    }
    memset(neul_dev.rbuf, 0, 16);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 16, 0, 200);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -4;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (NULL == str)
    {
        //read imei info
        ret = neul_dev.ops->dev_write(p2, strlen(p2), 0);
        if (ret < 0)
        {
            //write data to bc95 failed
            return -5;
        }
        memset(neul_dev.rbuf, 0, 64);
        ret = neul_dev.ops->dev_read(neul_dev.rbuf, 16, 0, 200);
        if (ret <= 0)
        {
            //read bc95 read set return value info failed
            return -6;
        }
        str = strstr(neul_dev.rbuf,"OK");
        if (NULL == str)
        {
            return -7;
        }
        str = strstr(neul_dev.rbuf,"+CGSN:");
        if (str && strlen(neul_dev.rbuf) >= 22)
        {
            return 0;
        }
        return -8;
    }
    return 0;
}

/* ============================================================
func name   :  int bc95_check_imei
discription :  check if bc95 imei is setted 
param       :  None
return      :  1 imei already set, <=0 not set
Revision    : 
=============================================================== */
int bc95_check_imei(void)
{
    int ret = 0;
    char *p2 = "AT+CGSN=1\r\n";
    char *str = NULL;
    int retry = 0;
    //int cmdlen = 0;

    if (NULL == neul_dev.wbuf)
    {
        return -1;
    }

		//read imei info
    while(retry < 3)
    {
        retry++;
				ret = neul_dev.ops->dev_write(p2, strlen(p2), 0);
				if (ret < 0)
				{
								//write data to bc95 failed
								return -2;
				}

				ret = neul_dev.ops->dev_read(neul_dev.rbuf, 50, 0, 300);
				if (ret <= 0)
				{
								//read bc95 read set return value info failed
								return -3;
				}
				str = strstr(neul_dev.rbuf,"OK");
				if (NULL == str)
				{
								str = strstr(neul_dev.rbuf,"ERROR");
								if (NULL != str)
								{
										continue;
								}
				}
				str = strstr(neul_dev.rbuf,"+CGSN:");
				if (str && strlen(neul_dev.rbuf) >= 22)
				{
								return 1;
				}
    }
    return 0;
}




/* ============================================================
func name   :  bc95_active_network
discription :  active bc95's network 
               this will take some time
param       :  None
return      :  0 set ok , !0 not ok
Revision    : 
=============================================================== */
int bc95_active_network(void)
{
    int ret = 0;
    char *str = NULL;
    char *cmd0 = "AT+CGSN=1\r";
    char *cmd1 = "AT+CFUN=1\r"; 
    char *cmd2 = "AT+CIMI\r";
    //char *cmd3 = "AT+CGDCONT=1,\"IP\",\"HUAWEI.COM\"\r";
    char *cmd4 = "AT+CGATT=1\r";

    /* Get SN */
    ret = neul_dev.ops->dev_write(cmd0, strlen(cmd0), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -1;
    }
    memset(neul_dev.rbuf, 0, 32);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 32, 0, 2000);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -2;
    }
    str = strstr(neul_dev.rbuf,"+CGSN:");
    if (!str)
    {
        return -3;
    }
    if (strlen(str) < 25)
    {
        return -4;
    }
    
    /* Config CFUN */
    ret = neul_dev.ops->dev_write(cmd1, strlen(cmd1), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -5;
    }
    //bc95_sleep(4000);//need wait 4 sconds or more, it's a little long
    memset(neul_dev.rbuf, 0, 32);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 6, 0, 5000);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -6;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (!str)
    {
        return -7;
    }    
 
    bc95_sleep(1000);
    /* Get IMSI */
    ret = neul_dev.ops->dev_write(cmd2, strlen(cmd2), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -8;
    }
    memset(neul_dev.rbuf, 0, 32);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 32, 0, 200);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -9;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (!str)
    {
        return -10;
    }
    if (strlen(neul_dev.rbuf) < 19)
    {
        return -11;
    }
    
    /* set band to do... */
    ret = bc95_set_band(5);
    if (ret < 0)
    {
        return -12;
    }
    #if 0
    /* Config apn */
    ret = neul_dev.ops->dev_write(cmd3, strlen(cmd3), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -13;
    }
    memset(neul_dev.rbuf, 0, 32);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 32, 0);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -14;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (!str)
    {
        return -15;
    }
    #endif
    
    /* Active Network */
    ret = neul_dev.ops->dev_write(cmd4, strlen(cmd4), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -16;
    }
    memset(neul_dev.rbuf, 0, 32);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 32, 0, 200);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -17;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (!str)
    {
        return -18;
    }

    return 0;
}

/* ============================================================
func name   :  bc95_query_ip
discription :  query if bc95 get ip address 
               
param       :  None
return      :  0 get ip , !0 no ip
Revision    : 
=============================================================== */
int bc95_query_ip(void)
{
    char *cmd = "AT+CGPADDR\r";
    char *str = NULL;
    int ret = 0;
    
    ret = neul_dev.ops->dev_write(cmd, strlen(cmd), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -1;
    }
    memset(neul_dev.rbuf, 0, 64);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 64, 0, 200);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -2;
    }
    //if get +CGPADDR:0 \r\n OK , no ip get ,need reboot
    str = strstr(neul_dev.rbuf,"+CGPADDR:0,");
    if (!str)
    {
        return -3;
    }
    
    return 0;
    
}
/* ============================================================
func name   :  bc95_get_time
discription :  get bc95's the current setting of the clock.
               
param       :  None
return      :  0 ok , !0 error
Revision    : 
=============================================================== */
int bc95_get_time(void)
{
    char *cmd = "AT+CCLK?\r";
    char *time_str = NULL;
    int ret = 0;
   
    ret = neul_dev.ops->dev_write(cmd, strlen(cmd), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -1;
    }
    memset(neul_dev.rbuf, 0, 50);
		ret = neul_dev.ops->dev_read(neul_dev.rbuf, 50, 0, 500);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -2;
    }
    time_str = strstr(neul_dev.rbuf,"+CCLK:");
    if (!time_str)
    {
        return -3;
    }
    sscanf(neul_dev.rbuf, "%*[^:]:%s\r\n",time_str);                             
		printf("time:%s\r\n",time_str);
		msg_oled_show.net_time_for_show = time_str;
    return 0;   
}

/* ============================================================
func name   :  bc95_reboot
discription :  reboot bc95 
               
param       :  None
return      :  0 connected , !0 not connect
Revision    : 
=============================================================== */
int bc95_reboot(void)
{
    char *cmd = "AT+NRB\r";
    char *str = NULL;
    int ret = 0;
    
    ret = neul_dev.ops->dev_write(cmd, strlen(cmd), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -1;
    }
    memset(neul_dev.rbuf, 0, 64);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 64, 0, 11000);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -2;
    }
    
    str = strstr(neul_dev.rbuf,"REBOOTING");
    if (!str)
    {
        return -3;
    }
    
    return 0;
}

/* ============================================================
func name   :  bc95_set_cdpserver
discription :  set bc95 cdp server 
               
param       :  ipaddr @ input param cdp server ip address
return      :  0 set ok , !0 not ok
Revision    : 
=============================================================== */
int bc95_set_cdpserver(const char *ipaddr)
{
    char *cmd = "AT+NCDP=";
    char *cmd2 = "AT+NCDP?";
    char *str = NULL;
    int ret = 0;
    
    if (NULL == ipaddr || strlen(ipaddr) >= 16)
    {
        return -1;
    }
    memset(neul_dev.wbuf, 0, 64);
    sprintf(neul_dev.wbuf, "%s%s%c", cmd, ipaddr, '\r');
    ret = neul_dev.ops->dev_write(neul_dev.wbuf, strlen(neul_dev.wbuf), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -2;
    }
    memset(neul_dev.rbuf, 0, 32);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 32, 0, 200);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -3;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (!str)
    {
        return -4;
    }
    
    //query the setted ip back
    ret = neul_dev.ops->dev_write(cmd2, strlen(cmd2), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -5;
    }
    memset(neul_dev.rbuf, 0, 32);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 32, 0, 200);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -6;
    }
    str = strstr(neul_dev.rbuf,ipaddr);
    if (!str)
    {
        return -7;
    }
    
    return 0;
}
/* ============================================================
func name   :  bc95_hex_to_str
discription :  convert char to hex string 
               
param       :  bufin @ input param , the data that need to convert
               len @ input param, data length
               bufout @ output param, hex string data 
return      :  0 convert ok , !0 not ok
Revision    : 
=============================================================== */
static int bc95_hex_to_str(const unsigned char *bufin, int len, char *bufout)
{
    int i = 0;
    #if 0
    int tmp = 0;
    #endif
    if (NULL == bufin || len <= 0 || NULL == bufout)
    {
        return -1;
    }
    for(i = 0; i < len; i++)
    {
        #if 0
        tmp = bufin[i]>>4;
        bufout[i*2] = tmp > 0x09?tmp+0x37:tmp+0x30;
        tmp = bufin[i]&0x0F;
        bufout[i*2+1] = tmp > 0x09?tmp+0x37:tmp+0x30;
        #else
        sprintf(bufout+i*2, "%02X", bufin[i]);
        #endif
    }
    return 0; 
}

/* ============================================================
func name   :  bc95_str_to_hex
discription :  convert hex string to hex data 
               
param       :  bufin @ input param , the data that need to convert
               len @ input param, data length
               bufout @ output param, hex data after convert 
return      :  0 send ok , !0 not ok
Revision    : 
=============================================================== */
static int bc95_str_to_hex(const char *bufin, int len, char *bufout)
{
    int i = 0;
    unsigned char tmp2 = 0x0;
    unsigned int tmp = 0;
    if (NULL == bufin || len <= 0 || NULL == bufout)
    {
        return -1;
    }
    for(i = 0; i < len; i = i+2)
    {
        #if 1
        tmp2 =  bufin[i];
        tmp2 =  tmp2 <= '9'?tmp2-0x30:tmp2-0x37;
        tmp =  bufin[i+1];
        tmp =  tmp <= '9'?tmp-0x30:tmp-0x37;
        bufout[i/2] =(tmp2<<4)|(tmp&0x0F);
        #else
        sscanf(bufin+i, "%02x", &tmp);
        bufout[i/2] = tmp;
        #endif
    }
    return 0; 
}
 int bc95_hexstr_to_byte(const char* source, char* dest, int sourceLen)  
{  
    short i;  
    unsigned char highByte, lowByte;  
      
    for (i = 0; i < sourceLen; i += 2)  
    {  
        highByte = toupper(source[i]);  
        lowByte  = toupper(source[i + 1]);  
  
        if (highByte > 0x39)  
            highByte -= 0x37;  
        else  
            highByte -= 0x30;  
  
        if (lowByte > 0x39)  
            lowByte -= 0x37;  
        else  
            lowByte -= 0x30;  
  
        dest[i / 2] = (highByte << 4) | lowByte;  
    }  
    return 0 ;  
} 

/* ============================================================
func name   :  bc95_send_coap_paylaod
discription :  send coap message(hex data) to cdp server 
               
param       :  buf @ input param , the data that send to cdp
               sendlen @ input param, data length
return      :  0 send ok , !0 not ok
Revision    : 
=============================================================== */
int bc95_send_coap_paylaod(const char *buf, int sendlen)
	{
    char *cmd = "AT+NMGS=";
    char *cmd2 = "AT+NQMGS\r";
    int ret = 0;
    char *str = NULL;
    static int sndcnt = 0;
    int curcnt = 0;
    
    if (NULL == buf || sendlen >= 512)
    {
        return -1;
    }
    memset(neul_dev.wbuf, 0, 64);
    memset(bc95_tmpbuf, 0, NEUL_MAX_BUF_SIZE);
    ret = bc95_hex_to_str((unsigned char *)buf, sendlen, bc95_tmpbuf);
    sprintf(neul_dev.wbuf, "%s%d,%s%c", cmd, sendlen, bc95_tmpbuf, '\r');
    ret = neul_dev.ops->dev_write(neul_dev.wbuf, strlen(neul_dev.wbuf), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -2;
    }
    memset(neul_dev.rbuf, 0, 32);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 32, 0, 200);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -3;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (!str)
    {
        return -4;
    }
    
    /* query the message if send */
    memset(neul_dev.rbuf, 0, 64);
    ret = neul_dev.ops->dev_write(cmd2, strlen(cmd2), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -5;
    }
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 64, 0, 200);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -6;
    }
    str = strstr(neul_dev.rbuf,"SENT="); 
    if (!str)
    {
        return -7;
    }
    sscanf(str, "SENT=%d,%s", &curcnt, neul_dev.wbuf);
    if (curcnt == sndcnt)
    {
        return -8;
    }
    sndcnt = curcnt;
    return 0;
}

/* ============================================================
func name   :  bc95_read_coap_msg
discription :  recv coap message(hex data) from cdp server 
               
param       :  buf @ input param , the data that send to cdp
               sendlen @ input param, data length
return      :  >=0 is read data length , <0 not ok
Revision    : 
=============================================================== */
int bc95_get_unrmsg_count(void)
{
    char *cmd = "AT+NQMGR\r\n";
    int ret = 0;
    char *str = NULL;
    int msgcnt = 0;
    
    memset(neul_dev.rbuf, 0, 128);
    ret = neul_dev.ops->dev_write(cmd, strlen(cmd), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -1;
    }
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 128, 0, 100);
    if (ret < 0)
    {
        //read bc95 read set return value info failed
        return -2;
    }
    str = strstr(neul_dev.rbuf,"BUFFERED");
    if (!str)
    {
        return 0;
    }
    sscanf(neul_dev.rbuf, "\r\nBUFFERED=%d,%s", &msgcnt, neul_dev.wbuf);
    if (msgcnt < 0 )
    {
        return 0;
    }
    return msgcnt;
}

/* ============================================================
func name   :  bc95_read_coap_msg
discription :  recv coap message(hex data) from cdp server 
               
param       :  buf @ input param , the data that send to cdp
               sendlen @ input param, data length
return      :  >=0 is read data length , <0 not ok
Revision    : 
=============================================================== */
int bc95_read_coap_msg(char *outbuf, int maxrlen)
{
    char *cmd = "AT+NMGR\r\n";
    int ret = 0;
    char *str = NULL;
    int readlen = 0;

    if (NULL == outbuf || maxrlen == 0)
    {
        return -1;
    }

    ret = neul_dev.ops->dev_write(cmd, strlen(cmd), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -2;
    }
    memset(neul_dev.rbuf, 0, NEUL_MAX_BUF_SIZE);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, NEUL_MAX_BUF_SIZE, 0, 540);
    if (ret < 0)
    {
        //read bc95 read set return value info failed
        return -3;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (!str && strlen(neul_dev.rbuf) <=10 )
    {
        return 0;
    }
    sscanf(neul_dev.rbuf, "\r\n%d,%s\r\n\r\nOK\r\n",&readlen, bc95_tmpbuf);
    if (readlen > 0)
    {
        bc95_str_to_hex(bc95_tmpbuf, readlen*2, outbuf);
        return readlen;
    }
    return 0;
}

/* ============================================================
func name   :  bc95_create_udpsocket
discription :  create udp socket 
               
param       :  localport @ input param , the port local socket used
return      :  >=0 socket handle , < 0 not ok
Revision    : 
=============================================================== */
int bc95_create_udpsocket(unsigned short localport)
{
    char *cmd = "AT+NSOCR=DGRAM,17,";
    int ret = 0;
    char *str = NULL;
    int socket = -1;
    if (0 == localport)
    {
        return -1;
    }
    memset(neul_dev.wbuf, 0, 64);
    sprintf(neul_dev.wbuf, "%s%d,1\r", cmd, localport);
    ret = neul_dev.ops->dev_write(neul_dev.wbuf, strlen(neul_dev.wbuf), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -2;
    }
    memset(neul_dev.rbuf, 0, 32);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 32, 0, 500);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -3;
		}
    str = strstr(neul_dev.rbuf,"OK");
    if (!str)
    {
        return -4;
    }
    sscanf(neul_dev.rbuf, "%d\r%s",&socket, bc95_tmpbuf);
    if (socket >= 0)
    {
        return socket;
    }
    return 0;
}

/* ============================================================
func name   :  bc95_close_udpsocket
discription :  close udp socket 
               
param       :  socket @ input param , the socket handle that need close
return      :  0 close ok , !0 not ok
Revision    : 
=============================================================== */
int bc95_close_udpsocket(int socket)
{
    char *cmd = "AT+NSOCL=";
    int ret = 0;
    char *str = NULL;
    
    memset(neul_dev.wbuf, 0, 64);
    sprintf(neul_dev.wbuf, "%s%d\r", cmd, socket);
    ret = neul_dev.ops->dev_write(neul_dev.wbuf, strlen(neul_dev.wbuf), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -1;
    }
    memset(neul_dev.rbuf, 0, 32);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 32, 0, 200);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -2;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (!str)
    {
        return -3;
    }

    bc95_reset_upd_param(socket);
    
    return 0;
}

/* ============================================================
func name   :  bc95_socket_config_remoteinfo
discription :  set remote address info that socket will send data to 
               
param       :  socket @ input param , the socket handle that need close
return      :  0 set ok , !0 not ok
Revision    : 
=============================================================== */
int bc95_socket_config_remoteinfo(int socket, const char *remoteip, unsigned short port)
{
    int ret = 0;
    if (socket < 0 || NULL == remoteip || port == 0)
    {
        return -1;
    }
    ret = bc95_set_upd_param(remoteip, port, socket);
    return ret;
}

/* ============================================================
func name   :  bc95_udp_send
discription :  send data to socket 
               
param       :  socket @ input param , the data will send to this socket
               buf @ input param, the data buf
               sendlen @ input param, the send data length
return      :  0 send data ok , !0 not ok
Revision    : 
=============================================================== */
int bc95_udp_send(int socket, const char *buf, int sendlen)
{
      //AT+NSOST=0,192.53.100.53,5683,25,400241C7B17401724D0265703D323031363038323331363438
    char *cmd = "AT+NSOST=";
    int ret = 0;
    char *str = NULL;
    
    sscanf(neul_dev.rbuf, "%d\r%s",&socket, bc95_tmpbuf);
    if (socket < 0 || NULL == buf || 0 == sendlen)
    {
        return -1;
    }
    
    memset(neul_dev.wbuf, 0, NEUL_MAX_BUF_SIZE);
    memset(bc95_tmpbuf, 0, NEUL_MAX_BUF_SIZE);
    bc95_hex_to_str((unsigned char *)buf, sendlen, bc95_tmpbuf);
    sprintf(neul_dev.wbuf, "%s%d,%s,%d,%d,%s\r", cmd, socket,
            (neul_dev.addrinfo+socket)->ip,
            (neul_dev.addrinfo+socket)->port,
            sendlen,
            bc95_tmpbuf);
    ret = neul_dev.ops->dev_write(neul_dev.wbuf, strlen(neul_dev.wbuf), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -2;
    }
    memset(neul_dev.rbuf, 0, 200);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 200, 0, 2000);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -3;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (!str)
    {
        return -4;
    }
    return (*(str+10)-'0')*10+(*(str+11)-'0');
}
int bc95_udp_msg_arrived(void)
{
	int ret = 0;
    char *str = NULL;
 memset(neul_dev.rbuf, 0, 32);
	ret = neul_dev.ops->dev_read(neul_dev.rbuf, 32, 0, 200);
    if (ret <= 0)
    {
        
        return -1;
    }
    str = strstr(neul_dev.rbuf,"+NSONMI:");
    if (!str)
    {
        return -1;
    }
		
    return (*(str+10)-'0')*10+((*(str+11)-'0'));
}

/* ============================================================
func name   :  bc95_udp_read
discription :  read data from socket 
               
param       :  socket @ input param , the data will read from this socket
               buf @ out param, store the data read
               maxrlen @ input param, the max read data length
return      :  >0 read data length , <0 not ok
Revision    : 
=============================================================== */
int bc95_udp_read(int socket,char *buf, int maxrlen, int mode)
{
    //AT+NSORF=0,4 
    char *cmd = "AT+NSORF=";
    int ret = 0;
    char *str = NULL;
    int rlen = 0;
    int rskt = -1;
    int port = 0;
    int readleft = 0;

     if (socket < 0 || NULL == buf || maxrlen <= 0)
    {
        return -1;
    }
    
    sprintf(neul_dev.wbuf, "%s%d,%d\r", cmd, socket, maxrlen);
    ret = neul_dev.ops->dev_write(neul_dev.wbuf, strlen(neul_dev.wbuf), 0);
    if (ret < 0)
    {
        //write data to bc95 failed
        return -2;
    }
    memset(neul_dev.rbuf, 0, 150);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 150, 0, 400);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -3;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (!str)
    {
        return -4;
    }
    sscanf(neul_dev.rbuf, "%d,%13s,%d,%d,%30s,%d\r\n", &rskt,    //暂时根据已知回复消息拆分
                            bc95_tmpbuf,
                            &port,
                            &rlen,
                            bc95_buf,
                            &readleft);
//		printf( "data1:%d_%s_%d_%d_%s_%d\r\n", rskt,
//                            bc95_tmpbuf,
//                            port,
//                            rlen,
//                            bc95_buf,
//                            readleft);	
		bc95_hexstr_to_byte(bc95_buf, readbuf, 30);
		printf("revice_hex:%s\r\n",bc95_buf);
    printf("revice_str:%s\r\n",readbuf);		
    return rlen;
}

/* ============================================================
func name   :  bc95_set_auto_connect
discription :  set bc95 module auto connect network
               
param       :  flag @ input param   0 set not auto connect, 1 set auto connect
return      :  0 set ok , !0 not ok
Revision    : 
=============================================================== */
int bc95_set_auto_connect(int flag)
{
    int ret = 0;
    char *str = NULL;
    char *p1 = "AT+NCONFIG=AUTOCONNECT,TRUE\r";
    char *p2 = "AT+NCONFIG=AUTOCONNECT,FALSE\r";

    if (flag)
    {
        //set auto connect
        ret = neul_dev.ops->dev_write(p1, strlen(p1), 0);
    }
    else
    {
        //not auto connect
        ret = neul_dev.ops->dev_write(p2, strlen(p2), 0);
    }
    memset(neul_dev.rbuf, 0, 32);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 32, 0, 300);
    if (ret <= 0)
    {
        //read bc95 read set return value info failed
        return -1;
    }
    str = strstr(neul_dev.rbuf,"OK");
    if (!str)
    {
        return -1;
    }
    return 0;
}

/* ============================================================
func name   :  bc95_hw_init
discription :  init neul bc95 device 
               
param       :  None
return      :  0 init ok , !0 not ok
Revision    : 
=============================================================== */
int bc95_hw_init(void)
{
    int ret = 0;
		
		//Reboot BC95
    ret = bc95_reboot();
		printf("Reboot:%d\r\n", ret);
		
    ret = bc95_hw_detect();
		printf("Sys staute:%d\r\n", ret);
	/*串口调试请注释以下一段*/
//    if (ret < 0)
//    {
//        return ret;
//    }
/**************************/
//    bc95_prepare_transmit();
//    //set auto connect
//    bc95_set_auto_connect(1);
//    bc95_sleep(1000);

//    bc95_prepare_transmit();
//    bc95_sleep(200);
//    ret = bc95_active_network();
 return 0;
    
}
int bc95_prepare_transmit(void)
{
    int ret = 0;
    char *p1 = "AT\r";
    
    memset(neul_dev.rbuf, 0, 32);
    ret = neul_dev.ops->dev_write(p1, strlen(p1), 0);
    ret = neul_dev.ops->dev_read(neul_dev.rbuf, 32, 0, 500);
	  uart_data_flush();
    return ret;
}
int bc95_sleep(int ticks)
{
  HAL_Delay(ticks);
    return 0;
}

/*******************************************************
* Function: uart_data_read
*	Param:
*	Retu: -1:error 0: No Data  Length
*
********************************************************/



int uart_data_read(char *buf, int maxrlen, int mode, int timeout)
{
    int retlen = 0;
    if (NULL == buf || maxrlen <= 0 || timeout < 0)
    {
        return 0;
    }
    if(mode == 0)
    {
        //block mode
				memset(buf, 0, maxrlen);
        retlen = HAL_UART_Receive(&huart2, buf, maxrlen, timeout);
				if(retlen == HAL_ERROR)
				{
					return -1;
				}
				return strlen((const char*)buf);		//返回收到数据的长度
    }
    else
    {
        //none block mode
        retlen = HAL_UART_Receive(&huart2, buf, maxrlen, 0);
    }
    
    return retlen;
}
//static int uart_data_write(const char *buf, int writelen, int mode)
int uart_data_write(char *buf, int writelen, int mode)
{
    if (NULL == buf || writelen <= 0)
    {
        return 0;
    }
    HAL_UART_Transmit(&huart2, buf, writelen, 500);
    return 0;
}

int uart_data_flush(void)
{
    memset(bc95_buf, 0, NEUL_MAX_BUF_SIZE);
    return 0;
}
