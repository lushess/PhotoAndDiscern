/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __CONFIG_H
#define __CONFIG_H

/*=========================
   Application configuration
 *=========================*/

//#define CONFIG_SYSTEM_SAVE_FILE_PATH          "/SystemSave.json"
//#define CONFIG_SYSTEM_SAVE_FILE_BACKUP_PATH   "/.SystemSaveBackup.json"
//#define CONFIG_SYSTEM_LANGUAGE_DEFAULT        "cn-GB"


#define __EXRAM    __attribute__((section("EXRAM")))     
#define __INRAM    __attribute__((section("INRAM"))) 

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

#define USE_FREERTOS_MALLOCATOR_ENABLE		1   // 如果DISABLE，需把分配给FREERTOS的EXRAM堆空间分配到系统HEAP节区
		#if USE_FREERTOS_MALLOCATOR_ENABLE
				#define USE_FREERTOS_MALLOCATOR
		#endif //USE_FREERTOS_MALLOCATOR_ENABLE

#define USART_OUTPUT_DEBUG_ENABLE 		1
		#if USART_OUTPUT_DEBUG_ENABLE
				#define USART_OUTPUT_DEBUG
				#define DEBUG_USART_USE_DMA_ENABLE    0
						#if DEBUG_USART_USE_DMA_ENABLE
								#define DEBUG_USART_USE_DMA
						#endif //DEBUG_USART_USE_DMA_ENABLE
		#endif //USART_OUTPUT_DEBUG_ENABLE

/* 内存记录 */
#define EXRAM_TOTAL         			((size_t)(0x00200000))
#define INRAM_TOTAL               ((size_t)(0x00010000))

/* FREERTOS */
#define FREERTOS_HEAP_SIZE_INRAM 	((size_t)(0x00006000))
#define FREERTOS_HEAP_SIZE_EXRAM 	((size_t)(0x0003FA00))


#endif //__CONFIG_H
