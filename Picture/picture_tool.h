#ifndef _PICTURE_TOOL_H
#define _PICTURE_TOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "Config/Config.h"
#include "picturememorymanager.h"

//#define myAssertCalled(char,int) printf("Error:%s,%d\r\n",char,int)
//#define myASSERT(x) if((x)==0) myAssertCalled(__FILE__,__LINE__)


#define GrayTORGB565(Gray)    (vu16)/*(((Gray>>3)<<11)|((Gray>>2)<<5)|(Gray>>3))*/((Gray>>3)|((Gray&~3)<<3)|((Gray&~7)<<8))

#define RGB24ToRGB16(r,g,b)   (vu16)(((r&0x1f)<<11)|((g&0x3f)<<5)|(b&0x1f))
#define R8FromRGB565(RGB565)  (vu8)((RGB565>>11)*255/31)
#define G8FromRGB565(RGB565)  (vu8)(((RGB565>>5)&0x3f)*255/63)
#define B8FromRGB565(RGB565)  (vu8)((RGB565&0x1f)*255/31)

#define hsv24Tohsv16(h,s,v)   (vu16)(((h&0x7f)<<9)|((s&0x0f)<<5)|(v&0x1f))
#define H8FromHSV745(HSV745)  (vu8)((HSV745>>9)*255/127)
#define S8FromHSV745(HSV745)  (vu8)(((HSV745>>5)&0x0f)*255/15)
#define V8FromHSV745(HSV745)  (vu8)((HSV745&0x1f)*255/31)

#define imagePixel_H		240
#define imagePixel_V		320


#ifdef USE_EXRAM_IMAGE_BUFFER
#define imageBuffer_Num		3		//�������ⲿSRAM���ܴ�С������Ϊ3
extern volatile uint8_t imageBuffer[imageBuffer_Num][imagePixel_H*imagePixel_V];
#endif //USE_EXRAM_IMAGE_BUFFER

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

/*****      �ڲ�����       *****/
static u16 ArrayAddup(vu8 *Array,u8 Num);
static void fastsort(vu8 *arrayhead,vu8 *arrayend); //��������
static u16 RGB565ToHSV745(vu16 RGB565); //RGB565תHSV745
static void CalcGrayHist_Chance(volatile float *histchance,vu8 *GrayImage); //����Ҷ�ֱ��ͼ���Ը��ʷֲ�����ʽ����
static u8 CalcThresold_Average(vu8 *GrayImage);  //ƽ���Ҷ�ֵ����ֵ
static u8 CalcThresold_MultiAverage(vu8 *GrayImage); //������ֵ��
static vu8 CalcThresold_SimpleTwoPeak_ByGrayHist(vu8 *GrayImage);//�򵥵�˫�巨ȡ��ֵ�����Ȳ�
static void GrayGradient_Diffrential(vu8 *Gradient,vu8 *GrayImage,u8 GradientThreshold);//��������洢��ÿ�����ص��ݶȣ���ֵ���ϵ��ݶ���Ϊ255������˫���ִ���˫��΢��
static void GrayGradient_Roberts(vu8 *Gradient,vu8 *GrayImage,u8 GradientThreshold); //Roberts�������ݶ�
static void GrayGradient_Sobel(vu8 *Gradient,vu8 *GrayImage,u8 GradientThreshold); //Sobel�������ݶ�
static void GrayImageMapping_ByChanceHist(vu8 *mapping,vu8 *GrayImage);

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
void Projective_H(vu16 *projection_x,vu8 *BinaryImage);             //����ͶӰ��x��
void Projective_V(vu16 *projection_y,vu8 *BinaryImage);             //����ͶӰ��y��
void GrayGradientToMask(vu8 *Mask,vu8 *Gradient);           //�����ݶ�ͼ������ģ�����ڲ���Ϊ255���������Ϊ������ݶ�ͼ����
void BinaryToEdge(vu8 *BinaryImage);                  //�Ϳ��ڲ��㷨ȡ��ǰ������
void ObjectMeasure_FromBinary(vu8 *ObjectFlag,vu8 *BinaryImage,vu32 *ObjectSize);//��Ǹ������壬�����أ����ֵ��1��ʼ��ObjectSize�����¼ÿ���������ռ�����ش�С


#ifdef __cplusplus
}
#endif /*extern "C"*/

#endif /* _PICTURE_TOOL_H */
