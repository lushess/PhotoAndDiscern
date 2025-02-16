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
    //const int enrefresh_event;/* �������������Ļ�ж����� */
    //const int disrefresh_event;/* �����ֹ������Ļ�ж����� */
    // const int refresh_event;/* ���������Ļ�ж����� */
    // int r_event; /* ����һ���¼����ձ��� */
    // int last_event;/* ����һ�������¼��ı��� */
private:
    CameraView View;
    CameraModel Model;
    lv_timer_t* beeptimer;
    lv_obj_t* lastFocus;//������Ļ���������
};

}

#endif /*__CAMERA_PRESENTER_H*/
