#ifndef __MEMSTORE_VIEW_H
#define __MEMSTORE_VIEW_H

#include "../Page.h"

namespace Page
{

class MemStoreView
{
public:
    void Create(lv_obj_t* root);
    void Delete();
    void AppearAnimStart(bool reverse = false);
    void DriOptAnimStart(bool reverse = false);
    void KeyboardAnimStart(bool reverse = false);
    void AddBtnToFileList(uint32_t nbtn, const char** pbtntxt, lv_event_cb_t btnclick_event_cb);
    void ClearFileList();
public:   
    struct
    {
        struct 
        {
            lv_obj_t* txtarea;
            lv_obj_t* button;
            lv_obj_t* btnlabel;
            lv_obj_t* cont;
        }find;

        struct 
        {
            lv_obj_t* sd;
            lv_obj_t* flash;
            lv_obj_t* opt;
            lv_obj_t* cont;
            lv_anim_timeline_t* dri_anim;
        }dri;

        struct 
        {
            struct 
            {
                lv_obj_t* total;
                lv_obj_t* value;
                lv_obj_t* unit;
                lv_obj_t* cont;
            }total;
            
            struct 
            {
                lv_obj_t* free;
                lv_obj_t* value;
                lv_obj_t* unit;
                lv_obj_t* cont;
            }free;

            lv_obj_t* cont;
        }volume;

        struct 
        {
            lv_obj_t* kb;
            lv_obj_t* imepy;
            lv_anim_timeline_t* kb_anim;
        }kb;

        struct 
        {
            lv_obj_t* flist;
            lv_obj_t* cont;
        }filelist;

        struct 
        {
            lv_obj_t* exit;  
            lv_obj_t* icon; 
            lv_obj_t* cont;
        }exit;
        
        lv_anim_timeline_t* anim_timeline;      
    } ui;

private:
    void FindCreat(lv_obj_t* par);
    void KeyboardCreat(lv_obj_t* par);
    void DriCreat(lv_obj_t* par);
    void FilelistCreat(lv_obj_t* par);
    void VolumeCreat(lv_obj_t* par); 
    void ExitCreat(lv_obj_t* par);
private:
    

};

}

#endif // !__MEMSTORE_VIEW_H
