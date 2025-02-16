#include "Camera.h"
#include "task/user_task.h"

using namespace Page;

Camera::Camera()
    : beeptimer(nullptr)
    , lastFocus(nullptr)
    
{
}

Camera::~Camera()
{

}

void Camera::onCustomAttrConfig()
{
    //SetCustomCacheEnable(true);
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_NONE);
}

void Camera::onViewLoad()
{
    Model.Init();
    View.Create(_root);
    Model.Shutter_info.opt = DataProc::EAN13_OPT;

    AttachEvent(View.ui.shutter.shutterex);
    AttachEvent(View.ui.shutter.shutterin);
    AttachEvent(View.ui.optlabel.chara);
    AttachEvent(View.ui.optlabel.ean13);
    AttachEvent(View.ui.storebtn.btn);
}

void Camera::onViewDidLoad()
{
    
}

void Camera::onViewWillAppear()
{       
    lv_indev_wait_release(lv_indev_get_act());
    lv_group_t* group = lv_group_get_default();
    LV_ASSERT_NULL(group);

    lv_group_set_wrap(group, false);

    lv_group_add_obj(group, View.ui.shutter.shutterex);
    lv_group_add_obj(group, View.ui.shutter.shutterin);
    lv_group_add_obj(group, View.ui.optlabel.chara);
    lv_group_add_obj(group, View.ui.optlabel.ean13);
    lv_group_add_obj(group, View.ui.storebtn.btn);

    if (lastFocus)
    {
        lv_group_focus_obj(lastFocus);
    }
    else
    {
        lv_group_focus_obj(View.ui.shutter.shutterex);
    }

    CameraRefresh_Cmd(true);

    Update();

    View.AppearAnimStart();
}

void Camera::onViewDidAppear()
{
    Param_t param;
    param.color = lv_color_black();

    PAGE_STASH_POP(param);

    lv_obj_set_style_bg_color(_root, param.color, LV_PART_MAIN);
    
}

void Camera::onViewWillDisappear()
{
    lv_group_t* group = lv_group_get_default();
    LV_ASSERT_NULL(group);
    lastFocus = lv_group_get_focused(group);
    lv_group_remove_all_objs(group);
    
    CameraRefresh_Cmd(false);
}

void Camera::onViewDidDisappear()
{ 
    
}

void Camera::onViewUnload()
{
    Model.Deinit();
    View.Delete();
}

void Camera::onViewDidUnload()
{
    
}

void Camera::AttachEvent(lv_obj_t* obj)
{
    //lv_obj_set_user_data(obj, this);
    lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

void Camera::Update()
{
    
}

void Camera::CameraRefresh_Cmd(bool en)
{
    if(en)
    {
        //参数定义在user_task.h
        xTaskNotify((TaskHandle_t	)camera_refresh_Handle,
                    (uint32_t	)ENREFRESH_EVENT, 
                    (eNotifyAction)eSetBits);
    }
    else
    {
        xTaskNotify((TaskHandle_t	)camera_refresh_Handle,
                    (uint32_t	)DISREFRESH_EVENT,
                    (eNotifyAction)eSetBits);
    }
}

void Camera::onlabelClicked(lv_obj_t* label)
{
    lv_coord_t y_chara = lv_obj_get_y(View.ui.optlabel.chara);
    lv_coord_t y_ean13 = lv_obj_get_y(View.ui.optlabel.ean13);
    if(label==View.ui.optlabel.chara)
    {
        Model.Shutter_info.opt = DataProc::CHARA_OPT;      
        if (y_chara>y_ean13) View.OptlabelAnimStart(false);
    }
    else if(label==View.ui.optlabel.ean13)
    {
        Model.Shutter_info.opt = DataProc::EAN13_OPT;
        if (y_ean13>y_chara) View.OptlabelAnimStart(false);
    }
}

void Camera::onEvent(lv_event_t* event)
{
    Camera* instance = (Camera*)lv_event_get_user_data(event);
    LV_ASSERT_NULL(instance);

    lv_obj_t* obj = lv_event_get_current_target(event);
    lv_event_code_t code = lv_event_get_code(event);

    if (code == LV_EVENT_SHORT_CLICKED)
    {
        instance->onlabelClicked(obj);           
    }
    if(obj==instance->View.ui.shutter.shutterex||obj==instance->View.ui.shutter.shutterin)
    {
        if(code == LV_EVENT_CLICKED)
        {
            instance->Model.Beep(300);
            instance->Model.Shutter(false); //无拍照
        }
        else if (code == LV_EVENT_LONG_PRESSED)
        {           
            instance->Model.Beep(500);
            instance->Model.Shutter(true); //有拍照
        }       
    }
    else if(obj == instance->View.ui.storebtn.btn)   
    {
        
        if(code == LV_EVENT_CLICKED)
        {
            instance->_Manager->Push("Pages/MemStore");
        }
    }
}
