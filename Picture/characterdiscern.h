#ifndef _CHARACTERDISCERN_H
#define _CHARACTERDISCERN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

#define FLASH_ASCII_ADDR     6112846  //ASCII字库首地址（6114304 - 1456 - 2）
#define FLASH_12CHAR_ADDR    6114304  //12号字库首地址（6880386 - 766080 - 2）
#define FLASH_16CHAR_ADDR    6880386  //16号字库首地址（8388608 - 1508220 - 2）


#define Charater_DeepColor   1
//#define Charater_LightColor  1
#define Charater_xmaxForNormalization      16
#define Charater_ymaxForNormalization      16
#define CharacterNumToCapture              3
#define CharacterErrorThreshold            10
#define CharacterMatchBuf                  5
#define WordCompareFromExflash             0x23ee      //(0xe0-0xb0-1)*190+0xa1-0x41+0xed-0x41，即从啊字到口字



/*****      内部调用       *****/
static u8 calcCharacterBit(vu8 *CharacterMatrix);
static void calcGridBit(vu8 *CharacterMatrix,vu8 *Grid);
static u16 FindMaxNumFromArray(vu16 *array,vu16 size);
static void CalcChangePoint_V(vu16 *ChangePointArray_V,vu8 *BinaryImage); //用于记录纵向跳变点，共240个数据，ChangePointArray_V[x]其中x为图片横坐标
static void CalcChangePoint_H(vu16 *ChangePointArray_H,vu8 *BinaryImage); //用于记录横向跳变点，共320个数据，ChangePointArray_H[y]其中y为图片纵坐标
static void CharacterSegmentation_x(vu16 *xArray,vu8 *BinaryImage);//x数组每两个元素分别表示xleft,xright
static void CharacterSegmentation_y(vu16 *yArray,vu8 *BinaryImage);//y数组每两个元素分别表示yup,ylow
static void SaveCharacterImage(vu8 *CharacterImage,vu8 *BinaryImage);//CharacterImage用于存储字符像素，其中用0x0f隔开
static void CharaterNormalized(vu8 *CharacterMatrix,vu8 *BinaryImage);//字符归一化，同时像素由8位转为1位表示

/*****      外部调用       *****/
void CharacterCompareFromExflash(vu8 *BinaryImage,vu16 *Character_GB2312); //从外部FLASH读取(16*16)字模进行比对，存储字符编码到Character_GB2312数组


#ifdef __cplusplus
}
#endif /*extern "C"*/

#endif /* _CHARACTERDISCERN_H */
