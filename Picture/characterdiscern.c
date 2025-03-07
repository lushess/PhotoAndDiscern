#include "characterdiscern.h"
#include "picture_tool.h"
#include "tftlcd.h"
#include "flash.h"
#include "Config/Config.h"
#include "usart.h"
#include <stdlib.h>
#include <string.h>

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
static u16 FindMaxNumFromArray(vu16 *array,vu16 len)
{
  vu16 i,temp=0,flag=0;
	vu16 *parray = array;
	for(i=0;i<len;i++)
	{		
	  if(temp<*parray) 
		{
			temp=*parray;
			flag=i;
		}
    parray++;		
	}
	return flag;
}
static void CalcChangePoint_V(vu16 *ChangePointArray_V,vu8 *BinaryImage) //用于记录纵向跳变点，共240个数据，ChangePointArray_V[x]其中x为图片横坐标
{
	vu8 *TmpBinaryImage=BinaryImage;
  vu16 i,j;
	vu16 *pChangePointArray_V=ChangePointArray_V;

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
}

static void CalcChangePoint_H(vu16 *ChangePointArray_H,vu8 *BinaryImage) //用于记录横向跳变点，共320个数据，ChangePointArray_H[y]其中y为图片纵坐标
{
	vu8 *TmpBinaryImage=BinaryImage;
  vu16 i,j;
	vu16 *pChangePointArray_H=ChangePointArray_H;

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
static void CharacterSegmentation_x(vu16 *xArray,vu8 *BinaryImage)//x数组每两个元素分别表示xleft,xright
{
	vu8 xstatus=0;  
  vu16 j;
	vu16 *pxArray=xArray;
	
	vu16 ChangePointArray_V[240];
	
	vu16 *pChangePointArray_V=ChangePointArray_V;
	CalcChangePoint_V(pChangePointArray_V,BinaryImage);

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
}
static void CharacterSegmentation_y(vu16 *yArray,vu8 *BinaryImage)//y数组每两个元素分别表示yup,ylow
{
  vu8 ystatus=0;
  vu16 i;
	vu16 *pyArray=yArray;
	
	vu16 ChangePointArray_H[320];
	
	vu16 *pChangePointArray_H = ChangePointArray_H;
	CalcChangePoint_H(ChangePointArray_H,BinaryImage);

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
}

static void SaveCharacterImage(vu8 *CharacterImage,vu8 *BinaryImage)//CharacterImage用于存储字符像素，其中用0x0f隔开
{
	vu8 CharaterExistStatus=0;
	vu8 *TmpBinaryImage=BinaryImage;
	vu8 *pCharacterImage=CharacterImage;
	vu8 *pCharacterImageTmp=CharacterImage;
	vu16 xleft,xright,yup,ylow,deltax,deltay,i,j,tmpxleft,tmpdeltax;
	u32 shifty,shiftx;
	
	vu16 xArray[240];
	vu16 *pCharacterSegmentation_x=xArray; 
	CharacterSegmentation_x(pCharacterSegmentation_x,TmpBinaryImage);
	
	vu16 yArray[320];
  vu16 *pCharacterSegmentation_y=yArray;
	CharacterSegmentation_y(pCharacterSegmentation_y,TmpBinaryImage);

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

	UDEBUG("获取字符像素成功\r\n");
}

static void CharaterNormalized(vu8 *CharacterMatrix,vu8 *BinaryImage)
{
	vu8 modx,leavex,mody,leavey,leavemodx,leavemody,np;
	vu16 CharacterImageDeltax=0,CharacterImageDeltay=0,i,j;
	vu8 *pCharacterMatrix=CharacterMatrix;
  
	vu8 *TmpCharacterMatrixImage=(vu8 *)Memalloc(240*320*sizeof(vu8));
	vu8 *pTmpCharacterMatrixImage=TmpCharacterMatrixImage;
	if(TmpCharacterMatrixImage!=NULL)	UDEBUG("TmpCharacterMatrixImage内存分配成功,其地址为:%p\r\n",TmpCharacterMatrixImage);		
 	else UDEBUG("TmpCharacterMatrixImage内存分配失败\r\n");

	vu8 *CharacterImage=(vu8 *)Memalloc(240*320*sizeof(vu8));
	if(CharacterImage!=NULL)	UDEBUG("CharacterImage内存分配成功,其地址为:%p\r\n",CharacterImage);		
 	else UDEBUG("CharacterImage内存分配失败\r\n");
	
	vu8 *pSaveCharacterImage=CharacterImage;
	vu8 *pTmpSaveCharacterImage=CharacterImage;
	vu8 *pBinaryImage = BinaryImage;
	SaveCharacterImage(pSaveCharacterImage,pBinaryImage);

	
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
		//UDEBUG("水平腐蚀完成\r\n");
		
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

	UDEBUG("TmpCharacterMatrixImage内存已释放,其地址为:%p\r\n",TmpCharacterMatrixImage);
	Memfree((void *)TmpCharacterMatrixImage);
	UDEBUG("CharacterImage内存已释放,其地址为:%p\r\n",CharacterImage);
	Memfree((void *)CharacterImage);

	UDEBUG("归一化成功\r\n");
}
/*  static vu8 CharacterMatrix[32*CharacterNumToCapture]={0};    //字模为16*16，即32*8位，最多存储20个字 */
	
void CharacterCompareFromExflash(vu8 *BinaryImage,vu16 *Character_GB2312) //存储读取到的字符的编码
{
	vu8 i,j,chsum,chsumEx,coutCharacterMatchBuf=0;	   
	vu8 CharacterExflash[32];//外部flash读数据缓冲区
	vu8 GridPerByte[32],GridPerByteEx[32]; 
	vu8 *pCharacterExflash=CharacterExflash,*pGridPerByte=GridPerByte,*pGridPerByteEx=GridPerByteEx;
  vu16 ConfindeceFlag,encode;	
	u32 sum,wordAddr,tmpwordAddr,wordToCompareFromExflash=WordCompareFromExflash;
	wordAddr=(0xb0-0x81)*190; //定位比对字模所存储的外部flash位置，从0xb0a1（啊）开始
	wordAddr+=0xa1-0x41;
	wordAddr*=32;
	wordAddr+=FLASH_12CHAR_ADDR;
	tmpwordAddr=wordAddr;

	vu16 Confidence[CharacterMatchBuf];  //记录外部flash粗比对缓冲区字模置信度，其标志数和缓冲区一致，不能打乱
	vu16 *TmpCharacter_GB2312=Character_GB2312,*pConfidence=Confidence;

	u32 CharacterMatch[CharacterMatchBuf];;  //存储从外部flash读取的粗比对成功的字模地址
	u32 *pCharacterMatch=CharacterMatch;

	vu8 CharacterMatrix[32*CharacterNumToCapture];    //字模为16*16，即32*8位，最多存储20个字
	vu8 *pCharacterMatrix=CharacterMatrix,*pCharacterMatrixTmp=pCharacterMatrix,*pBinaryImage = BinaryImage;
	CharaterNormalized(pCharacterMatrix,pBinaryImage);
	
	UDEBUG("开始遍历字库\r\n");
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
		UDEBUG("开始比对字模\r\n");
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
		ConfindeceFlag=FindMaxNumFromArray(pConfidence,CharacterMatchBuf);
		
		wordAddr=CharacterMatch[ConfindeceFlag]; //外部flash地址指向对比置信度最高的字模地址
		UDEBUG("CharacterMatch+ConfindeceFlag地址为:%p\r\n",&CharacterMatch[ConfindeceFlag]);
		UDEBUG("其内容:%d\r\n",wordAddr);
		
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
	
	UDEBUG("识别完成\r\n");
}
