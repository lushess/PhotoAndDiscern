#ifndef __CAMERA_PRESENTER_H
#define __CAMERA_PRESENTER_H

#include "CameraView.h"
#include "CameraModel.h"

namespace Page
{

class Camera : public PageBase
{
public:
    typedef struct
    {
        lv_color_t color;
    } Param_t;

public:
    Camera();
    virtual ~Camera();

    virtual void onCustomAttrConfig();
    virtual void onViewLoad();
    virtual void onViewDidLoad();
    virtual void onViewWillAppear();
    virtual void onViewDidAppear();
    virtual void onViewWillDisappear();
    virtual void onViewDidDisappear();
    virtual void onViewUnload();
    virtual void onViewDidUnload();

private:
    void Update();
    void AttachEvent(lv_obj_t* obj);
    void onlabelClicked(lv_obj_t* label);
    void onBeepTimeCb(lv_timer_t* timer); 
    void CameraRefresh_Cmd(bool en);     
    static void onEvent(lv_event_t* event);
private:
    //const int enrefresh_event;/* 定义允许更新屏幕中断掩码 */
    //const int disrefresh_event;/* 定义禁止更新屏幕中断掩码 */
    // const int refresh_event;/* 定义更新屏幕中断掩码 */
    // int r_event; /* 定义一个事件接收变量 */
    // int last_event;/* 定义一个保存事件的变量 */
private:
    CameraView View;
    CameraModel Model;
    lv_timer_t* beeptimer;
    lv_obj_t* lastFocus;//用于屏幕以外的外设
};

}

#endif /*__CAMERA_PRESENTER_H*/
