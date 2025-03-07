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
static void CalcChangePoint_V(vu16 *ChangePointArray_V,vu8 *BinaryImage) //���ڼ�¼��������㣬��240�����ݣ�ChangePointArray_V[x]����xΪͼƬ������
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

static void CalcChangePoint_H(vu16 *ChangePointArray_H,vu8 *BinaryImage) //���ڼ�¼��������㣬��320�����ݣ�ChangePointArray_H[y]����yΪͼƬ������
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
//---------���ݺ�����������鼰������������飬�ָ��ַ�,�ַ��ĸ������ޱ�����xArray��yArray
void CharacterSegmentation(vu8 *BinaryImage,vu16 *xArray,vu16 *yArray)//x��y����ÿ����Ԫ�طֱ��ʾxleft,xright,yup,ylow
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
static void CharacterSegmentation_x(vu16 *xArray,vu8 *BinaryImage)//x����ÿ����Ԫ�طֱ��ʾxleft,xright
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
	*pxArray=0;  //��Ϊ��̬����xArray������,����ʱ�ж�����ֹͣ	
}
static void CharacterSegmentation_y(vu16 *yArray,vu8 *BinaryImage)//y����ÿ����Ԫ�طֱ��ʾyup,ylow
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

static void SaveCharacterImage(vu8 *CharacterImage,vu8 *BinaryImage)//CharacterImage���ڴ洢�ַ����أ�������0x0f����
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

	while(*pCharacterSegmentation_y) //����yArray���飬ÿ�ζ�ȡһ��yup,ylow
	{
		yup=*pCharacterSegmentation_y;
		ylow=*(pCharacterSegmentation_y++);
		deltay=ylow-yup;
		pCharacterSegmentation_y+=2;
		if(deltay<Charater_ymaxForNormalization) continue; //��ȥ��С�ַ�
		tmpxleft=0;tmpdeltax=0;
	  while(*pCharacterSegmentation_x)//����xArray���飬ÿ�ζ�ȡһ��xleft,xright
		{
		  xleft=*pCharacterSegmentation_x;
			xright=*(pCharacterSegmentation_x++);
			if(0!=tmpxleft)tmpdeltax=xleft-tmpxleft;
			deltax=xright-xleft+tmpdeltax;
			if(deltay>(deltax%deltay)/2||1>deltax/deltay)//����ڸ߳�����ֵ����ȥ
			{
				tmpxleft=0;
				tmpdeltax=0;
				continue;
			}
			pCharacterSegmentation_x+=2;
//			if(deltax<Charater_xmaxForNormalization) continue;//��ȥ��С�ַ�
			if(deltax>(deltay%deltax)/2||1>deltay/deltax) //�������⣬��ʾ�����Դ���϶,����x�������������
			{
				if(tmpxleft==0)tmpxleft=xleft;				
				continue;
			}
			if(0!=tmpxleft)xleft=tmpxleft;
			tmpxleft=0;tmpdeltax=0;
			shifty=(yup-1)*240;
			shiftx=xleft-1;
			TmpBinaryImage=BinaryImage+shifty+shiftx;  //��λ��ȡָ�뵽��ֵ������Ӧλ��	
			pCharacterImageTmp=pCharacterImage;			
			for(j=1;j<=deltay;j++)
			{
				for(i=1;i<=deltax;i++)
				{
			    *pCharacterImage++=*TmpBinaryImage;
					if(*TmpBinaryImage!=*(TmpBinaryImage+1)&&i!=deltax&&CharaterExistStatus==0) CharaterExistStatus++;
					TmpBinaryImage++;
				}
				TmpBinaryImage+=240-deltax;              //��λ��ȡָ�뵽��ֵ������һ�е��ַ���ʼλ��
			}
			if(CharaterExistStatus==0)pCharacterImage=pCharacterImageTmp; //�ַ�����Ϊ�գ��򸲸�
			else 
			{
				*pCharacterImage++=0x0f;                                //��Ϊ���������ַ��ָ�����ֵ
				*pCharacterImage++=(vu8)((deltax>>8)&0x00ff);            //��¼�ַ�����x����
				*pCharacterImage++=(vu8)(deltax&0x00ff);
				*pCharacterImage++=(vu8)((deltay>>8)&0x00ff);            //��¼�ַ�����y����
				*pCharacterImage++=(vu8)(deltay&0x00ff);
			}
			CharaterExistStatus=0;
		}
	}	
  *pCharacterImage=0xf0;       //���������������ֵ

	UDEBUG("��ȡ�ַ����سɹ�\r\n");
}

static void CharaterNormalized(vu8 *CharacterMatrix,vu8 *BinaryImage)
{
	vu8 modx,leavex,mody,leavey,leavemodx,leavemody,np;
	vu16 CharacterImageDeltax=0,CharacterImageDeltay=0,i,j;
	vu8 *pCharacterMatrix=CharacterMatrix;
  
	vu8 *TmpCharacterMatrixImage=(vu8 *)Memalloc(240*320*sizeof(vu8));
	vu8 *pTmpCharacterMatrixImage=TmpCharacterMatrixImage;
	if(TmpCharacterMatrixImage!=NULL)	UDEBUG("TmpCharacterMatrixImage�ڴ����ɹ�,���ַΪ:%p\r\n",TmpCharacterMatrixImage);		
 	else UDEBUG("TmpCharacterMatrixImage�ڴ����ʧ��\r\n");

	vu8 *CharacterImage=(vu8 *)Memalloc(240*320*sizeof(vu8));
	if(CharacterImage!=NULL)	UDEBUG("CharacterImage�ڴ����ɹ�,���ַΪ:%p\r\n",CharacterImage);		
 	else UDEBUG("CharacterImage�ڴ����ʧ��\r\n");
	
	vu8 *pSaveCharacterImage=CharacterImage;
	vu8 *pTmpSaveCharacterImage=CharacterImage;
	vu8 *pBinaryImage = BinaryImage;
	SaveCharacterImage(pSaveCharacterImage,pBinaryImage);

	
	while(CharacterNumToCapture)
	{
	  pTmpSaveCharacterImage=pSaveCharacterImage; //��ʱ�洢�ַ����������ַ���ʼָ��
	  while(*pSaveCharacterImage++!=0x0f)           //����ֱ���ַ��ָ
		{
			
		}
		pSaveCharacterImage++;
		CharacterImageDeltax=*pSaveCharacterImage; //��ȡ�ַ����ص�x����
		CharacterImageDeltax<<=8;
		pSaveCharacterImage++;
		CharacterImageDeltax|=*pSaveCharacterImage;
		pSaveCharacterImage++;
		CharacterImageDeltay=*pSaveCharacterImage; //��ȡ�ַ����ص�y����
		CharacterImageDeltay<<=8;
		pSaveCharacterImage++;
		CharacterImageDeltay|=*pSaveCharacterImage;
		
		pSaveCharacterImage=pTmpSaveCharacterImage;//�����ַ����������ַ���ʼָ��
		//pTmpCharacterMatrixImage=TmpCharacterMatrixImage;//ָ���ַ�������ʱ������Ԫ��
		
//		for(j=1;j<=CharacterImageDeltay;j++)         //����ˮƽ��ʴ
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
//				  if(128<*(pSaveCharacterImage+np-2))   //��������һ������Ϊ255����Ա����ظ�ֵ255
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
		//UDEBUG("ˮƽ��ʴ���\r\n");
		
		if(*pSaveCharacterImage==0xf0) break;      //�ж��Ƿ����ȫ�����ַ���ȡ

		modx=CharacterImageDeltax/Charater_xmaxForNormalization;
		leavex=CharacterImageDeltax%Charater_xmaxForNormalization;
		mody=CharacterImageDeltay/Charater_ymaxForNormalization;
		leavey=CharacterImageDeltay%Charater_ymaxForNormalization;
		if(leavex!=0) leavemodx=Charater_xmaxForNormalization/leavex;
		if(leavey!=0) leavemody=Charater_ymaxForNormalization/leavey;
		
		pTmpCharacterMatrixImage=pSaveCharacterImage;//TmpCharacterMatrixImage;//��λָ������ʱ�ַ����������׵�ַ
		pCharacterMatrix--;                  //��λ���ַ�λ������һ���ַ���β��ַ
		np=0;
		for(j=1;j<=CharacterImageDeltay;j+=mody)       //��һ��16*16����8λ�洢��CharacterMatrix���飬ÿλ��ʾһ����ֵ
		{			
			for(i=1;i<=CharacterImageDeltax;i+=modx)
			{
				if(np==0)
				{
				  np=8;
					pCharacterMatrix++;           //Ԥ��д��һ���ֽڣ�8λ�������ظ�����ʹ��ַָ��Ҫд����ֽ�
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
			  pTmpCharacterMatrixImage+=modx; //����x����С������������Ӧ����
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
			pTmpCharacterMatrixImage+=(mody-1)*CharacterImageDeltax;//����y����С������������Ӧ����
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

	UDEBUG("TmpCharacterMatrixImage�ڴ����ͷ�,���ַΪ:%p\r\n",TmpCharacterMatrixImage);
	Memfree((void *)TmpCharacterMatrixImage);
	UDEBUG("CharacterImage�ڴ����ͷ�,���ַΪ:%p\r\n",CharacterImage);
	Memfree((void *)CharacterImage);

	UDEBUG("��һ���ɹ�\r\n");
}
/*  static vu8 CharacterMatrix[32*CharacterNumToCapture]={0};    //��ģΪ16*16����32*8λ�����洢20���� */
	
void CharacterCompareFromExflash(vu8 *BinaryImage,vu16 *Character_GB2312) //�洢��ȡ�����ַ��ı���
{
	vu8 i,j,chsum,chsumEx,coutCharacterMatchBuf=0;	   
	vu8 CharacterExflash[32];//�ⲿflash�����ݻ�����
	vu8 GridPerByte[32],GridPerByteEx[32]; 
	vu8 *pCharacterExflash=CharacterExflash,*pGridPerByte=GridPerByte,*pGridPerByteEx=GridPerByteEx;
  vu16 ConfindeceFlag,encode;	
	u32 sum,wordAddr,tmpwordAddr,wordToCompareFromExflash=WordCompareFromExflash;
	wordAddr=(0xb0-0x81)*190; //��λ�ȶ���ģ���洢���ⲿflashλ�ã���0xb0a1��������ʼ
	wordAddr+=0xa1-0x41;
	wordAddr*=32;
	wordAddr+=FLASH_12CHAR_ADDR;
	tmpwordAddr=wordAddr;

	vu16 Confidence[CharacterMatchBuf];  //��¼�ⲿflash�ֱȶԻ�������ģ���Ŷȣ����־���ͻ�����һ�£����ܴ���
	vu16 *TmpCharacter_GB2312=Character_GB2312,*pConfidence=Confidence;

	u32 CharacterMatch[CharacterMatchBuf];;  //�洢���ⲿflash��ȡ�ĴֱȶԳɹ�����ģ��ַ
	u32 *pCharacterMatch=CharacterMatch;

	vu8 CharacterMatrix[32*CharacterNumToCapture];    //��ģΪ16*16����32*8λ�����洢20����
	vu8 *pCharacterMatrix=CharacterMatrix,*pCharacterMatrixTmp=pCharacterMatrix,*pBinaryImage = BinaryImage;
	CharaterNormalized(pCharacterMatrix,pBinaryImage);
	
	UDEBUG("��ʼ�����ֿ�\r\n");
	for(j=1;j<=CharacterNumToCapture;j++)
	{
		pCharacterMatrixTmp=pCharacterMatrix;   //ָ����ģλ����
		chsum=calcCharacterBit(pCharacterMatrixTmp); //������ģλ����
	  if(chsum==0) break; //���ַ���ʾ�������˳�ѭ��
		
		wordAddr=tmpwordAddr; //�ⲿflash��ַָ���ʼλ��
		while(wordToCompareFromExflash--)        //�����ֿ⣬������ģλ�����ȶԣ�����ɸѡ
		{
			pCharacterExflash=CharacterExflash;
			EN25QXX_Read(pCharacterExflash,wordAddr,32); 
			
			pCharacterExflash=CharacterExflash;
			chsumEx=calcCharacterBit(pCharacterExflash);
			
			if(CharacterErrorThreshold<chsum-chsumEx||CharacterErrorThreshold<chsumEx-chsum) //�Ƚ���ģ���ֿ��е���ģ��������������
			{		
				wordAddr+=32;
				continue;
			}
			else
			{ 
				*pCharacterMatch++=wordAddr;	//��¼�ȶԳɹ�����ģ�ĵ�ַ
				wordAddr+=32;
				coutCharacterMatchBuf++;
				if(coutCharacterMatchBuf==CharacterMatchBuf)//�ж�Ԥɸѡ�������Ƿ���������������ѭ��
				{				
					coutCharacterMatchBuf=0;
					break;  
				}				
				//if(*pCharacterMatch=='\0') break;      //�ж�Ԥɸѡ�������Ƿ���������������ѭ��
			}
		}
		
		pCharacterMatrixTmp=pCharacterMatrix;
		
		calcGridBit(pCharacterMatrixTmp,pGridPerByte);       //��32������ָ�����ģ��ÿ�������ڵ���ģλ������
	
		pCharacterMatch=CharacterMatch;
		pConfidence=Confidence;
		UDEBUG("��ʼ�ȶ���ģ\r\n");
    for(coutCharacterMatchBuf=0;coutCharacterMatchBuf<CharacterMatchBuf;coutCharacterMatchBuf++)//����ģ�ȶ�Ԥɸѡ�������ڵ���ģ
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
		
		wordAddr=CharacterMatch[ConfindeceFlag]; //�ⲿflash��ַָ��Ա����Ŷ���ߵ���ģ��ַ
		UDEBUG("CharacterMatch+ConfindeceFlag��ַΪ:%p\r\n",&CharacterMatch[ConfindeceFlag]);
		UDEBUG("������:%d\r\n",wordAddr);
		
		wordAddr-=FLASH_12CHAR_ADDR;   //����ΪGB2312����
		wordAddr/=32;
		encode=wordAddr/190;
		encode+=0x81; //�õ���λ�ֽ�
		wordAddr%=190;
    wordAddr+=0x41;//�õ���λ�ֽ�
		if(wordAddr<0x7f) wordAddr--; //��λ֮ǰֻ��+0x40
		encode<<=8;
		encode|=wordAddr&0xff;
		*TmpCharacter_GB2312++=encode;//��¼�ַ�����
		
		pCharacterMatrix+=32;
	}
	
	UDEBUG("ʶ�����\r\n");
}
