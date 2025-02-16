#include "DataProc.h"
//#include "../HAL/HAL.h"
#include "lvgl/lvgl.h"
#include "../App/beep/beep.h"

static void onBeepTimeCb(lv_timer_t* beeptimer)
{
    beep = 0;
}

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_NOTIFY)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(DataProc::Beep_Info_t))
    {
        return Account::RES_SIZE_MISMATCH;
    }

    DataProc::Beep_Info_t* info = (DataProc::Beep_Info_t*)param->data_p;

    if(info->cmd == DataProc::BEEP_Is_Used)
    {
       if(info->tim == NULL) info->tim = 1000;
       beep = 1;
       lv_timer_t* beeptimer = lv_timer_create(onBeepTimeCb, info->tim, NULL);
       lv_timer_set_repeat_count(beeptimer, 1);
    }   
    else beep = 0;

    return Account::RES_OK;
}

DATA_PROC_INIT_DEF(Beep)
{
    account->SetEventCallback(onEvent);
}
