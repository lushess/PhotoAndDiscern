#ifndef __CONFIG_H
#define __CONFIG_H

/*=========================
   Application configuration
 *=========================*/

#define __EXRAM    __attribute__((section("EXRAM")))     
#define __INRAM    __attribute__((section("INRAM"))) 
#define __USED	   __attribute__((used)) //防止编译器优化

#define __ALIGN(x) __attribute__((align(x)))


#define LV_COLOR_DEPTH	 16
//#define CONFIG_LVGL_USE_SRAMEX_ENABLE    1
//		#if CONFIG_LVGL_USE_SRAMEX_ENABLE
//		#define CONFIG_LVGL_USE_SRAMEX
//#endif //CONFIG_LVGL_USE_SRAMEX_ENABLE

//#define CONFIG_LVGL_USE_MALLOC_ENABLE 	 1  //同时影响其他使用SRAMEX的任务
//		#if CONFIG_LVGL_USE_MALLOC_ENABLE
//		#define CONFIG_LVGL_USE_MALLOC
//#endif //CONFIG_LVGL_USE_MALLOC_ENABLE

#define USE_FREERTOS		1

//内存管理器
#define USE_FREERTOS_MALLOCATOR_ENABLE		1  //注意调用freeRTOS内存分配函数所分配内存的范围
		#if USE_FREERTOS_MALLOCATOR_ENABLE
				#define USE_FREERTOS_MALLOCATOR
		#else
				#include <string.h>
				#define Memalloc malloc
				#define Memfree free
		#endif //USE_FREERTOS_MALLOCATOR_ENABLE

#define USE_EXRAM_IMAGE_BUFFER_ENABLE		1		//外部SRAM图像缓冲区使能
		#if USE_EXRAM_IMAGE_BUFFER_ENABLE
				#define USE_EXRAM_IMAGE_BUFFER
		#endif //USE_EXRAM_IMAGE_BUFFER_ENABLE


//DEBUG设置
#define USART_OUTPUT_DEBUG_ENABLE 		1
		#if USART_OUTPUT_DEBUG_ENABLE
				#define USART_OUTPUT_DEBUG
				#define DEBUG_USART_USE_DMA_ENABLE    0
						#if DEBUG_USART_USE_DMA_ENABLE
								#define DEBUG_USART_USE_DMA
						#endif //DEBUG_USART_USE_DMA_ENABLE
		#endif //USART_OUTPUT_DEBUG_ENABLE

/* 内存记录 */
#define EXRAM_TOTAL         			((size_t)(0x00040000))
#define INRAM_TOTAL               ((size_t)(0x00010000))

/* FREERTOS */
#define FREERTOS_HEAP_SIZE_INRAM 	((size_t)(0x0000A000))
//#define FREERTOS_HEAP_SIZE_EXRAM 	((size_t)(0x0003FA00))


#endif //__CONFIG_H
