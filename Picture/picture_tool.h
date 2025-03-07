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
#define imageBuffer_Num		3		//受限于外部SRAM的总大小，至多为3
extern volatile uint8_t imageBuffer[imageBuffer_Num][imagePixel_H*imagePixel_V];
#endif //USE_EXRAM_IMAGE_BUFFER

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

/*****      内部调用       *****/
static u16 ArrayAddup(vu8 *Array,u8 Num);
static void fastsort(vu8 *arrayhead,vu8 *arrayend); //快速排序
static u16 RGB565ToHSV745(vu16 RGB565); //RGB565转HSV745
static void CalcGrayHist_Chance(volatile float *histchance,vu8 *GrayImage); //计算灰度直方图，以概率分布的形式保存
static u8 CalcThresold_Average(vu8 *GrayImage);  //平均灰度值作阈值
static u8 CalcThresold_MultiAverage(vu8 *GrayImage); //迭代阈值法
static vu8 CalcThresold_SimpleTwoPeak_ByGrayHist(vu8 *GrayImage);//简单的双峰法取阈值，精度差
static void GrayGradient_Diffrential(vu8 *Gradient,vu8 *GrayImage,u8 GradientThreshold);//返回数组存储其每个像素的梯度，阈值以上的梯度置为255。运用双向差分代替双向微分
static void GrayGradient_Roberts(vu8 *Gradient,vu8 *GrayImage,u8 GradientThreshold); //Roberts算子求梯度
static void GrayGradient_Sobel(vu8 *Gradient,vu8 *GrayImage,u8 GradientThreshold); //Sobel算子求梯度
static void GrayImageMapping_ByChanceHist(vu8 *mapping,vu8 *GrayImage);

static void Corrode(vu8 *Image);                      //腐蚀
static void Expand(vu8 *Image);                       //膨胀


/*****      外部调用       *****/
void CapLCDToGray(vu8 *GrayImage);                    //读取LCD屏，并存储到SRAM
void GrayEven(vu8 *GrayImage);                        //灰度图均衡化
void GrayChange_Linear(vu8 *GrayImage,float modulus,int8_t trans);//线性改变灰度图各个像素的灰度值
void OptionalFilter(vu8 *Image);                      //自适应选择性平滑滤波，其内有9个不同方向算子，能有效保留边沿
void MedianFilter(vu8 *Image);                        //中值滤波
void CloseOperation(vu8 *Image);                      //闭运算
void OpenOperation(vu8 *Image);                       //开运算
void GrayToBinary(vu8 *GrayImage);                    //灰度图转化为二值图   
void ShowGrayToLCD(vu8 *GrayImage);                   //把灰度图打印到LCD屏
void ShowBinaryToLCD(vu8 *BinaryImage);               //把二值图打印到LCD屏
void ImageAjust_Horizontal(vu8 *ImageToAjust,volatile int8_t *offset);//倾斜校正，传入每行偏移量数组,*offset 小于0表示左移，大于0表示右移
void Projective_H(vu16 *projection_x,vu8 *BinaryImage);             //纵向投影到x轴
void Projective_V(vu16 *projection_y,vu8 *BinaryImage);             //横向投影到y轴
void GrayGradientToMask(vu8 *Mask,vu8 *Gradient);           //基于梯度图制作掩模，遮掩部分为255，输入参数为所需的梯度图函数
void BinaryToEdge(vu8 *BinaryImage);                  //掏空内部点法取得前景边沿
void ObjectMeasure_FromBinary(vu8 *ObjectFlag,vu8 *BinaryImage,vu32 *ObjectSize);//标记各个物体，并返回，标记值从1开始，ObjectSize数组记录每个标记物体占用像素大小


#ifdef __cplusplus
}
#endif /*extern "C"*/

#endif /* _PICTURE_TOOL_H */
