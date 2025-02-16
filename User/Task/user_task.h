#ifndef __USER_TASK_H
#define __USER_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"


#define DISREFRESH_EVENT    (0x01<<2)
#define ENREFRESH_EVENT     (0x01<<1)
#define REFRESH_EVENT       0x01

#define EAN13_DISCERN       (0x01<<3)
#define CHARA_DISCERN       (0x01<<4)

#define SDCARD_PHOTO_PATH_DEFAULT  "0:/Photo"
#define FLASH_PHOTO_PATH_DEFAULT   "1:/Photo"

extern TaskHandle_t camera_refresh_Handle;
extern TaskHandle_t discern_Handle;
extern TaskHandle_t photo_Handle;


//位图文件头
typedef __packed struct
{
    uint16_t  type ;     //文件标志.固定为'BM'(0x424d),用来识别BMP位图类型
    uint32_t  size ;	   //文件大小
    uint16_t  reserved1 ;//保留，总为0
    uint16_t  reserved2 ;//保留，总为0
    uint32_t  Offset ;  //从文件开始到位图数据(bmp data)开始之间的的偏移量，无调色板54字节
} BMP_Header_t ;

//位图信息头
typedef __packed struct
{
    uint32_t biSize ;		   	//BMP_Info_Header_t结构size,40。
    long  biWidth ;		   	    //图象的宽度，以像素为单位 
    long  biHeight ;	   	    //图象的高度，以像素为单位 
    uint16_t  biPlanes ;	    //平面数，总为1 
    uint16_t  biBitCount ;	   	//颜色位数，为1、4、8、16、24、或32
    uint32_t biCompression ;  	//图象数据压缩的类型。0：没有压缩；	                              
    uint32_t biSizeImage ;		//图象的大小，以字节为单位。当用BI_RGB格式时，可设置为0  
    long  biXPelsPerMeter ;	    //水平分辨率，用像素/米表示
    long  biYPelsPerMeter ;	    //垂直分辨率，用像素/米表示
    uint32_t biClrUsed ;	  	//使用的颜色数，为0表示默认值(2^颜色位数)
    uint32_t biClrImportant ; 	//对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要。 
} BMP_Info_Header_t ;

//调色板
typedef __packed struct 
{
    uint8_t b ;  //指定蓝色强度
    uint8_t g ;	 //指定绿色强度 
    uint8_t r ;	 //指定红色强度 
    uint8_t reserved ;//保留，设置为0 
} RGB_Quad_t ;

typedef __packed struct 
{ 
	BMP_Header_t header;     //位图文件头
	BMP_Info_Header_t infoheader;     //位图信息头 
	//uint32_t rgbmask[3];			//调色板用于存放RGB掩码.
	//RGB_Quad_t qua[256];
    //bmp data  
} bmpinfo_t; 

extern bmpinfo_t bmpinfo;

void camera_refresh(void);
void photo(void);
void discern(void);

#ifdef __cplusplus
}
#endif /* extern "C" */

void TaskSwitchTo(void);

#endif /* __USER_TASK_H */
