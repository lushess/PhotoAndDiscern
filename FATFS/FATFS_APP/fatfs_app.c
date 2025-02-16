#include "fatfs_app.h"
#include <string.h>
#include "Config/Config.h"
#include <stdlib.h>


#define FILE_MAX_TYPE_NUM		7	//���FILE_MAX_TYPE_NUM������
#define FILE_MAX_SUBT_NUM		4	//���FILE_MAX_SUBT_NUM��С��

const char DRIVER_TBL[_VOLUMES][2]={{"0:"},{"1:"},{"2:"},};

 //�ļ������б�
u8* const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BIN�ļ�
{"LRC"},			//LRC�ļ�
{"NES"},			//NES�ļ�
{"TXT","C","H"},	//�ı��ļ�
{"WAV","MP3","APE","FLAC"},//֧�ֵ������ļ�
{"BMP","JPG","JPEG","GIF"},//ͼƬ�ļ�
{"AVI"},			//��Ƶ�ļ�
};
///////////////////////////////�����ļ���,ʹ��malloc��ʱ��////////////////////////////////////////////
FATFS *fs[_VOLUMES] = {NULL};//�߼����̹�����.
FIL *file = NULL;	  		//�ļ�1
FIL *ftemp = NULL;	  		//�ļ�2.
UINT br,bw;			//��д����
FILINFO *fileinfo = NULL;	//�ļ���Ϣ
DIR *dir = NULL;  			//Ŀ¼
u8 fs_ready_status[_VOLUMES];//����״̬��Ϊ0��ʾ����


u8 *fatbuf = NULL;			//���ݻ���������СΪ1024���ֽ�
///////////////////////////////////////////////////////////////////////////////////////
//Ϊexfuns�����ڴ�
//����ֵ:0,�ɹ�
//1,ʧ��
u8 FATFS_Init(void)
{
	u8 i;
	for(i=0;i<_VOLUMES;i++)
	{
		fs[i]=(FATFS*)malloc(sizeof(FATFS));	//Ϊ����i�����������ڴ�	
		if(!fs[i])break;
	}
	file=(FIL*)malloc(sizeof(FIL));		//Ϊfile�����ڴ�
	ftemp=(FIL*)malloc(sizeof(FIL));		//Ϊftemp�����ڴ�
	fatbuf=(u8*)malloc(512);				//Ϊfatbuf�����ڴ�
	if(i==_VOLUMES&&file&&ftemp&&fatbuf)return 0;  //������һ��ʧ��,��ʧ��.
	else return 1;
}

u8 lv_FATFS_Init(FATFS *fs[_VOLUMES],u8 fsres[_VOLUMES])
{
	u8 i;
	u8 *pfsres=fsres;
	FATFS **pfs=fs;
	FRESULT res;
    
	/* ��ֹ��ε����ڴ�й© */
	if(file != NULL)free((void *)file);
	if(ftemp != NULL)free((void *)ftemp);
	if(fileinfo != NULL)free((void *)fileinfo);
	if(dir != NULL)free((void *)dir);
	if(fatbuf != NULL)free((void *)fatbuf);
	for(i=0;i<_VOLUMES;i++)
	{
		pfsres[i]=0;
		if(pfs[i]!= NULL) free((void *)pfs[i]);
		pfs[i]=(FATFS*)malloc(sizeof(FATFS));	//Ϊ����i�����������ڴ�	
		if(!pfs[i]) pfsres[i]=1; //����ʧ����״̬Ϊ1
		else
		{
			res = f_mount(pfs[i],DRIVER_TBL[i],1); //�����ļ�ϵͳ
			if(res == FR_NO_FILESYSTEM)
			{
				res = f_mkfs(DRIVER_TBL[i],0,0); //û���ļ�ϵͳ���ʽ������
				if(res!=FR_OK) pfsres[i]=1;
				else
				{
					res = f_mount(NULL,DRIVER_TBL[i],1);
				  res = f_mount(pfs[i],DRIVER_TBL[i],1); //���¹����ļ�ϵͳ
				}							
			}
			if(res!=FR_OK) pfsres[i]=1;
	  }
	}
	
	file=(FIL*)malloc(sizeof(FIL));		//Ϊfile�����ڴ�
	ftemp=(FIL*)malloc(sizeof(FIL));		//Ϊftemp�����ڴ�
	fileinfo=(FILINFO *)malloc(sizeof(FILINFO));//Ϊfileinfo�����ڴ�
	dir=(DIR *)malloc(sizeof(DIR)); //ΪĿ¼�����ڴ�
	fatbuf=(u8*)malloc(512);				//Ϊfatbuf�����ڴ�
	if(file&&ftemp&&fileinfo&&dir&&fatbuf) return 0;
	else return 1;
}


//��Сд��ĸתΪ��д��ĸ,���������,�򱣳ֲ���.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//����,���ֲ���.
	if(c>='a')return c-0x20;//��Ϊ��д.
	else return c;//��д,���ֲ���
}	      
//�����ļ�������
//fname:�ļ���
//����ֵ:0XFF,��ʾ�޷�ʶ����ļ����ͱ��.
//		 ����,����λ��ʾ��������,����λ��ʾ����С��.
u8 FATFS_ScanFilesType(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr;//��׺��
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//ƫ�Ƶ��������.
		fname++;
	}
	if(i==250)return 0XFF;//������ַ���.
 	for(i=0;i<5;i++)//�õ���׺��
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  	}
	strcpy((char *)tbuf,(const char*)attr);//copy
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//ȫ����Ϊ��д 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//����Ա�
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//����Ա�
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//�����Ѿ�û�пɶԱȵĳ�Ա��.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//�ҵ���
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//û�ҵ�		 			   
}	 


u8 FATFS_ScanFiles(uint8_t *dir, FileNameTypeDef *file)
{
	FRESULT res;
	FILINFO  fileInfo;
	DIR dirs;
	char *fn, i = 0; 

#if _USE_LFN
    static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
    fileInfo.lfname = lfn;
    fileInfo.lfsize = sizeof(lfn);
#endif
	/* ���ļ��� */
	res = f_opendir(&dirs, (const TCHAR*)dir);
	
	if (res == FR_OK)    //�ɹ����ļ��� 
	{    
		while (f_readdir(&dirs, &fileInfo) == FR_OK)   //����˳����ļ��� 
		{ 	 
			if(!fileInfo.fname[0]) 	  //����ļ���Ϊ0,���� 
			{
				break;     
			} 	
			
			/* �ж��ļ����� */								
			if((fileInfo.fattrib == AM_ARC)|(fileInfo.fattrib == AM_DIR)) 
			{

#if _USE_LFN	//Ϊ���ַ�����׼��
            	fn = *fileInfo.lfname ? fileInfo.lfname : fileInfo.fname;
#else
           	 	fn = fileInfo.fname;
#endif

				/* ��ȡ���� */
				i = 0;
	       		while((*fn != '.') && (*fn != '\0')) //��һ������������ѭ��
				{
					file->name[i] = *fn;
					i++;
					fn++;	
				}
                file->name[i] = '\0';

				/* ��ȡ��׺ */
				i = 0;
				while(*fn != '\0')
				{
					file->type[i] = *fn;
					i++;
					fn++;
				}
                file->type[i] = '\0';
                 
				file++;
			}//end of �ļ������ж���
		} //end of while (f_readdir(&dirs, &fileInfo) == FR_OK)//����˳����ļ���
	}
	return res;
}


//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
u8 FATFS_GetFree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//�õ���������
	    fre_sect=fre_clust*fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//��λΪKB
		*free=fre_sect>>1;	//��λΪKB 
 	}
	return res;
}	




