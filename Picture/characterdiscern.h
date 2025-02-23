#ifndef _CHARACTERDISCERN_H
#define _CHARACTERDISCERN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

#define FLASH_ASCII_ADDR     6112846  //ASCII�ֿ��׵�ַ��6114304 - 1456 - 2��
#define FLASH_12CHAR_ADDR    6114304  //12���ֿ��׵�ַ��6880386 - 766080 - 2��
#define FLASH_16CHAR_ADDR    6880386  //16���ֿ��׵�ַ��8388608 - 1508220 - 2��


#define Charater_DeepColor   1
//#define Charater_LightColor  1
#define Charater_xmaxForNormalization      16
#define Charater_ymaxForNormalization      16
#define CharacterNumToCapture              3
#define CharacterErrorThreshold            10
#define CharacterMatchBuf                  5
#define WordCompareFromExflash             0x23ee      //(0xe0-0xb0-1)*190+0xa1-0x41+0xed-0x41�����Ӱ��ֵ�����



/*****      �ڲ�����       *****/
static u8 calcCharacterBit(vu8 *CharacterMatrix);
static void calcGridBit(vu8 *CharacterMatrix,vu8 *Grid);
static u16 FindMaxNumFromArray(vu16 *array,vu16 size);
static void CalcChangePoint_V(vu16 *ChangePointArray_V,vu8 *BinaryImage); //���ڼ�¼��������㣬��240�����ݣ�ChangePointArray_V[x]����xΪͼƬ������
static void CalcChangePoint_H(vu16 *ChangePointArray_H,vu8 *BinaryImage); //���ڼ�¼��������㣬��320�����ݣ�ChangePointArray_H[y]����yΪͼƬ������
static void CharacterSegmentation_x(vu16 *xArray,vu8 *BinaryImage);//x����ÿ����Ԫ�طֱ��ʾxleft,xright
static void CharacterSegmentation_y(vu16 *yArray,vu8 *BinaryImage);//y����ÿ����Ԫ�طֱ��ʾyup,ylow
static void SaveCharacterImage(vu8 *CharacterImage,vu8 *BinaryImage);//CharacterImage���ڴ洢�ַ����أ�������0x0f����
static void CharaterNormalized(vu8 *CharacterMatrix,vu8 *BinaryImage);//�ַ���һ����ͬʱ������8λתΪ1λ��ʾ

/*****      �ⲿ����       *****/
void CharacterCompareFromExflash(vu8 *BinaryImage,vu16 *Character_GB2312); //���ⲿFLASH��ȡ(16*16)��ģ���бȶԣ��洢�ַ����뵽Character_GB2312����


#ifdef __cplusplus
}
#endif /*extern "C"*/

#endif /* _CHARACTERDISCERN_H */
