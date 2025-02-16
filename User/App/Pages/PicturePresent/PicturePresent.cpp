#include "PicturePresent.h"

using namespace Page;

PicturePresent::PicturePresent()  
                       
{
    
}

PicturePresent::~PicturePresent()
{

}

void PicturePresent::onCustomAttrConfig()
{
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_NONE);
}

void PicturePresent::onViewLoad()
{
    ViewCreat(_root);
    lv_obj_add_event_cb(_root,onEvent,LV_EVENT_SHORT_CLICKED,this);

    account = new Account("MemStoreModel", DataProc::Center(), 0, this);
    account->SetEventCallback(onEventPathReceive);

}

void PicturePresent::onViewDidLoad()
{
    lv_obj_set_style_bg_color(_root, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(_root, LV_OPA_COVER, LV_PART_MAIN);
}

void PicturePresent::onViewWillAppear()
{

    Showpicture(photo.photopath);
}

void PicturePresent::onViewDidAppear()
{
}

void PicturePresent::onViewWillDisappear()
{
}

void PicturePresent::onViewDidDisappear()
{
}

void PicturePresent::onViewUnload()
{
}

void PicturePresent::onViewDidUnload()
{
}

void PicturePresent::AttachEvent(lv_obj_t* obj)
{
    lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

void PicturePresent::Update()
{
}

void PicturePresent::onTimerUpdate(lv_timer_t* timer)
{
}

void PicturePresent::onEvent(lv_event_t* event)
{
    PicturePresent* instance = (PicturePresent*)lv_event_get_user_data(event);
    LV_ASSERT_NULL(instance);

    lv_obj_t* obj = lv_event_get_current_target(event);
    lv_event_code_t code = lv_event_get_code(event);

    if (obj == instance->_root)
    {
        if (code == LV_EVENT_SHORT_CLICKED)
        {
            instance->_Manager->Pop();
        }
    }
}

int PicturePresent::onEventPathReceive(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_NOTIFY)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    if (strcmp(param->tran->ID, "MemStore") != 0 || param->size != sizeof(DataProc::PicturePresent_Info_t))
    {
        return Account::RES_PARAM_ERROR;
    }

    PicturePresent *instance = (PicturePresent *)account->UserData;

    //strcpy(instance->photo.photopath,info->photopath);
    memcpy(&(instance->photo), param->data_p, param->size);
}

void PicturePresent::ViewCreat(lv_obj_t* root)
{
    lv_obj_t* cont = lv_obj_create(root);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont,LV_HOR_RES,LV_VER_RES);
    lv_obj_align(cont,LV_ALIGN_CENTER,0,0);
    lv_obj_add_flag(cont,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(cont,LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(cont,LV_DIR_ALL);
    pic.cont = cont;

    lv_obj_t* bmppic = lv_img_create(cont);
    pic.bmppic = bmppic;
}

void PicturePresent::Showpicture(const char* path)
{
    return lv_img_set_src(pic.bmppic,path);
}
