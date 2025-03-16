#include "MemStore.h"
#include <string>
#include <stdio.h>

using namespace Page;

#define ChangeDriFromTo(dri1,dri2)\
do\
{\
    if(Model.dri2##_Info.b_info.isReady == true)\
    {\
        Model.dri2##_Info.isSelected = true;\
        Model.dri1##_Info.isSelected = false;\
    }\
    else if(Model.dri1##_Info.b_info.isReady == true)\
    {\
        Model.dri2##_Info.isSelected = false;\
        Model.dri1##_Info.isSelected = true;\
    }\
    else\
    {\
        Model.dri2##_Info.isSelected = false;\
        Model.dri1##_Info.isSelected = false;\
    }\
}while(0)

#define onChangeDriFromTo(dri1,dri2)\
do\
{\
    if(instance->Model.dri2##_Info.b_info.isReady == true)\
    {\
        instance->Model.dri2##_Info.isSelected = true;\
        instance->Model.dri1##_Info.isSelected = false;\
    }\
    else if(instance->Model.dri1##_Info.b_info.isReady == true)\
    {\
        instance->Model.dri2##_Info.isSelected = false;\
        instance->Model.dri1##_Info.isSelected = true;\
    }\
    else\
    {\
        instance->Model.dri2##_Info.isSelected = false;\
        instance->Model.dri1##_Info.isSelected = false;\
    }\
}while(0)

#define COLOR_GRAY      lv_color_hex(0x808080)

MemStore::MemStore()
    : timer(NULL),
      lastFocus(NULL),
      listchanged(true)            
{
    currentpath = new char[128];
    strcpy(currentpath, Model.SDCARD_PHOTO_PATH);
}

MemStore::~MemStore()
{

}

void MemStore::onCustomAttrConfig()
{
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_NONE);
}

void MemStore::onViewLoad()
{
    View.Create(_root);
    Model.Init();

    AttachEvent(View.ui.find.txtarea);
    AttachEvent(View.ui.find.button);
    AttachEvent(View.ui.dri.sd);
    AttachEvent(View.ui.dri.flash);
    AttachEvent(View.ui.exit.exit);
    
    ChangeDriFromTo(Flash,SD);

    if(Model.NoDri()) 
    {
        lv_obj_add_flag(View.ui.dri.opt, LV_OBJ_FLAG_HIDDEN);//隐藏选择框
        lv_obj_add_flag(View.ui.volume.cont, LV_OBJ_FLAG_HIDDEN);//隐藏容量信息
    } 
    else if(Model.SD_Info.b_info.isReady == false && \
            Model.Flash_Info.b_info.isReady == true) 
    {
        lv_obj_align_to(View.ui.dri.opt,View.ui.dri.cont,LV_ALIGN_DEFAULT,0,90);//选择框改为Flash
        strcpy(currentpath, Model.FLASH_PHOTO_PATH);
    }
}

void MemStore::onViewDidLoad()
{
    lv_obj_set_style_bg_color(_root, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(_root, LV_OPA_COVER, LV_PART_MAIN);
}

void MemStore::onViewWillAppear()
{
    lv_indev_wait_release(lv_indev_get_act());
    lv_group_t* group = lv_group_get_default();
    LV_ASSERT_NULL(group);

    lv_group_set_wrap(group, false);

    lv_group_add_obj(group, View.ui.find.txtarea);
    lv_group_add_obj(group, View.ui.find.button);
    lv_group_add_obj(group, View.ui.dri.sd);
    lv_group_add_obj(group, View.ui.dri.flash);
    lv_group_add_obj(group, View.ui.exit.exit);

    if (lastFocus)
    {
        lv_group_focus_obj(lastFocus);
    }
    else
    {
        lv_group_focus_obj(View.ui.find.button);
    }

    View.AppearAnimStart();
}

void MemStore::onViewDidAppear()
{
    timer = lv_timer_create(onTimerUpdate, 1000, this);
    onFilelistupdate(listchanged);
}

void MemStore::onViewWillDisappear()
{
    lv_group_t* group = lv_group_get_default();
    LV_ASSERT_NULL(group);
    lastFocus = lv_group_get_focused(group);
    lv_group_remove_all_objs(group);
    
}

void MemStore::onViewDidDisappear()
{
    lv_timer_del(timer);
}

void MemStore::onViewUnload()
{
    View.Delete();
    Model.Deinit();
}

void MemStore::onViewDidUnload()
{
}

void MemStore::AttachEvent(lv_obj_t* obj)
{
    lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

void MemStore::Update()
{
    Model.UpdateDri();
    if(!Model.NoDri()) 
    {
        lv_obj_clear_flag(View.ui.dri.opt, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(View.ui.volume.cont, LV_OBJ_FLAG_HIDDEN);
    }

    onFilelistupdate(listchanged);
    
}

void MemStore::onFilelistupdate(bool en)
{
    const char* pfilename;
    uint32_t nfilename;

    if(en)
    {
        if(Model.SD_Info.isSelected == true)
        {
            if(currentpath != NULL)
            {
                Model.GetSDCardDirContent(currentpath,pfilename,nfilename);
                View.ClearFileList();
                View.AddBtnToFileList(nfilename,pfilename,onFileBtnClickEvent);
            } 
            else
            {
                View.ClearFileList();
                View.AddBtnToFileList(Model.Search_Info.npfilename,Model.Search_Info.pfilename,onFileBtnClickEvent);
            }     
            lv_label_set_text_fmt(View.ui.volume.total.value, "%d", Model.SD_Info.b_info.totalSizeMB); //SDcard总容量
            lv_label_set_text(View.ui.volume.total.unit,"MB"); 
        }
        else if(Model.Flash_Info.isSelected == true)
        {
            if(currentpath != NULL)
            {
                Model.GetFlashDirContent(currentpath,pfilename,nfilename);
                View.ClearFileList();
                View.AddBtnToFileList(nfilename,pfilename,onFileBtnClickEvent);
            } 
            else
            {
                View.ClearFileList();
                View.AddBtnToFileList(Model.Search_Info.npfilename,Model.Search_Info.pfilename,onFileBtnClickEvent);
            }      
            lv_label_set_text_fmt(View.ui.volume.total.value, "%d", Model.Flash_Info.b_info.totalSizeKB); //Flash总容量
            lv_label_set_text(View.ui.volume.total.unit,"KB"); 
        }

        listchanged = false;
    }
}

void MemStore::onTimerUpdate(lv_timer_t* timer)
{
    MemStore* instance = (MemStore*)timer->user_data;

    instance->Update();
}

void MemStore::onEvent(lv_event_t* event)
{
    MemStore* instance = (MemStore*)lv_event_get_user_data(event);
    LV_ASSERT_NULL(instance);

    lv_obj_t* obj = lv_event_get_current_target(event);
    lv_event_code_t code = lv_event_get_code(event);

    if(code == LV_EVENT_CLICKED)
    {
        if(obj == instance->View.ui.find.txtarea)
        {
            instance->View.KeyboardAnimStart(false); //键盘弹出
        }
        else if(obj == instance->View.ui.find.button)
        {
            instance->View.KeyboardAnimStart(true); //键盘收起
            instance->Model.Search(instance->currentpath,lv_textarea_get_text(instance->View.ui.find.txtarea));
            instance->listchanged = true;
            instance->currentpath = NULL;
        }
        else if(obj == instance->View.ui.dri.sd)
        {
            if(instance->Model.SD_Info.isSelected != true)
            {
                onChangeDriFromTo(Flash,SD);
                instance->View.DriOptAnimStart(false);
                instance->listchanged = true;
                strcpy(instance->currentpath,instance->Model.SDCARD_PHOTO_PATH);
            }
        }
        else if(obj == instance->View.ui.dri.flash)
        {
            if(instance->Model.Flash_Info.isSelected != true)
            {
                onChangeDriFromTo(SD,Flash);
                instance->View.DriOptAnimStart(false);
                instance->listchanged = true;
                strcpy(instance->currentpath,instance->Model.FLASH_PHOTO_PATH);
            }
        }
        else if(obj == instance->View.ui.exit.exit)
        {
            instance->_Manager->Push("Pages/Camera");
        }
    }
}

void MemStore::onFileBtnClickEvent(lv_event_t* event)
{
    MemStore* instance = (MemStore*)lv_event_get_user_data(event);
    LV_ASSERT_NULL(instance);

    lv_obj_t* obj = lv_event_get_current_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    
    if(code == LV_EVENT_CLICKED)
    {
        char* path = new char[128];
        if(instance->Model.SD_Info.isSelected == true)
        {
            // strcpy(path,instance->currentpath);
            // strcat(path,"/");
            // strcat(path,lv_list_get_btn_text(instance->View.ui.filelist.flist, obj));
            sprintf(path,"%s%s%s",instance->currentpath,"/",lv_list_get_btn_text(instance->View.ui.filelist.flist, obj));
            instance->Model.SDcardFileOpen(path);
            if(instance->Model.SD_Info.ft != DataProc::T_DIR)
            {
                instance->Model.SendPathToPicturePresentPage(instance->Model.SD_Info.path);
                instance->_Manager->Push("Pages/PicturePresent");
            }
            else
            {
                strcpy(instance->currentpath,instance->Model.SD_Info.path);
            }
        }
        else if(instance->Model.Flash_Info.isSelected == true)
        {
            // strcpy(path,instance->currentpath);
            // strcat(path,"/");
            // strcat(path,lv_list_get_btn_text(instance->View.ui.filelist.flist, obj));
            sprintf(path,"%s%s%s",instance->currentpath,"/",lv_list_get_btn_text(instance->View.ui.filelist.flist, obj));
            instance->Model.FlashFileOpen(path);
            if(instance->Model.Flash_Info.ft != DataProc::T_DIR)
            {
                instance->Model.SendPathToPicturePresentPage(instance->Model.Flash_Info.path);
                instance->_Manager->Push("Pages/PicturePresent");
            }
            else
            {
                strcpy(instance->currentpath,instance->Model.Flash_Info.path);
            }
        }                
    }
    else if(code == LV_EVENT_LONG_PRESSED)
    {
        lv_obj_set_style_bg_color(obj,COLOR_GRAY,LV_STATE_CHECKED);
        //TODO:菜单
    }

}
