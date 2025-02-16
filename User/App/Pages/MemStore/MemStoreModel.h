#ifndef __MEMSTORE_MODEL_H
#define __MEMSTORE_MODEL_H

#include "lvgl/lvgl.h"
#include "Common/DataProc/DataProc.h"
#include "../../../lv_port_fs_Fatfs.h"

namespace Page
{

class MemStoreModel
{
public:
    DataProc::SDcard_Info_t SD_Info;
    //DataProc::SDcard_Basic_Info_t SD_B_Info;
    DataProc::Flash_Info_t Flash_Info;
    //DataProc::Flash_Basic_Info_t Flash_B_Info;
    DataProc::Search_Info_t Search_Info;

    const char* SDCARD_PHOTO_PATH = "0:/Photo";
    const char* FLASH_PHOTO_PATH = "1:/Photo";

public:
    void Init();
    void Deinit();
    bool NoDri();
    void UpdateDri();
    void SDcardFileOpen(const char* path);
    void FlashFileOpen(const char* path);
    void SendPathToPicturePresentPage(const char* path);
    void Search(const char* dir, const char* namesearch);

    void GetSDCardDirContent(  
                        const char* dir,                                               
                        const char** pfilename, 
                        uint32_t nfilename
                    );

    void GetSDCardVolumeInfo(uint32_t totalSizeMB, uint32_t freeSizeMB); //GetSDCardDirContent 会同时获取b_Info                   

    void GetFlashDirContent( 
                        const char* dir,                                                        
                        const char** pfilename, 
                        uint32_t nfilename
                    );

    void GetFlashVolumeInfo(uint32_t totalSizeKB, uint32_t freeSizeKB); 

private:
    Account* account;

private:
    static int onEvent(Account* account, Account::EventParam_t* param);

};

}

#endif //!__MEMSTORE_MODEL_H
