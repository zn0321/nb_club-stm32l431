/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BC95_H
#define __BC95_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
typedef int (*neul_read)(char *buf, int maxrlen, int mode, int timeout);
typedef int (*neul_write)(char *buf, int writelen, int mode);

typedef struct _neul_dev_operation_t 
{
    neul_read dev_read;
    neul_write dev_write;
} neul_dev_operation_t;

typedef struct _remote_info_t
{
    unsigned short port;
    int socket;
    char ip[16];
}remote_info;

typedef struct _neul_dev_info_t 
{
    char *rbuf;
    char *wbuf;
    int remotecount;
    remote_info *addrinfo;
    neul_dev_operation_t *ops;
    char cdpip[16];
} neul_dev_info_t;


/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */
int bc95_set_upd_param(const char *rip, const unsigned short port, const int socket);
int bc95_hw_detect(void);
int bc95_set_band(int band);
int bc95_get_scq(void);
int bc95_get_cfunstat(void);
int bc95_get_netstat(void);
int bc95_get_regstat(void);
int bc95_get_connetstat(void);
int bc95_set_imei(const char *imei, int len);
int bc95_check_imei(void);
int bc95_active_network(void);
int bc95_query_ip(void);
int bc95_get_time(void);
int bc95_reboot(void);
int bc95_set_cdpserver(const char *ipaddr);
int bc95_send_coap_paylaod(const char *buf, int sendlen);
int bc95_get_unrmsg_count(void);
int bc95_read_coap_msg(char *outbuf, int maxrlen);
int bc95_create_udpsocket(unsigned short localport);
int bc95_close_udpsocket(int socket);
int bc95_socket_config_remoteinfo(int socket, const char *remoteip, unsigned short port);
int bc95_udp_send(int socket, const char *buf, int sendlen);
int bc95_udp_msg_arrived (void);
int bc95_udp_read(int socket,char *buf, int maxrlen, int mode);
int bc95_hw_init(void);
int bc95_create_data_task(void);
int bc95_set_auto_connect(int flag);
int bc95_sleep(int ticks);
int bc95_prepare_transmit(void);
int uart_data_flush(void);
int bc95_hexstr_to_byte(const char* source,  char* dest, int sourceLen); 

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
