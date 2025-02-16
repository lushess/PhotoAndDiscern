#ifndef __CAMERA_MODEL_H
#define __CAMERA_MODEL_H

#include "Common/DataProc/DataProc.h"

namespace Page
{

class CameraModel
{
public:
    void Init();
    void Deinit();
    void Beep(uint32_t tim);
    void Shutter(bool photo);
public:
    DataProc::Beep_Info_t Beep_info;
    DataProc::Shutter_Info_t Shutter_info;
private:
   Account* account;  

private:
    
};

}

#endif /*__CAMERA_MODEL_H*/
