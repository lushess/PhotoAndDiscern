#ifndef _PICTURE_TOOL_H
#define _PICTURE_TOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

#define myAssertCalled(char,int) printf("Error:%s,%d\r\n",char,int)
#define myASSERT(x) if((x)==0) myAssertCalled(__FILE__,__LINE__)


#define GrayTORGB565(Gray)    (vu16)/*(((Gray>>3)<<11)|((Gray>>2)<<5)|(Gray>>3))*/((Gray>>3)|((Gray&~3)<<3)|((Gray&~7)<<8))

#define RGB24ToRGB16(r,g,b)   (vu16)(((r&0x1f)<<11)|((g&0x3f)<<5)|(b&0x1f))
#define R8FromRGB565(RGB565)  (vu8)((RGB565>>11)*255/31)
#define G8FromRGB565(RGB565)  (vu8)(((RGB565>>5)&0x3f)*255/63)
#define B8FromRGB565(RGB565)  (vu8)((RGB565&0x1f)*255/31)

#define hsv24Tohsv16(h,s,v)   (vu16)(((h&0x7f)<<9)|((s&0x0f)<<5)|(v&0x1f))
#define H8FromHSV745(HSV745)  (vu8)((HSV745>>9)*255/127)
#define S8FromHSV745(HSV745)  (vu8)(((HSV745>>5)&0x0f)*255/15)
#define V8FromHSV745(HSV745)  (vu8)((HSV745&0x1f)*255/31)

/*****      �ڲ�����       *****/
static u16 ArrayAddup(vu8 *Array,u8 Num);
static void fastsort(vu8 *arrayhead,vu8 *arrayend); //��������
static u16 RGB565ToHSV745(vu16 RGB565); //RGB565תHSV745
static __IO float *CalcGrayHist_Chance(vu8 *GrayImage); //����Ҷ�ֱ��ͼ���Ը��ʷֲ�����ʽ����
static u8 CalcThresold_Average(vu8 *GrayImage);  //ƽ���Ҷ�ֵ����ֵ
static u8 CalcThresold_MultiAverage(vu8 *GrayImage); //������ֵ��
static u8 CalcThresold_SimpleTwoPeak_ByGrayHist(vu8 *GrayImage);//�򵥵�˫�巨ȡ��ֵ�����Ȳ�
static __IO u8 *GrayGradient_Diffrential(vu8 *GrayImage,u8 GradientThreshold);//��������洢��ÿ�����ص��ݶȣ���ֵ���ϵ��ݶ���Ϊ255������˫���ִ���˫��΢��
static __IO u8 *GrayGradient_Roberts(vu8 *GrayImage,u8 GradientThreshold); //Roberts�������ݶ�
static __IO u8 *GrayGradient_Sobel(vu8 *GrayImage,u8 GradientThreshold); //Sobel�������ݶ�

static void Corrode(vu8 *Image);                      //��ʴ
static void Expand(vu8 *Image);                       //����


/*****      �ⲿ����       *****/
void CapLCDToGray(vu8 *GrayImage);                    //��ȡLCD�������洢��SRAM
void GrayEven(vu8 *GrayImage);                        //�Ҷ�ͼ���⻯
void GrayChange_Linear(vu8 *GrayImage,float modulus,int8_t trans);//���Ըı�Ҷ�ͼ�������صĻҶ�ֵ
void OptionalFilter(vu8 *Image);                      //����Ӧѡ����ƽ���˲���������9����ͬ�������ӣ�����Ч��������
void MedianFilter(vu8 *Image);                        //��ֵ�˲�
void CloseOperation(vu8 *Image);                      //������
void OpenOperation(vu8 *Image);                       //������
void GrayToBinary(vu8 *GrayImage);                    //�Ҷ�ͼת��Ϊ��ֵͼ   
void ShowGrayToLCD(vu8 *GrayImage);                   //�ѻҶ�ͼ��ӡ��LCD��
void ShowBinaryToLCD(vu8 *BinaryImage);               //�Ѷ�ֵͼ��ӡ��LCD��
void ImageAjust_Horizontal(vu8 *ImageToAjust,volatile int8_t *offset);//��бУ��������ÿ��ƫ��������,*offset С��0��ʾ���ƣ�����0��ʾ����
__IO u16 *Projective_H(vu8 *BinaryImage);             //����ͶӰ��x��
__IO u16 *Projective_V(vu8 *BinaryImage);             //����ͶӰ��y��
__IO u8 *GrayGradientToMask(vu8 *Gradient);           //�����ݶ�ͼ������ģ�����ڲ���Ϊ255���������Ϊ������ݶ�ͼ����
void BinaryToEdge(vu8 *BinaryImage);                  //�Ϳ��ڲ��㷨ȡ��ǰ������
__IO u8 *ObjectMeasure_FromBinary(vu8 *BinaryImage,vu32 *ObjectSize);//��Ǹ������壬�����أ����ֵ��1��ʼ��ObjectSize�����¼ÿ���������ռ�����ش�С


#ifdef __cplusplus
}
#endif /*extern "C"*/

#endif /* _PICTURE_TOOL_H */
