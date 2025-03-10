#include "DataProc.h"
#include "Fatfs/ff.h"
#include "FATFS/FATFS_APP/fatfs_app.h"
#include "lvgl/lvgl.h"
#include <stdio.h>

extern lv_fs_file_t lv_file;
extern lv_fs_dir_t lv_dir;
extern char dirfilename[DIRFILENUM];

extern FATFS *fs[_VOLUMES];  
extern FIL *file;	 
extern FIL *ftemp;	 
extern UINT br,bw;
extern FILINFO *fileinfo;
extern DIR *dir;
extern u8 fs_ready_status[_VOLUMES];
extern u8 *fatbuf;//Êý¾Ý»º´æÇø,1024byte

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_NOTIFY)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(DataProc::Search_Info_t))
    {
        return Account::RES_SIZE_MISMATCH;
    }

    DataProc::Search_Info_t* info = (DataProc::Search_Info_t*)param->data_p;

    FRESULT res;
    char* dfn = dirfilename;
    uint32_t filenum = 0;
    
    LV_ASSERT_NULL(info->dir);
    LV_ASSERT_NULL(info->namesearch);

    res = f_findfirst(dir,fileinfo,info->dir,info->namesearch);
    if(res == FR_OK && fileinfo->fname[0] != NULL)
    {
        while(1)
        {
            dfn = dirfilename;
            sprintf(dfn,"%s%s",info->dir,"/");
            // strcpy(*dfn,info->dir);
            // strcat(*dfn,"/");
#if _USE_LFN         
            strcat(dfn,fileinfo->lfname);
#else
            strcat(*dfn,fileinfo->fname);
#endif /*_USE_LFN*/			
            f_findnext(dir,fileinfo);
            filenum++;
            dfn++;
            if(res != FR_OK || fileinfo->fname[0] == NULL)
            {
                break;
            }
        }
        f_closedir(dir);
    }
    
    info->npfilename = filenum;
    info->pfilename = dfn;

    // account->Commit(&info, sizeof(info));
    // account->Publish();

    return Account::RES_OK;
}

DATA_PROC_INIT_DEF(Search)
{
    account->SetEventCallback(onEvent);
}
