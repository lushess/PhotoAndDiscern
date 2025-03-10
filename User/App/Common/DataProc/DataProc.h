#ifndef __DATA_PROC_H
#define __DATA_PROC_H

#include "User/App/Utils/DataCenter/DataCenter.h"
#include "DataProc_Def.h"

#define DATA_PROC_INIT_DEF(name)   void _DP_##name##_Init(Account* account)
#define DATA_PROC_INIT_STRUCT(sct) memset(&sct, 0, sizeof(sct))

#define DIRFILENUM  128
extern char dirfilename[DIRFILENUM];
extern lv_fs_file_t lv_file;
extern lv_fs_dir_t lv_dir;

void DataProc_Init();

namespace DataProc
{

DataCenter* Center();
uint32_t    GetTick();
uint32_t    GetTickElaps(uint32_t prevTick);
const char* MakeTimeString(uint64_t ms, char* buf, uint16_t len);

}

#endif
