#ifndef __usart_H
#define __usart_H

#include "system.h" 
#include "stdio.h" 
#include "../User/App/Config/Config.h"


	
void USART1_Init(uint32_t bound);
void UsartRX_DMA_Init(void);
void UsartRX_DMA_Enable(uint16_t bufsize);
void UsartTX_DMA_Init(void);
void UsartTX_DMA_Enable(char* buff,uint16_t bufsize);



//串口打印调试信息，供串口调试助手查看
#ifdef USART_OUTPUT_DEBUG
		#ifdef DEBUG_USART_USE_DMA
				#define DEBUG_BUFF_SIZE     128 
				extern volatile u8 usart_dma_tx_busy_buf1;
				extern volatile u8 usart_dma_tx_busy_buf2;
				extern volatile u8 usartbuf1_ready;
				extern volatile u8 usartbuf2_ready;
				extern char usartbuf1[DEBUG_BUFF_SIZE];
				extern char usartbuf2[DEBUG_BUFF_SIZE];
				#define UDEBUG(fmt,...)      do{\
																					u16 i = 0;\
																					if((0 == usart_dma_tx_busy_buf1)&&(0 == usart_dma_tx_busy_buf2)){\
																							if(0 != usartbuf1_ready){\
																								usart_dma_tx_busy_buf1 = 0xff;\
																								UsartTX_DMA_Enable(usartbuf1,strlen(usartbuf1));\
																								usartbuf1_ready = 0;\
																							}\
																							else if(0 != usartbuf2_ready) {\
																								usart_dma_tx_busy_buf2 = 0xff;\
																								UsartTX_DMA_Enable(usartbuf2,strlen(usartbuf2));\
																								usartbuf2_ready = 0;\
																							}\
																							else {\
																								memset(usartbuf1,0,sizeof(usartbuf1));\
																								i = snprintf(usartbuf1,sizeof(usartbuf1),"[%s][%d][%s]"fmt"\r\n",__FILE__,__LINE__,__func__,##__VA_ARGS__);\
																								if(i >0){\
																										usart_dma_tx_busy_buf1 = 0xff;\
																										UsartTX_DMA_Enable(usartbuf1,i);\
																								}\
																							}\
																					}\
																					else {\
																						  if((0 == usartbuf1_ready)&&(0 != usart_dma_tx_busy_buf2)){\
																								memset(usartbuf1,0,sizeof(usartbuf1));\
																								i = snprintf(usartbuf1,sizeof(usartbuf1),"[%s][%d][%s]"fmt"\r\n",__FILE__,__LINE__,__func__,##__VA_ARGS__);\
																								if(i >0) usartbuf1_ready = 0xff;\
																							}\
																							else if((0 == usartbuf2_ready)&&(0 != usart_dma_tx_busy_buf1)){\
																								memset(usartbuf2,0,sizeof(usartbuf2));\
																								i = snprintf(usartbuf2,sizeof(usartbuf2),"[%s][%d][%s]"fmt"\r\n",__FILE__,__LINE__,__func__,##__VA_ARGS__);\
																								if(i >0) usartbuf2_ready = 0xff;\
																							}\
																					}\
																			}while(0)
		#else
				#define DEBUG_BUFF_SIZE      0
				#define UDEBUG(fmt,...)      do{\
																					printf("[%s][%d][%s]"fmt"\r\n",__FILE__,__LINE__,__func__,##__VA_ARGS__);\
																				}while(0)
		#endif //DEBUG_USART_USE_DMA
#else
		#define DEBUG_BUFF_SIZE      0
		#define UDEBUG(fmt,...)
#endif //USART_OUTPUT_DEBUG

#endif //__usart_H


