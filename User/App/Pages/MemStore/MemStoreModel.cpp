#include "MemStoreModel.h"

using namespace Page;

void MemStoreModel::Init()
{
    account = new Account("MemStoreModel", DataProc::Center(), 0, this);
    account->Subscribe("SDcard");
    account->Subscribe("Flash");
    account->Subscribe("Search");
    account->Subscribe("PicturePresent");
    //account->SetEventCallback(onEvent);

    DATA_PROC_INIT_STRUCT(SD_Info);
    DATA_PROC_INIT_STRUCT(Flash_Info);
    DATA_PROC_INIT_STRUCT(Search_Info);

    SD_Info.b_info.isReady = lv_fs_is_ready('0')? true : false;
    Flash_Info.b_info.isReady = lv_fs_is_ready('1')? true : false;
}

void MemStoreModel::Deinit()
{
    if (account)
    {
        delete account;
        account = nullptr;
    }
}

int MemStoreModel::onEvent(Account *account, Account::EventParam_t *param)
{
    if (param->event != Account::EVENT_PUB_PUBLISH)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    if(strcmp(param->tran->ID, "SDcard") == 0 && param->size == sizeof(DataProc::SDcard_Info_t))
    {
        MemStoreModel *instance = (MemStoreModel *)account->UserData;
        memcpy(&(instance->SD_Info), param->data_p, param->size);
    }
    else if(strcmp(param->tran->ID, "Flash") == 0 && param->size == sizeof(DataProc::Flash_Info_t))
    {
        MemStoreModel *instance = (MemStoreModel *)account->UserData;
        memcpy(&(instance->Flash_Info), param->data_p, param->size);
    }
    else if(strcmp(param->tran->ID, "Search") == 0 && param->size == sizeof(DataProc::Search_Info_t))
    {
        MemStoreModel *instance = (MemStoreModel *)account->UserData;
        memcpy(&(instance->Search_Info), param->data_p, param->size);
    }
    else
    {
        return Account::RES_PARAM_ERROR;
    }

    return Account::RES_OK;
}

bool MemStoreModel::NoDri()
{
    if(SD_Info.b_info.isReady == false && \
       Flash_Info.b_info.isReady == false)
       return true;
    else return false;
}

void MemStoreModel::UpdateDri()
{
    if(SD_Info.b_info.isReady == false || \
       Flash_Info.b_info.isReady == false)
    {
        lv_port_fs_init();
        SD_Info.b_info.isReady = lv_fs_is_ready('0')? true : false;
        Flash_Info.b_info.isReady = lv_fs_is_ready('1')? true : false;
    }  
}

void MemStoreModel::SDcardFileOpen(const char* path)
{
    SD_Info.path = path;
    SD_Info.cmd = DataProc::STORE_CMD_OPEN;
    account->Notify("SDcard",&SD_Info,sizeof(SD_Info));
}

void MemStoreModel::FlashFileOpen(const char* path)
{
    Flash_Info.path = path;
    Flash_Info.cmd = DataProc::STORE_CMD_OPEN;
    account->Notify("SDcard",&Flash_Info,sizeof(Flash_Info));
}

void MemStoreModel::SendPathToPicturePresentPage(const char* path)
{
    DataProc::PicturePresent_Info_t info;
    DATA_PROC_INIT_STRUCT(info);

    strcpy(info.photopath,path);

    account->Notify("PicturePresent",&info,sizeof(info));
}

void MemStoreModel::GetSDCardDirContent(  
                            const char* dir,                                               
                            const char* pfilename, 
                            uint32_t nfilename
                            )
{
    SD_Info.path = dir;
    SD_Info.cmd = DataProc::STORE_CMD_DIR_READ;
    account->Notify("SDcard",&SD_Info,sizeof(SD_Info));

    pfilename = (char*)SD_Info.data;
    nfilename = SD_Info.size;
}

void MemStoreModel::GetSDCardVolumeInfo(uint32_t totalSizeMB, uint32_t freeSizeMB)
{
    totalSizeMB = SD_Info.b_info.totalSizeMB;
    freeSizeMB = SD_Info.b_info.freeSizeMB;
}

void MemStoreModel::GetFlashDirContent(  
                            const char* dir,                                               
                            const char* pfilename, 
                            uint32_t nfilename
                            )
{
    Flash_Info.path = dir;
    Flash_Info.cmd = DataProc::STORE_CMD_DIR_READ;
    account->Notify("Flash",&Flash_Info,sizeof(Flash_Info));

    pfilename = (char*)Flash_Info.data;
    nfilename = Flash_Info.size;
}

void MemStoreModel::GetFlashVolumeInfo(uint32_t totalSizeKB, uint32_t freeSizeKB)
{
    totalSizeKB = Flash_Info.b_info.totalSizeKB;
    freeSizeKB = Flash_Info.b_info.freeSizeKB;
}

void MemStoreModel::Search(const char* dir, const char* namesearch)
{
    Search_Info.dir = dir;
    Search_Info.namesearch = namesearch;
    account->Notify("Search",&Search_Info,sizeof(Search_Info));
}


