#include "DataProc.h"
#include "FATFS/FATFS_APP/fatfs_app.h"
#include "lvgl/lvgl.h"

extern lv_fs_file_t lv_file;
extern lv_fs_dir_t lv_dir;
extern char *dirfilename[DIRFILENUM];

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_NOTIFY)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(DataProc::Flash_Info_t))
    {
        return Account::RES_SIZE_MISMATCH;
    }

    DataProc::Flash_Info_t* info = (DataProc::Flash_Info_t*)param->data_p;

    if(info->cmd == DataProc::STORE_CMD_OPEN)
    {
        LV_ASSERT_NULL(info->path);
        
        if(lv_fs_open(&lv_file,info->path,LV_FS_MODE_RD) == LV_FS_RES_OK)
        {
            info->ft = DataProc::T_BIN;
            lv_fs_close(&lv_file);       
        }
        else
        {
            info->ft = DataProc::T_DIR;
            lv_fs_close(&lv_file);
        }
    }
    else if(info->cmd == DataProc::STORE_CMD_DIR_READ)
    {
        char name[128];
        char** dfn = dirfilename;
        uint32_t filenum = 0;
        if(lv_fs_dir_open(&lv_dir,info->path) == LV_FS_RES_OK)
        {
            while(1)
            {
                lv_fs_res_t res = lv_fs_dir_read(&lv_dir,name);

                *dfn = new char[128];

                if (name[0] == '\0' || res != LV_FS_RES_OK)
                {
                    break;
                }

                strcpy(*dfn,name);
                dfn++;
                filenum++;
            }
            lv_fs_dir_close(&lv_dir);
        }
        // delete info->path;
        // info->path = nullptr;
        info->data = dirfilename;
        info->size = filenum;
    }

    uint32_t total,free;

    FATFS_GetFree((uint8_t *)"0:",&total,&free);
    info->b_info.totalSizeKB = total/1024;
    info->b_info.freeSizeKB = free/1024;

    // account->Commit(&info, sizeof(info));
    // account->Publish();

    return Account::RES_OK;
}

DATA_PROC_INIT_DEF(Flash)
{
    account->SetEventCallback(onEvent);
}
