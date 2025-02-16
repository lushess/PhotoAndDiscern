#ifndef __PICTURE_PRESENTER_H
#define __PICTURE_PRESENTER_H

#include "../Page.h"
#include "Common/DataProc/DataProc.h"

namespace Page
{

class PicturePresent : public PageBase
{
public:
    struct 
    {
        lv_obj_t* bmppic;
        lv_obj_t* cont;
    } pic;

    DataProc::PicturePresent_Info_t photo;
public:
    void ViewCreat(lv_obj_t* root);
    void Showpicture(const char* path);
    static int onEventPathReceive(Account* account, Account::EventParam_t* param);

public:
    PicturePresent();
    virtual ~PicturePresent();

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
    static void onTimerUpdate(lv_timer_t* timer);
    static void onEvent(lv_event_t* event);

private:
    Account* account;
    
};

}

#endif /* __PICTURE_PRESENTER_H*/
