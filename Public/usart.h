#ifndef __usart_H
#define __usart_H

#include "system.h" 
#include "stdio.h" 
#include "../User/App/Config/Config.h"

//���ڴ�ӡ������Ϣ�������ڵ������ֲ鿴
#ifdef USART_OUTPUT_DEBUG
#  define UDEBUG(fmt,...)      do{\
																	printf("[%s][%d][%s]"fmt"\r\n",__FILE__,__LINE__,__func__,##__VA_ARGS__);\
																	}while(0)
#else
#  define UDEBUG(fmt,...)
#endif

void USART1_Init();


#endif


