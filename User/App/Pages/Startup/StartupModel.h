#ifndef __STARTUP_MODEL_H
#define __STARTUP_MODEL_H

#include "Common/DataProc/DataProc.h"

namespace Page
{

class StartupModel
{
public:
    void Init();
    void Deinit();
    void Beep(uint32_t tim);
    /*
    void SetEncoderEnable(bool en)
    {
        HAL::Encoder_SetEnable(en);
    }
    void SetStatusBarAppear(bool en);
    */

private:
    Account* account;
};

}

#endif
