#ifndef _PICTURE_MEMORY_MANAGER_H
#define _PICTURE_MEMORY_MANAGER_H

#include "system.h"
#include "Config/Config.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_FREERTOS_MALLOCATOR
#include "freeRTOS.h"
#endif //USE_FREERTOS_MALLOCATOR

#define imagePixel_H		240
#define imagePixel_V		320
#define imageBuffer_Num		3		//受限于外部SRAM的总大小，至多为3

typedef enum {BUFFER_IDLE = 0,BUFFER_BE_USED = (!BUFFER_IDLE)} ImageBufferIdleFlag;
typedef struct{
	volatile uint8_t *bufferAddrBase;
	uint32_t bufferSize;
	ImageBufferIdleFlag bufferStat;
}ImageBufferStatus;

#if defined(USE_EXRAM_IMAGE_BUFFER)||defined(USE_INRAM_IMAGE_BUFFER)
		extern volatile uint8_t imageBuffer[imageBuffer_Num][imagePixel_H*imagePixel_V];
#else
		extern volatile uint8_t imageBuffer[imageBuffer_Num][0];
#endif //defined(USE_EXRAM_IMAGE_BUFFER)||defined(USE_INRAM_IMAGE_BUFFER)

//必须先调用imageBufStat_Init()，初始化状态结构体;
void imageBufStat_Init(void);
void *MemorySelector_Malloc(size_t mallocSize);
void MemorySelector_Free(void* pMalloc);

#define Memalloc MemorySelector_Malloc
#define Memfree	MemorySelector_Free

#ifdef __cplusplus
}
#endif /*extern "C"*/

#endif //_PICTURE_MEMORY_MANAGER_H
