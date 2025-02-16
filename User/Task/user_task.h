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


//λͼ�ļ�ͷ
typedef __packed struct
{
    uint16_t  type ;     //�ļ���־.�̶�Ϊ'BM'(0x424d),����ʶ��BMPλͼ����
    uint32_t  size ;	   //�ļ���С
    uint16_t  reserved1 ;//��������Ϊ0
    uint16_t  reserved2 ;//��������Ϊ0
    uint32_t  Offset ;  //���ļ���ʼ��λͼ����(bmp data)��ʼ֮��ĵ�ƫ�������޵�ɫ��54�ֽ�
} BMP_Header_t ;

//λͼ��Ϣͷ
typedef __packed struct
{
    uint32_t biSize ;		   	//BMP_Info_Header_t�ṹsize,40��
    long  biWidth ;		   	    //ͼ��Ŀ�ȣ�������Ϊ��λ 
    long  biHeight ;	   	    //ͼ��ĸ߶ȣ�������Ϊ��λ 
    uint16_t  biPlanes ;	    //ƽ��������Ϊ1 
    uint16_t  biBitCount ;	   	//��ɫλ����Ϊ1��4��8��16��24����32
    uint32_t biCompression ;  	//ͼ������ѹ�������͡�0��û��ѹ����	                              
    uint32_t biSizeImage ;		//ͼ��Ĵ�С�����ֽ�Ϊ��λ������BI_RGB��ʽʱ��������Ϊ0  
    long  biXPelsPerMeter ;	    //ˮƽ�ֱ��ʣ�������/�ױ�ʾ
    long  biYPelsPerMeter ;	    //��ֱ�ֱ��ʣ�������/�ױ�ʾ
    uint32_t biClrUsed ;	  	//ʹ�õ���ɫ����Ϊ0��ʾĬ��ֵ(2^��ɫλ��)
    uint32_t biClrImportant ; 	//��ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ�� 
} BMP_Info_Header_t ;

//��ɫ��
typedef __packed struct 
{
    uint8_t b ;  //ָ����ɫǿ��
    uint8_t g ;	 //ָ����ɫǿ�� 
    uint8_t r ;	 //ָ����ɫǿ�� 
    uint8_t reserved ;//����������Ϊ0 
} RGB_Quad_t ;

typedef __packed struct 
{ 
	BMP_Header_t header;     //λͼ�ļ�ͷ
	BMP_Info_Header_t infoheader;     //λͼ��Ϣͷ 
	//uint32_t rgbmask[3];			//��ɫ�����ڴ��RGB����.
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
