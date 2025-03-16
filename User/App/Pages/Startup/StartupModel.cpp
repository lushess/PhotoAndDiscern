#include "StartUpModel.h"

using namespace Page;

void StartupModel::Init()
{
    account = new Account("StartupModel", DataProc::Center(), 0, this);
    account->Subscribe("Beep");
    //account->Subscribe("StatusBar");
}

void StartupModel::Deinit()
{
    if (account)
    {
        delete account;
        account = NULL;
    }
}

void StartupModel::Beep(uint32_t tim)
{
    DataProc::Beep_Info_t info;
    DATA_PROC_INIT_STRUCT(info);
    info.cmd = DataProc::BEEP_Is_Used;
    info.tim = tim;
    account->Notify("Beep",&info, sizeof(info));
}
/*
void StartupModel::SetStatusBarAppear(bool en)
{
    DataProc::StatusBar_Info_t info;
    DATA_PROC_INIT_STRUCT(info);
    info.cmd = DataProc::STATUS_BAR_CMD_APPEAR;
    info.param.appear = en;
    account->Notify("StatusBar", &info, sizeof(info));
}
*/
