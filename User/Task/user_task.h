#ifndef __USER_TASK_H
#define __USER_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"


#define DISREFRESH_EVENT    (0x01<<2)
#define ENREFRESH_EVENT     (0x01<<1)
#define REFRESH_EVENT       0x01

#define EAN13_DISCERN       (0x01<<3)
#define CHARA_DISCERN       (0x01<<4)

#define SDCARD_PHOTO_PATH_DEFAULT  "0:/Photo"
#define FLASH_PHOTO_PATH_DEFAULT   "1:/Photo"

extern TaskHandle_t camera_refresh_Handle;
extern TaskHandle_t discern_Handle;
extern TaskHandle_t photo_Handle;


void camera_refresh(void);
void photo(void);
void discern(void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* __USER_TASK_H */
