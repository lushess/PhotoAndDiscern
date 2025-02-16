#ifndef __CAMERA_VIEW_H
#define __CAMERA_VIEW_H

#include "../Page.h"

namespace Page
{

class CameraView
{
public:
    void Create(lv_obj_t* root);
    void Delete();
    void AppearAnimStart(bool reverse = false);
    void OptlabelAnimStart(bool reverse = false);

public:
    struct
    {
        struct 
        {
            lv_obj_t* shutterex;
            lv_obj_t* shutterin;
        }shutter;      
        
        struct 
        {
            lv_obj_t* ean13;
            lv_obj_t* chara;
            lv_obj_t* opt;
            lv_obj_t* cont;
            lv_anim_timeline_t* label_anim;
        }optlabel;
        
        struct 
        {
            lv_obj_t* slider;
            lv_obj_t* sliderlabel;
        }threashold_slider;  

        struct 
        {
            lv_obj_t* btn;
            lv_obj_t* icon;
            lv_obj_t* cont;
        }storebtn;
             

        lv_anim_timeline_t* anim_timeline;
    } ui;

private:
    void ShutterCreat(lv_obj_t* par);
    void OptlabelCreat(lv_obj_t* par);
    void StorebtnCreat(lv_obj_t* par);
};

}

#endif /*__CAMERA_VIEW_H*/
