#include "CameraModel.h"

using namespace Page;

void CameraModel::Init()
{
    account = new Account("CameraModel", DataProc::Center(), 0, this);
    account->Subscribe("Shutter");
    account->Subscribe("Beep");

    DATA_PROC_INIT_STRUCT(Beep_info);
    DATA_PROC_INIT_STRUCT(Shutter_info);
}

void CameraModel::Deinit()
{
    if (account)
    {
        delete account;
        account = nullptr;
    }
}

void CameraModel::Beep(uint32_t tim)
{
    Beep_info.tim = tim;
    account->Notify("Beep",&Beep_info, sizeof(Beep_info));
}

void CameraModel::Shutter(bool photo)
{
    Shutter_info.photo = photo;
    account->Notify("Shutter",&Shutter_info, sizeof(Shutter_info));
}
