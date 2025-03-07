#include "picturememorymanager.h"


#ifdef USE_EXRAM_IMAGE_BUFFER
		__USED __EXRAM volatile uint8_t imageBuffer[imageBuffer_Num][imagePixel_H*imagePixel_V];
#else
		#ifdef USE_INRAM_IMAGE_BUFFER
				__USED __INRAM volatile uint8_t imageBuffer[imageBuffer_Num][imagePixel_H*imagePixel_V];
		#else
				volatile uint8_t imageBuffer[imageBuffer_Num][0];
		#endif //USE_INRAM_IMAGE_BUFFER
#endif //USE_EXRAM_IMAGE_BUFFER
__INRAM static ImageBufferStatus imageBufStat[imageBuffer_Num];


void imageBufStat_Init(void){
	for(uint16_t i = 0;i<imageBuffer_Num;i++){
		imageBufStat[i].bufferAddrBase = imageBuffer[i];
		imageBufStat[i].bufferSize = imagePixel_H*imagePixel_V*sizeof(volatile uint8_t);
		imageBufStat[i].bufferStat = BUFFER_IDLE;
	}
}

void *MemorySelector_Malloc(size_t mallocSize){
	//判断要分配的内存是否为image的需求
	if(mallocSize == imagePixel_H*imagePixel_V*sizeof(volatile uint8_t)){
		//遍历状态表，查找空闲缓冲区
		for(uint16_t i = 0;i<imageBuffer_Num;i++){ 
			if(imageBufStat[i].bufferStat == BUFFER_IDLE){
				imageBufStat[i].bufferStat = BUFFER_BE_USED;
				return (void *)imageBufStat[i].bufferAddrBase;
			}
		}
		return NULL;
	}
	else // 调用堆分配函数
	{
		#ifdef USE_FREERTOS_MALLOCATOR
		return pvPortMalloc(mallocSize);
		#else
		#include <stdlib.h>
		return malloc(mallocSize);
		#endif //USE_FREERTOS_MALLOCATOR
	}
}

void MemorySelector_Free(void* pMalloc){
	if(pMalloc != NULL){
		for(uint16_t i = 0;i<imageBuffer_Num;i++){ 
				if(imageBufStat[i].bufferAddrBase == pMalloc){
					imageBufStat[i].bufferStat = BUFFER_IDLE;
					return;
				}
			}
		#ifdef USE_FREERTOS_MALLOCATOR
		return vPortFree(pMalloc);
		#else
		#include <stdlib.h>
		return free(pMalloc);
		#endif //USE_FREERTOS_MALLOCATOR
	}
}