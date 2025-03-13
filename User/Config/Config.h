#ifndef __CONFIG_H
#define __CONFIG_H

/*=========================
   Application configuration
 *=========================*/

#define __EXRAM    __attribute__((section("EXRAM")))     
#define __INRAM    __attribute__((section("INRAM"))) 
#define __USED	   __attribute__((used)) //��ֹ�������Ż�

#define __ALIGN(x) __attribute__((aligned(x)))


#define LV_COLOR_DEPTH	 16

#define USE_FREERTOS		1

//�ڴ������
#define USE_FREERTOS_MALLOCATOR_ENABLE		1  //ע�����freeRTOS�ڴ���亯���������ڴ�ķ�Χ
		#if USE_FREERTOS_MALLOCATOR_ENABLE
				#define USE_FREERTOS_MALLOCATOR
		#else
				#include <string.h>
				#define Memalloc malloc
				#define Memfree free
		#endif //USE_FREERTOS_MALLOCATOR_ENABLE

#define USE_EXRAM_IMAGE_BUFFER_ENABLE		0		//�ⲿSRAMͼ�񻺳���ʹ��
		#if USE_EXRAM_IMAGE_BUFFER_ENABLE
				#define USE_EXRAM_IMAGE_BUFFER
		#endif //USE_EXRAM_IMAGE_BUFFER_ENABLE


//DEBUG����

#define USART_OUTPUT_DEBUG_ENABLE 		1
		#if USART_OUTPUT_DEBUG_ENABLE
				#define USART_BOUND		9600
				#define USART_OUTPUT_DEBUG
				#define DEBUG_USART_USE_DMA_ENABLE    0
						#if DEBUG_USART_USE_DMA_ENABLE
								#define DEBUG_USART_USE_DMA
						#endif //DEBUG_USART_USE_DMA_ENABLE
		#endif //USART_OUTPUT_DEBUG_ENABLE

/* �ڴ��¼ */
#define EXRAM_TOTAL         			((size_t)(0x00040000))
#define INRAM_TOTAL               ((size_t)(0x00010000))

/* FREERTOS */
#define FREERTOS_USE_EXRAM_ENABLE	1
		#if	FREERTOS_USE_EXRAM_ENABLE
				#define FREERTOS_USE_EXRAM
		#endif //FREERTOS_USE_EXRAM_ENABLE
#define FREERTOS_HEAP_SIZE 	((size_t)(0x00008000))


#endif //__CONFIG_H
