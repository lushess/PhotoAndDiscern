#include "DataProc.h"
//#include "../HAL/HAL.h"
#include "task/user_task.h"

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_NOTIFY)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(DataProc::Shutter_Info_t))
    {
        return Account::RES_SIZE_MISMATCH;
    }

    DataProc::Shutter_Info_t* info = (DataProc::Shutter_Info_t*)param->data_p;

    xTaskNotify((TaskHandle_t	)camera_refresh_Handle,
                       (uint32_t	)DISREFRESH_EVENT,
                       (eNotifyAction)eSetBits);
                       
    if(info->photo == true) 
    {
        xTaskNotifyGive(photo_Handle);
    }

    if(info->opt==DataProc::EAN13_OPT)
    {
       
        xTaskNotify((TaskHandle_t	)discern_Handle,
                    (uint32_t	)EAN13_DISCERN,
                    (eNotifyAction)eSetBits);
    }
    else if (info->opt==DataProc::CHARA_OPT)
    {

        xTaskNotify((TaskHandle_t	)discern_Handle,
                    (uint32_t	)CHARA_DISCERN,
                    (eNotifyAction)eSetBits);
    }
    
    return Account::RES_OK;
}

DATA_PROC_INIT_DEF(Shutter)
{
    account->SetEventCallback(onEvent);
}
