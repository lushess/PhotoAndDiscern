#include "characterdiscern.h"
#include "picture_tool.h"
#include "tftlcd.h"
#include "flash.h"
#include "usart.h"
#include "Config/Config.h"
#ifdef CONFIG_LVGL_USE_MALLOC
    #include <stdlib.h>
		#include <string.h>
#else
		#include "lvgl/lvgl.h"	 
#endif /*CONFIG_LVGL_USE_MALLOC*/

static u8 calcCharacterBit(vu8 *CharacterMatrix)
{
	vu8 sum=0,tim=32;
	while(tim--)
	{
		while(*CharacterMatrix)
		{
			*CharacterMatrix&=*CharacterMatrix-1;
			sum++;
		}
		CharacterMatrix++;
	}
	return sum;
}
static void calcGridBit(vu8 *CharacterMatrix,vu8 *Grid)
{
	vu8 sum=0,tim=32;
	while(tim--)
	{
		while(*CharacterMatrix)
		{
			*CharacterMatrix&=*CharacterMatrix-1;
			sum++;
		}
		sum=0;
		*Grid++=sum;
		CharacterMatrix++;
	}
}
static u16 FindMaxNumFromArray(vu16 *array,vu16 size)
{
  vu16 i,temp=0,flag=0;
	for(i=0;i<size;i++)
	{		
	  if(temp<*array) 
		{
			temp=*array;
			flag=i;
		}
    array++;		
	}
	return flag;
}
static __IO u16 *CalcChangePoint_V(vu8 *BinaryImage) //用于记录纵向跳变点，共240个数据，ChangePointArray_V[x]其中x为图片横坐标
{
	vu8 *TmpBinaryImage=BinaryImage;
  vu16 i,j;
#ifdef CONFIG_LVGL_USE_MALLOC
	vu16 *ChangePointArray_V=(vu16 *)malloc(240);
	vu16 *pChangePointArray_V=ChangePointArray_V;
//	memset((void*)ChangePointArray_V,0,240*2);
#else
	vu16 *ChangePointArray_V=(vu16 *)lv_malloc(240);
	vu16 *pChangePointArray_V=ChangePointArray_V;
//	lv_mem_set((void*)ChangePointArray_V,0,240*2);
#endif /*CONFIG_LVGL_USE_MALLOC*/
	if(ChangePointArray_V!=NULL)	printf("ChangePointArray_V内存分配成功\r\n");		
 	else printf("ChangePointArray_V内存分配失败\r\n");
	
  for(j=1;j<=240;j++)
	{
		TmpBinaryImage=BinaryImage+j-1;
    for(i=1;i<=320;i++)
		{
		  if(*TmpBinaryImage!=*(TmpBinaryImage+240)&&i!=320) (*pChangePointArray_V)++;
			TmpBinaryImage+=240;
		}	
    pChangePointArray_V++;		
	}	
	return ChangePointArray_V;
}

static __IO u16 *CalcChangePoint_H(vu8 *BinaryImage) //用于记录横向跳变点，共320个数据，ChangePointArray_H[y]其中y为图片纵坐标
{
	vu8 *TmpBinaryImage=BinaryImage;
  vu16 i,j;

#ifdef CONFIG_LVGL_USE_MALLOC
  vu16 *ChangePointArray_H=(vu16 *)malloc(320);
	vu16 *pChangePointArray_H=ChangePointArray_H;
//	memset((void*)ChangePointArray_H,0,320*2);
#else
  vu16 *ChangePointArray_H=(vu16 *)lv_malloc(320);
	vu16 *pChangePointArray_H=ChangePointArray_H;
//	lv_mem_set((void*)ChangePointArray_H,0,320*2);
#endif /*CONFIG_LVGL_USE_MALLOC*/
	if(ChangePointArray_H!=NULL)	printf("ChangePointArray_H内存分配成功\r\n");		
 	else printf("ChangePointArray_H内存分配失败\r\n");

  for(j=1;j<=320;j++)
	{
		*pChangePointArray_H=0;
    for(i=1;i<=240;i++)
		{
		  if(*TmpBinaryImage!=*(TmpBinaryImage+1)&&i!=240) (*pChangePointArray_H)++;
			TmpBinaryImage++;
		}	
    pChangePointArray_H++;		
	}	
	return ChangePointArray_H;
}	
/*
//---------根据横向跳变点数组及纵向跳变点数组，分割字符,字符的各个界限保存在xArray及yArray
void CharacterSegmentation(vu8 *BinaryImage,vu16 *xArray,vu16 *yArray)//x和y数组每两个元素分别表示xleft,xright,yup,ylow
{
	vu8 xstatus=0,ystatus=0;
	vu8 *TmpBinaryImage=BinaryImage;
  vu16 xright,xleft,yup,ylow,i,j;
	vu16 *pChangePointArray_V=CalcChangePoint_V(TmpBinaryImage);
	vu16 *pChangePointArray_H=CalcChangePoint_H(BinaryImage);
	for(j=1;j<=240;j++)
  {
	  if(*pChangePointArray_V!=0&&xstatus==0) 
		{
			*xArray++=j;
			xstatus++;
		}
	 if(*pChangePointArray_V==0&&xstatus!=0) 
		{
			*xArray++=j-1;
			xstatus=0;
		}
		pChangePointArray_V++;
	}
	
  for(i=1;i<=320;i++)
  {
	  if(*pChangePointArray_H!=0&&ystatus==0) 
		{
			*yArray++=i;
			ystatus++;
		}
	 if(*pChangePointArray_H==0&&ystatus!=0) 
		{
			*yArray++=i-1;
			ystatus=0;
		}
		pChangePointArray_H++;
	}
}
*/
static __IO u16 *CharacterSegmentation_x(vu8 *BinaryImage)//x数组每两个元素分别表示xleft,xright
{
	vu8 xstatus=0;  
  vu16 j;
	vu16 *pChangePointArray_V=CalcChangePoint_V(BinaryImage),*pTmpChangePointArray_V=pChangePointArray_V;
#ifdef CONFIG_LVGL_USE_MALLOC
  vu16 *xArray=(vu16 *)malloc(240);
	vu16 *pxArray=xArray;
#else
  vu16 *xArray=(vu16 *)lv_malloc(240);
	vu16 *pxArray=xArray;
#endif /*CONFIG_LVGL_USE_MALLOC*/
	if(xArray!=NULL)	printf("xArray内存分配成功\r\n");		
 	else printf("xArray内存分配失败\r\n");

  for(j=1;j<=240;j++)
  {
	  if(*pChangePointArray_V!=0&&xstatus==0) 
		{
			*pxArray++=j;
			xstatus++;
		}
	 if(*pChangePointArray_V==0&&xstatus!=0) 
		{
			*pxArray++=j-1;
			xstatus=0;
		}
		pChangePointArray_V++;
	}
	*pxArray=0;  //作为静态数组xArray特征点,迭代时判断以作停止
#ifdef CONFIG_LVGL_USE_MALLOC
	free((void *)pTmpChangePointArray_V);
#else
	lv_free((void *)pTmpChangePointArray_V);
#endif /*CONFIG_LVGL_USE_MALLOC*/
  return xArray;
}
static __IO u16 *CharacterSegmentation_y(vu8 *BinaryImage)//y数组每两个元素分别表示yup,ylow
{
  vu8 ystatus=0;
  vu16 i;
	vu16 *pChangePointArray_H=CalcChangePoint_H(BinaryImage),*pTmpChangePointArray_H=pChangePointArray_H;
#ifdef CONFIG_LVGL_USE_MALLOC
  vu16 *yArray=(vu16 *)malloc(320);
	vu16 *pyArray=yArray;
#else
  vu16 *yArray=(vu16 *)lv_malloc(320);
	vu16 *pyArray=yArray;
#endif /*CONFIG_LVGL_USE_MALLOC*/
	if(yArray!=NULL)	printf("yArray内存分配成功\r\n");		
 	else printf("yArray内存分配失败\r\n");

  for(i=1;i<=320;i++)
  {
	  if(*pChangePointArray_H!=0&&ystatus==0) 
		{
			*pyArray++=i;
			ystatus++;
		}
	 if(*pChangePointArray_H==0&&ystatus!=0) 
		{
			*pyArray++=i-1;
			ystatus=0;
		}
		pChangePointArray_H++;
	}
	*pyArray=0;
#ifdef CONFIG_LVGL_USE_MALLOC
	free((void *)pTmpChangePointArray_H);
#else
	lv_free((void *)pTmpChangePointArray_H);
#endif /*CONFIG_LVGL_USE_MALLOC*/
  return yArray;
}

static __IO u8 *SaveCharacterImage(vu8 *BinaryImage)//CharacterImage用于存储字符像素，其中用0x0f隔开
{
	vu8 CharaterExistStatus=0;
	vu8 *TmpBinaryImage=BinaryImage;
	vu16 xleft,xright,yup,ylow,deltax,deltay,i,j,tmpxleft,tmpdeltax;
	vu16 *pCharacterSegmentation_x=CharacterSegmentation_x(TmpBinaryImage),*pTmpCharacterSegmentation_x=pCharacterSegmentation_x; 
	TmpBinaryImage=BinaryImage;
  vu16 *pCharacterSegmentation_y=CharacterSegmentation_y(TmpBinaryImage),*pTmpCharacterSegmentation_y=pCharacterSegmentation_y;
  u32 shifty,shiftx;
#ifdef CONFIG_LVGL_USE_MALLOC
	vu8 *CharacterImage=(vu8 *)malloc(240*320);
	vu8 *pCharacterImage=CharacterImage,*pCharacterImageTmp=pCharacterImage;
#else
	vu8 *CharacterImage=(vu8 *)lv_malloc(240*320);
	vu8 *pCharacterImage=CharacterImage,*pCharacterImageTmp=pCharacterImage;
#endif /*CONFIG_LVGL_USE_MALLOC*/
	if(CharacterImage!=NULL)	printf("CharacterImage内存分配成功\r\n");		
 	else printf("CharacterImage内存分配失败\r\n");

//	my_mem_set((void *)CharacterImage,0,240*320);
	while(*pCharacterSegmentation_y) //遍历yArray数组，每次读取一组yup,ylow
	{
		yup=*pCharacterSegmentation_y;
		ylow=*(pCharacterSegmentation_y++);
		deltay=ylow-yup;
		pCharacterSegmentation_y+=2;
		if(deltay<Charater_ymaxForNormalization) continue; //舍去过小字符
		tmpxleft=0;tmpdeltax=0;
	  while(*pCharacterSegmentation_x)//遍历xArray数组，每次读取一组xleft,xright
		{
		  xleft=*pCharacterSegmentation_x;
			xright=*(pCharacterSegmentation_x++);
			if(0!=tmpxleft)tmpdeltax=xleft-tmpxleft;
			deltax=xright-xleft+tmpdeltax;
			if(deltay>(deltax%deltay)/2||1>deltax/deltay)//宽大于高超过阈值，舍去
			{
				tmpxleft=0;
				tmpdeltax=0;
				continue;
			}
			pCharacterSegmentation_x+=2;
//			if(deltax<Charater_xmaxForNormalization) continue;//舍去过小字符
			if(deltax>(deltay%deltax)/2||1>deltay/deltax) //比例特殊，表示文字自带间隙,连接x轴完成完整文字
			{
				if(tmpxleft==0)tmpxleft=xleft;				
				continue;
			}
			if(0!=tmpxleft)xleft=tmpxleft;
			tmpxleft=0;tmpdeltax=0;
			shifty=(yup-1)*240;
			shiftx=xleft-1;
			TmpBinaryImage=BinaryImage+shifty+shiftx;  //定位读取指针到二值数组相应位置	
			pCharacterImageTmp=pCharacterImage;			
			for(j=1;j<=deltay;j++)
			{
				for(i=1;i<=deltax;i++)
				{
			    *pCharacterImage++=*TmpBinaryImage;
					if(*TmpBinaryImage!=*(TmpBinaryImage+1)&&i!=deltax&&CharaterExistStatus==0) CharaterExistStatus++;
					TmpBinaryImage++;
				}
				TmpBinaryImage+=240-deltax;              //定位读取指针到二值数组下一行的字符起始位置
			}
			if(CharaterExistStatus==0)pCharacterImage=pCharacterImageTmp; //字符数组为空，则覆盖
			else 
			{
				*pCharacterImage++=0x0f;                                //不为空则输入字符分割特征值
				*pCharacterImage++=(vu8)((deltax>>8)&0x00ff);            //记录字符像素x长度
				*pCharacterImage++=(vu8)(deltax&0x00ff);
				*pCharacterImage++=(vu8)((deltay>>8)&0x00ff);            //记录字符像素y长度
				*pCharacterImage++=(vu8)(deltay&0x00ff);
			}
			CharaterExistStatus=0;
		}
	}	
  *pCharacterImage=0xf0;       //输入数组结束特征值
#ifdef CONFIG_LVGL_USE_MALLOC
	free((void *)pTmpCharacterSegmentation_x);
	free((void *)pTmpCharacterSegmentation_y);
#else
	lv_free((void *)pTmpCharacterSegmentation_x);
	lv_free((void *)pTmpCharacterSegmentation_y);
#endif /*CONFIG_LVGL_USE_MALLOC*/
	printf("获取字符像素成功\r\n");
  return CharacterImage;
}

static __IO u8 *CharaterNormalized(vu8 *BinaryImage)
{
	vu8 modx,leavex,mody,leavey,leavemodx,leavemody,np;
	vu8 *pSaveCharacterImage=SaveCharacterImage(BinaryImage);
	vu16 CharacterImageDeltax=0,CharacterImageDeltay=0,i,j;
#ifdef CONFIG_LVGL_USE_MALLOC
  vu8 *CharacterMatrix=(vu8 *)malloc(32*CharacterNumToCapture);    //字模为16*16，即32*8位，最多存储20个字
	vu8 *TmpCharacterMatrixImage;//=(vu8 *)malloc(240*320);
  vu8 *pTmpSaveCharacterImage=pSaveCharacterImage,*pSaveCharacterImageForfree=pSaveCharacterImage,\
	    *pTmpCharacterMatrixImage=TmpCharacterMatrixImage,*pCharacterMatrix=CharacterMatrix;
#else
  vu8 *CharacterMatrix=(vu8 *)lv_malloc(32*CharacterNumToCapture);    //字模为16*16，即32*8位，最多存储20个字
	vu8 *TmpCharacterMatrixImage;//=(vu8 *)lv_malloc(240*320);
  vu8 *pTmpSaveCharacterImage=pSaveCharacterImage,*pSaveCharacterImageForfree=pSaveCharacterImage,\
	    *pTmpCharacterMatrixImage=TmpCharacterMatrixImage,*pCharacterMatrix=CharacterMatrix;
#endif /*CONFIG_LVGL_USE_MALLOC*/	
	if(CharacterMatrix!=NULL)	printf("CharacterMatrix内存分配成功\r\n");		
 	else printf("CharacterMatrix内存分配失败\r\n");
	
	if(TmpCharacterMatrixImage!=NULL)	printf("TmpCharacterMatrixImage内存分配成功\r\n");		
 	else printf("TmpCharacterMatrixImage内存分配失败\r\n");

//	printf("内存已使用: %d\r\n",my_mem_perused(SRAMEX));
//	my_mem_set((void *)CharacterMatrix,0,32*CharacterNumToCapture);
//	my_mem_set((void *)TmpCharacterMatrixImage,0,240*320);
	while(CharacterNumToCapture)
	{
	  pTmpSaveCharacterImage=pSaveCharacterImage; //临时存储字符像素数组字符开始指针
	  while(*pSaveCharacterImage++!=0x0f)           //遍历直到字符分割处
		{
			
		}
		pSaveCharacterImage++;
		CharacterImageDeltax=*pSaveCharacterImage; //读取字符像素的x长度
		CharacterImageDeltax<<=8;
		pSaveCharacterImage++;
		CharacterImageDeltax|=*pSaveCharacterImage;
		pSaveCharacterImage++;
		CharacterImageDeltay=*pSaveCharacterImage; //读取字符像素的y长度
		CharacterImageDeltay<<=8;
		pSaveCharacterImage++;
		CharacterImageDeltay|=*pSaveCharacterImage;
		
		pSaveCharacterImage=pTmpSaveCharacterImage;//返回字符像素数组字符开始指针
		//pTmpCharacterMatrixImage=TmpCharacterMatrixImage;//指向字符像素临时数组首元素
		
//		for(j=1;j<=CharacterImageDeltay;j++)         //进行水平腐蚀
//		{
//		  for(i=2;i<=CharacterImageDeltax-1;i++)
//			{
//			  if(i==2)
//				{					
//					*pTmpCharacterMatrixImage++=*pSaveCharacterImage;
//					pSaveCharacterImage++;
//				}
//        #ifdef Charater_DeepColor				
//				*pTmpCharacterMatrixImage=0;
//				for(np=1;np<=3;np++)
//				{
//				  if(128<*(pSaveCharacterImage+np-2))   //左本右其中一个像素为255，则对本像素赋值255
//					{
//					  *pTmpCharacterMatrixImage=255;
//						break;
//					}				
//				}
//        #endif /* Charater_DeepColor */
//			  #ifdef Charater_LightColor				
//				*pTmpCharacterMatrixImage=255;
//				for(np=1;np<=3;np++)
//				{
//				  if(128>*(pSaveCharacterImage+np-2))
//					{
//					  *pTmpCharacterMatrixImage=0;
//						break;
//					}				
//				}
//        #endif /* Charater_LightColor */
//		    pTmpCharacterMatrixImage++;
//			  pSaveCharacterImage++;
//			  if(i==CharacterImageDeltax-1)
//			  {					
//			  	*pTmpCharacterMatrixImage++=*pSaveCharacterImage;
//				  pSaveCharacterImage++;
//			  }				
//			}		
//		}
		//printf("水平腐蚀完成\r\n");
		
		if(*pSaveCharacterImage==0xf0) break;      //判断是否完成全部的字符读取

		modx=CharacterImageDeltax/Charater_xmaxForNormalization;
		leavex=CharacterImageDeltax%Charater_xmaxForNormalization;
		mody=CharacterImageDeltay/Charater_ymaxForNormalization;
		leavey=CharacterImageDeltay%Charater_ymaxForNormalization;
		if(leavex!=0) leavemodx=Charater_xmaxForNormalization/leavex;
		if(leavey!=0) leavemody=Charater_ymaxForNormalization/leavey;
		
		pTmpCharacterMatrixImage=pSaveCharacterImage;//TmpCharacterMatrixImage;//定位指针于临时字符像素数组首地址
		pCharacterMatrix--;                  //定位于字符位数组上一个字符的尾地址
		np=0;
		for(j=1;j<=CharacterImageDeltay;j+=mody)       //归一化16*16并以8位存储在CharacterMatrix数组，每位表示一个二值
		{			
			for(i=1;i<=CharacterImageDeltax;i+=modx)
			{
				if(np==0)
				{
				  np=8;
					pCharacterMatrix++;           //预留写入一个字节（8位）的像素个数，使地址指向要写入的字节
	      }
				#ifdef Charater_DeepColor
				if(*pTmpCharacterMatrixImage==255) *pCharacterMatrix<<=1;
				else
				{
					*pCharacterMatrix|=0x01;
				  *pCharacterMatrix<<=1;
				}
				#endif /* Charater_DeepColor */
				#ifdef Charater_LightColor
				if(*pTmpCharacterMatrixImage==0) *pCharacterMatrix<<=1;
				else
				{
					*pCharacterMatrix|=0x01;
				  *pCharacterMatrix<<=1;
				}
				#endif /* Charater_LightColor */
				np--;
			  pTmpCharacterMatrixImage+=modx; //根据x轴缩小倍数，跳过相应数据
				if(leavex!=0)
				{
					if((i+1)%leavemodx==0&&i!=1)
					{
						pTmpCharacterMatrixImage++;
						i--;
						leavex--;
					}
				}
			}
			pTmpCharacterMatrixImage+=(mody-1)*CharacterImageDeltax;//根据y轴缩小倍数，跳过相应数据
			if(leavey!=0)
			{
				if((j+1)%leavemody==0&&j!=1)
				{
					pTmpCharacterMatrixImage+=CharacterImageDeltax;
					j--;
					leavey--;
				}
			}
	  }
	}
#ifdef CONFIG_LVGL_USE_MALLOC
//	free((void *)TmpCharacterMatrixImage);
	free((void *)pTmpSaveCharacterImage);
	free((void *)pSaveCharacterImageForfree);
#else
//	lv_free((void *)TmpCharacterMatrixImage);
	lv_free((void *)pTmpSaveCharacterImage);
	lv_free((void *)pSaveCharacterImageForfree);
#endif /*CONFIG_LVGL_USE_MALLOC*/
	printf("归一化成功\r\n");
	return CharacterMatrix;
}
/*  static vu8 CharacterMatrix[32*CharacterNumToCapture]={0};    //字模为16*16，即32*8位，最多存储20个字 */
	
void CharacterCompareFromExflash(vu8 *BinaryImage,vu16 *Character_GB2312) //存储读取到的字符的编码
{
	vu8 i,j,chsum,chsumEx,coutCharacterMatchBuf=0;	   
	vu8 CharacterExflash[32];//外部flash读数据缓冲区
	vu8 GridPerByte[32],GridPerByteEx[32]; 
	vu8 *pCharacterExflash=CharacterExflash,*pGridPerByte=GridPerByte,*pGridPerByteEx=GridPerByteEx;
  vu8 *pCharacterMatrix=CharaterNormalized(BinaryImage),*pCharacterMatrixTmp=pCharacterMatrix,*pCharacterMatrixForFree=pCharacterMatrix;
  vu16 ConfindeceFlag,encode;	
	u32 sum,wordAddr,tmpwordAddr,wordToCompareFromExflash=WordCompareFromExflash;
	wordAddr=(0xb0-0x81)*190; //定位比对字模所存储的外部flash位置，从0xb0a1（啊）开始
	wordAddr+=0xa1-0x41;
	wordAddr*=32;
	wordAddr+=FLASH_12CHAR_ADDR;
	tmpwordAddr=wordAddr;
#ifdef CONFIG_LVGL_USE_MALLOC
	vu16 *Confidence=(vu16 *)malloc(CharacterMatchBuf);  //记录外部flash粗比对缓冲区字模置信度，其标志数和缓冲区一致，不能打乱
	vu16 *TmpCharacter_GB2312=Character_GB2312,*pConfidence=Confidence;
	u32 *CharacterMatch=(u32 *)malloc(CharacterMatchBuf);;  //存储从外部flash读取的粗比对成功的字模地址
	u32 *pCharacterMatch=CharacterMatch;
#else
	vu16 *Confidence=(vu16 *)lv_malloc(CharacterMatchBuf);  //记录外部flash粗比对缓冲区字模置信度，其标志数和缓冲区一致，不能打乱
	vu16 *TmpCharacter_GB2312=Character_GB2312,*pConfidence=Confidence;
	u32 *CharacterMatch=(u32 *)lv_malloc(CharacterMatchBuf);;  //存储从外部flash读取的粗比对成功的字模地址
	u32 *pCharacterMatch=CharacterMatch;
#endif /*CONFIG_LVGL_USE_MALLOC*/
if(Confidence!=NULL)	printf("Confidence内存分配成功\r\n");		
 	else printf("Confidence内存分配失败\r\n");

	if(CharacterMatch!=NULL)	printf("CharacterMatch内存分配成功\r\n");		
 	else printf("CharacterMatch内存分配失败\r\n");


	for(j=1;j<=CharacterNumToCapture;j++)
	{
		pCharacterMatrixTmp=pCharacterMatrix;   //指向字模位数组
		chsum=calcCharacterBit(pCharacterMatrixTmp); //计算字模位数量
	  if(chsum==0) break; //空字符表示结束，退出循环
		
		wordAddr=tmpwordAddr; //外部flash地址指向初始位置
		while(wordToCompareFromExflash--)        //遍历字库，进行字模位数量比对，快速筛选
		{
			pCharacterExflash=CharacterExflash;
			EN25QXX_Read(pCharacterExflash,wordAddr,32); 
			
			pCharacterExflash=CharacterExflash;
			chsumEx=calcCharacterBit(pCharacterExflash);
			
			if(CharacterErrorThreshold<chsum-chsumEx||CharacterErrorThreshold<chsumEx-chsum) //比较字模与字库中的字模，差别过大者舍弃
			{		
				wordAddr+=32;
				continue;
			}
			else
			{ 
				*pCharacterMatch++=wordAddr;	//记录比对成功的字模的地址
				wordAddr+=32;
				coutCharacterMatchBuf++;
				if(coutCharacterMatchBuf==CharacterMatchBuf)//判断预筛选缓冲区是否已满，满则跳出循环
				{				
					coutCharacterMatchBuf=0;
					break;  
				}				
				//if(*pCharacterMatch=='\0') break;      //判断预筛选缓冲区是否已满，满则跳出循环
			}
		}
		
		pCharacterMatrixTmp=pCharacterMatrix;
		
		calcGridBit(pCharacterMatrixTmp,pGridPerByte);       //用32个网格分割现字模，每个网格内的字模位数保存
	
		pCharacterMatch=CharacterMatch;
		pConfidence=Confidence;
    for(coutCharacterMatchBuf=0;coutCharacterMatchBuf<CharacterMatchBuf;coutCharacterMatchBuf++)//现字模比对预筛选缓冲区内的字模
    {				
      pCharacterExflash=CharacterExflash;			
			EN25QXX_Read(pCharacterExflash,*pCharacterMatch,32); 
			
			pCharacterExflash=CharacterExflash;			
			calcGridBit(pCharacterExflash,pGridPerByteEx);
			
      pGridPerByte=GridPerByte;
			pGridPerByteEx=GridPerByteEx;
			
			sum=0;
			for(i=1;i<=32;i++)
			{
			  if(*pGridPerByte>=*pGridPerByteEx) sum+=(*pGridPerByte-*pGridPerByteEx)*(*pGridPerByte-*pGridPerByteEx);
				else 	sum+=(*pGridPerByteEx-*pGridPerByte)*(*pGridPerByteEx-*pGridPerByte);
				pGridPerByte++;
				pGridPerByteEx++;
			}
			sum=sum/32;
			sum=~sum&0xffff;
			*pConfidence++=(vu16)sum;
			
			pCharacterMatch++;
		}
//		pCharacterMatch=CharacterMatch;
		pConfidence=Confidence;
		ConfindeceFlag=FindMaxNumFromArray(pConfidence,32);
		
		wordAddr=*(CharacterMatch+ConfindeceFlag); //外部flash地址指向对比置信度最高的字模地址
		
		wordAddr-=FLASH_12CHAR_ADDR;   //解析为GB2312编码
		wordAddr/=32;
		encode=wordAddr/190;
		encode+=0x81; //得到高位字节
		wordAddr%=190;
    wordAddr+=0x41;//得到低位字节
		if(wordAddr<0x7f) wordAddr--; //空位之前只需+0x40
		encode<<=8;
		encode|=wordAddr&0xff;
		*TmpCharacter_GB2312++=encode;//记录字符编码
		
		pCharacterMatrix+=32;
	}
#ifdef CONFIG_LVGL_USE_MALLOC
  free((void *)pCharacterMatrixForFree);
	free((void *)Confidence);
  free((void *)CharacterMatch);
#else
  lv_free((void *)pCharacterMatrixForFree);
	lv_free((void *)Confidence);
  lv_free((void *)CharacterMatch);
#endif /*CONFIG_LVGL_USE_MALLOC*/
	printf("识别完成\r\n");
}
