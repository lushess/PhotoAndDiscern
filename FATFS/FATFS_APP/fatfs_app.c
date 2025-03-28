#include "fatfs_app.h"
#include <string.h>
#include "Config/Config.h"
#include <stdlib.h>


#define FILE_MAX_TYPE_NUM		7	//最多FILE_MAX_TYPE_NUM个大类
#define FILE_MAX_SUBT_NUM		4	//最多FILE_MAX_SUBT_NUM个小类

const char DRIVER_TBL[_VOLUMES][2]={{"0:"},{"1:"},{"2:"},};

 //文件类型列表
u8* const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BIN文件
{"LRC"},			//LRC文件
{"NES"},			//NES文件
{"TXT","C","H"},	//文本文件
{"WAV","MP3","APE","FLAC"},//支持的音乐文件
{"BMP","JPG","JPEG","GIF"},//图片文件
{"AVI"},			//视频文件
};
///////////////////////////////公共文件区,使用malloc的时候////////////////////////////////////////////
FATFS *fs[_VOLUMES] = {NULL};//逻辑磁盘工作区.
FIL *file = NULL;	  		//文件1
FIL *ftemp = NULL;	  		//文件2.
UINT br,bw;			//读写变量
FILINFO *fileinfo = NULL;	//文件信息
DIR *dir = NULL;  			//目录
u8 fs_ready_status[_VOLUMES];//磁盘状态，为0表示就绪


u8 *fatbuf = NULL;			//数据缓存区，大小为1024个字节
///////////////////////////////////////////////////////////////////////////////////////
//为exfuns申请内存
//返回值:0,成功
//1,失败
u8 FATFS_Init(void)
{
	u8 i;
	for(i=0;i<_VOLUMES;i++)
	{
		fs[i]=(FATFS*)malloc(sizeof(FATFS));	//为磁盘i工作区申请内存	
		if(!fs[i])break;
	}
	file=(FIL*)malloc(sizeof(FIL));		//为file申请内存
	ftemp=(FIL*)malloc(sizeof(FIL));		//为ftemp申请内存
	fatbuf=(u8*)malloc(512);				//为fatbuf申请内存
	if(i==_VOLUMES&&file&&ftemp&&fatbuf)return 0;  //申请有一个失败,即失败.
	else return 1;
}

u8 lv_FATFS_Init(FATFS *fs[_VOLUMES],u8 fsres[_VOLUMES])
{
	u8 i;
	u8 *pfsres=fsres;
	FATFS **pfs=fs;
	FRESULT res;
    
	/* 防止多次调用内存泄漏 */
	if(file != NULL)free((void *)file);
	if(ftemp != NULL)free((void *)ftemp);
	if(fileinfo != NULL)free((void *)fileinfo);
	if(dir != NULL)free((void *)dir);
	if(fatbuf != NULL)free((void *)fatbuf);
	for(i=0;i<_VOLUMES;i++)
	{
		pfsres[i]=0;
		if(pfs[i]!= NULL) free((void *)pfs[i]);
		pfs[i]=(FATFS*)malloc(sizeof(FATFS));	//为磁盘i工作区申请内存	
		if(!pfs[i]) pfsres[i]=1; //分配失败置状态为1
		else
		{
			res = f_mount(pfs[i],DRIVER_TBL[i],1); //挂载文件系统
			if(res == FR_NO_FILESYSTEM)
			{
				res = f_mkfs(DRIVER_TBL[i],0,0); //没有文件系统则格式化磁盘
				if(res!=FR_OK) pfsres[i]=1;
				else
				{
					res = f_mount(NULL,DRIVER_TBL[i],1);
				  res = f_mount(pfs[i],DRIVER_TBL[i],1); //重新挂载文件系统
				}							
			}
			if(res!=FR_OK) pfsres[i]=1;
	  }
	}
	
	file=(FIL*)malloc(sizeof(FIL));		//为file申请内存
	ftemp=(FIL*)malloc(sizeof(FIL));		//为ftemp申请内存
	fileinfo=(FILINFO *)malloc(sizeof(FILINFO));//为fileinfo申请内存
	dir=(DIR *)malloc(sizeof(DIR)); //为目录申请内存
	fatbuf=(u8*)malloc(512);				//为fatbuf申请内存
	if(file&&ftemp&&fileinfo&&dir&&fatbuf) return 0;
	else return 1;
}


//将小写字母转为大写字母,如果是数字,则保持不变.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//数字,保持不变.
	if(c>='a')return c-0x20;//变为大写.
	else return c;//大写,保持不变
}	      
//报告文件的类型
//fname:文件名
//返回值:0XFF,表示无法识别的文件类型编号.
//		 其他,高四位表示所属大类,低四位表示所属小类.
u8 FATFS_ScanFilesType(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr;//后缀名
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//偏移到了最后了.
		fname++;
	}
	if(i==250)return 0XFF;//错误的字符串.
 	for(i=0;i<5;i++)//得到后缀名
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
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//全部变为大写 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//大类对比
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//子类对比
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//此组已经没有可对比的成员了.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//找到了
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//没找到		 			   
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
	/* 打开文件夹 */
	res = f_opendir(&dirs, (const TCHAR*)dir);
	
	if (res == FR_OK)    //成功打开文件夹 
	{    
		while (f_readdir(&dirs, &fileInfo) == FR_OK)   //按照顺序读文件夹 
		{ 	 
			if(!fileInfo.fname[0]) 	  //如果文件名为0,结束 
			{
				break;     
			} 	
			
			/* 判断文件属性 */								
			if((fileInfo.fattrib == AM_ARC)|(fileInfo.fattrib == AM_DIR)) 
			{

#if _USE_LFN	//为长字符串名准备
            	fn = *fileInfo.lfname ? fileInfo.lfname : fileInfo.fname;
#else
           	 	fn = fileInfo.fname;
#endif

				/* 读取名字 */
				i = 0;
	       		while((*fn != '.') && (*fn != '\0')) //有一个成立就跳出循环
				{
					file->name[i] = *fn;
					i++;
					fn++;	
				}
                file->name[i] = '\0';

				/* 读取后缀 */
				i = 0;
				while(*fn != '\0')
				{
					file->type[i] = *fn;
					i++;
					fn++;
				}
                file->type[i] = '\0';
                 
				file++;
			}//end of 文件属性判断呢
		} //end of while (f_readdir(&dirs, &fileInfo) == FR_OK)//按照顺序读文件夹
	}
	return res;
}


//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
u8 FATFS_GetFree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//单位为KB
		*free=fre_sect>>1;	//单位为KB 
 	}
	return res;
}	




