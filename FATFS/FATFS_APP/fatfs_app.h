#ifndef _fatfs_app_H
#define _fatfs_app_H


#ifdef __cplusplus
extern "C"{
#endif

#include "system.h"
#include "ff.h" 

typedef struct
{
	uint8_t type[6];		   //��׺6���ֽ�
	uint8_t name[100];		   //·�����ļ�����100���ֽڣ�֧��25�����ִ�С���֣�
} FileNameTypeDef;


extern FATFS *fs[_VOLUMES];  
extern FIL *file;	 
extern FIL *ftemp;	 
extern UINT br,bw;
extern FILINFO *fileinfo;
extern DIR *dir;
extern u8 fs_ready_status[_VOLUMES];//����״̬��Ϊ0��ʾ����
extern u8 *fatbuf;//���ݻ���������СΪ1024���ֽ�

extern const char DRIVER_TBL[_VOLUMES][2];
//f_typetell���ص����Ͷ���
//���ݱ�FILE_TYPE_TBL���.��exfuns.c���涨��
/*
#define T_BIN		0X00	//bin�ļ�
#define T_LRC		0X10	//lrc�ļ�
#define T_NES		0X20	//nes�ļ�

#define T_TEXT		0X30	//.txt�ļ�
#define T_C			0X31	//.c�ļ�
#define T_H			0X32    //.h�ļ�

#define T_WAV		0X40	//WAV�ļ�
#define T_MP3		0X41	//MP3�ļ� 
#define T_APE		0X42	//APE�ļ�
#define T_FLAC		0X43	//FLAC�ļ�

#define T_BMP		0X50	//bmp�ļ�
#define T_JPG		0X51	//jpg�ļ�
#define T_JPEG		0X52	//jpeg�ļ�		 
#define T_GIF		0X53	//gif�ļ�   

#define T_AVI		0X60	//avi�ļ�  
*/

u8 FATFS_Init(void);							//�����ڴ�
u8 FATFS_ScanFilesType(u8 *fname);						//ʶ���ļ�����
u8 FATFS_ScanFiles(uint8_t *dir, FileNameTypeDef *file);
u8 FATFS_GetFree(u8 *drv,u32 *total,u32 *free);	//�õ�������������ʣ������

u8 lv_FATFS_Init(FATFS *fs[_VOLUMES],u8 fsres[_VOLUMES]); //lvgl��ʼ��FATFS

#ifdef __cplusplus
}
#endif

#endif
