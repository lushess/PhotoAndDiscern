/**
 * @file lv_port_fs_Fatfs.h
 *
 */

 /*Copy this file as "lv_port_fs.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_PORT_FS_TEMPL_H
#define LV_PORT_FS_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"
/*********************
 *      DEFINES
 *********************/
#define LV_FS_USE_LOG 1

#if !defined(STM32F10X_HD) && LV_FS_USE_LOG
#include <stdio.h>
#  define _FS_LOG(format, ...)      printf("[FS]" format "\r\n", ##__VA_ARGS__)
#  define FS_LOG_INFO(format, ...)  //_FS_LOG("[Info] "format, ##__VA_ARGS__)
#  define FS_LOG_WARN(format, ...)  _FS_LOG("[Warn] " format, ##__VA_ARGS__)
#  define FS_LOG_ERROR(format, ...) _FS_LOG("[Error] " format, ##__VA_ARGS__)
#else
#  define fS_LOG_INFO(...)
#  define FS_LOG_WARN(...)
#  define FS_LOG_ERROR(...)
#endif

#define fsAssertCalled(char,int)\
do{\
   printf("Error:%s,%d\r\n",char,int);\
   while(1);\
}while(0)

#define FS_ASSERT(x) \
do{\
 if((x)==0) fsAssertCalled(__FILE__,__LINE__);\
}while(0)	
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
 void lv_port_fs_init(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_FS_TEMPL_H*/

#endif /*Disable/Enable content*/
