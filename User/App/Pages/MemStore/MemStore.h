#ifndef __MEMSTORE_PRESENTER_H
#define __MEMSTORE_PRESENTER_H

#include "MemStoreView.h"
#include "MemStoreModel.h"

namespace Page
{

class MemStore : public PageBase
{
public:
    MemStore();
    virtual ~MemStore();

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
    void onFilelistupdate(bool en);
    static void onTimerUpdate(lv_timer_t* timer);
    static void onEvent(lv_event_t* event);
    static void onFileBtnClickEvent(lv_event_t* event);    
    
private:
    MemStoreView View;
    MemStoreModel Model;
    lv_timer_t* timer;
    lv_obj_t* lastFocus;
    char* currentpath;
    bool listchanged;

};

}

#endif //!__MEMSTORE_PRESENTER_H
