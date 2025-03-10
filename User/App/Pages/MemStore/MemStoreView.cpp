#include "MemStoreView.h"
#include <stdarg.h>
#include <stdio.h>

using namespace Page;

#define COLOR_PURPLE    lv_color_hex(0xa01eff)
#define COLOR_LIGHTPURPLE lv_color_hex(0xcca0ff)
#define COLOR_BROWN     lv_color_hex(0x804040)
#define COLOR_GRAY      lv_color_hex(0x808080)
#define COLOR_RED       lv_color_hex(0xa00000)
#define COLOR_GREEN     lv_color_hex(0x00a000)
#define COLOR_BlUE      lv_color_hex(0x0000a0)

#define ANIM_DEF(start_time, obj, attr, start, end) \
    {start_time, obj, LV_ANIM_EXEC(attr), start, end, 500, lv_anim_path_ease_out, true}
#define ANIM_OPA_DEF(start_time, obj) \
    ANIM_DEF(start_time, obj, opa_scale,LV_OPA_TRANSP,LV_OPA_COVER)

void MemStoreView::Create(lv_obj_t* root)
{
    FindCreat(root);
    DriCreat(root);
    FilelistCreat(root);
    VolumeCreat(root);
    ExitCreat(root);
    KeyboardCreat(root);

    ui.anim_timeline = lv_anim_timeline_create();

    lv_coord_t y_find = lv_obj_get_y(ui.find.cont);
    lv_coord_t y_dri = lv_obj_get_y(ui.dri.cont);   
    //lv_coord_t y_list = lv_obj_get_y(ui.filelist.cont);
    lv_coord_t y_exit = lv_obj_get_y(ui.exit.cont);
    lv_coord_t y_volume = lv_obj_get_y(ui.volume.cont);

    lv_anim_timeline_wrapper_t wrapper[] =
    {
        ANIM_DEF(0, ui.find.cont, y, LV_VER_RES+lv_obj_get_height(ui.find.cont), y_find),

        //ANIM_DEF(200, ui.filelist.cont, y, LV_VER_RES+lv_obj_get_height(ui.filelist.cont), y_list),
        //ANIM_OPA_DEF(200, ui.filelist.cont),
        ANIM_DEF(200, ui.dri.cont, y, LV_VER_RES+lv_obj_get_height(ui.dri.cont), y_dri),

        ANIM_DEF(500, ui.exit.cont, y, LV_VER_RES+lv_obj_get_height(ui.exit.cont), y_exit),
        ANIM_DEF(700, ui.volume.cont, y, LV_VER_RES+lv_obj_get_height(ui.volume.cont), y_volume),
        LV_ANIM_TIMELINE_WRAPPER_END
    };

    lv_anim_timeline_add_wrapper(ui.anim_timeline, wrapper);
}

void MemStoreView::Delete()
{
    if(ui.anim_timeline)
    {
        lv_anim_timeline_del(ui.anim_timeline);
        ui.anim_timeline = nullptr;
    }
    if(ui.dri.dri_anim)
    {
        lv_anim_timeline_del(ui.dri.dri_anim);
        ui.dri.dri_anim = nullptr;
    }
}

void MemStoreView::FindCreat(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont,LV_HOR_RES, 34);
    lv_obj_align(cont,LV_ALIGN_TOP_MID,0,1);
    ui.find.cont = cont;

    lv_obj_t* txtarea = lv_textarea_create(cont);
    lv_textarea_set_max_length(txtarea,8); //设置字符长度最大为8
    lv_textarea_set_one_line(txtarea,true); //单行模式
    lv_obj_set_size(txtarea,200,30);
    lv_obj_align_to(txtarea,cont,LV_ALIGN_LEFT_MID,0,0);
    lv_obj_clear_flag(txtarea, LV_OBJ_FLAG_SCROLLABLE); //禁止滚动
    lv_obj_set_scroll_dir(txtarea, LV_DIR_HOR);
	  lv_obj_set_style_text_font(txtarea, ResourcePool::GetFont("ta"), 0); //设置为默认字库
    lv_textarea_set_placeholder_text(txtarea, "查找文件或目录"); //占位符
    lv_obj_set_style_text_color(txtarea,COLOR_LIGHTPURPLE,LV_PART_TEXTAREA_PLACEHOLDER);//设置占位符颜色
    lv_obj_set_style_anim_time(txtarea, 1000, LV_STATE_FOCUSED | LV_PART_CURSOR);//光标闪烁时间为1000ms
    //lv_textarea_set_cursor_click_pos(txtarea, true); //光标跟随触摸，默认已选择
    lv_textarea_set_text_selection(txtarea, true); //文本选择开
    lv_obj_set_style_bg_color(lv_textarea_get_label(txtarea), COLOR_LIGHTPURPLE, LV_PART_SELECTED); //选择文本时改变颜色
    lv_obj_set_style_border_color(txtarea,COLOR_PURPLE,LV_PART_MAIN|LV_STATE_FOCUSED);//聚焦时改变文本框颜色
    ui.find.txtarea = txtarea;

    lv_obj_t* button = lv_obj_create(cont);
    lv_obj_set_size(button,45,30);
    lv_obj_set_style_radius(button,0,0);
    lv_obj_set_style_border_color(button,COLOR_PURPLE,0);
    lv_obj_set_style_bg_color(button,COLOR_PURPLE,0);  
    lv_obj_align_to(button,txtarea,LV_ALIGN_OUT_RIGHT_MID,-10,0);
    lv_obj_clear_flag(button, LV_OBJ_FLAG_SCROLLABLE); //禁止滚动
    ui.find.button = button;

    lv_obj_t* btnlabel = lv_label_create(button);
    lv_label_set_text(btnlabel,LV_SYMBOL_NEW_LINE);
    lv_obj_set_style_text_color(btnlabel,lv_color_white(),0);
    lv_obj_align_to(btnlabel,button,LV_ALIGN_CENTER,0,0);
    //lv_obj_add_flag(btnlabel,LV_OBJ_FLAG_EVENT_BUBBLE);
    ui.find.btnlabel = btnlabel;
}

void MemStoreView::KeyboardCreat(lv_obj_t* par)
{
    lv_obj_t* imepy = lv_ime_pinyin_create(par);
	  lv_obj_set_style_text_font(imepy, ResourcePool::GetFont("ta"), 0); 
    
    lv_obj_t* kb = lv_keyboard_create(par);
    lv_keyboard_set_textarea(kb, ui.find.txtarea);
    lv_ime_pinyin_set_keyboard(imepy,kb);
    lv_keyboard_set_mode(imepy,LV_IME_PINYIN_MODE_K9);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    ui.kb.kb = kb;
    ui.kb.imepy = imepy;

    ui.kb.kb_anim = lv_anim_timeline_create();

    lv_coord_t y_kb = lv_obj_get_y(kb);

    lv_anim_timeline_wrapper_t wrapper[] =
    {
        ANIM_DEF(0, ui.kb.kb, y, LV_VER_RES+lv_obj_get_height(ui.kb.kb), y_kb),
        LV_ANIM_TIMELINE_WRAPPER_END
    };
    
    lv_anim_timeline_add_wrapper(ui.kb.kb_anim, wrapper);
}

void MemStoreView::DriCreat(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont,45, 180);
    lv_obj_align(cont,LV_ALIGN_DEFAULT,0,35);
    ui.dri.cont = cont;

    lv_obj_t* opt = lv_obj_create(cont);
    lv_obj_set_size(opt,45,90);
    lv_obj_set_style_radius(opt,15,0);
    lv_obj_set_style_border_color(opt,COLOR_PURPLE,0);//内框颜色
    lv_obj_set_style_border_width(opt,2,0);//内框厚度
    lv_obj_set_style_bg_opa(opt,0,0);
    lv_obj_align_to(opt,cont,LV_ALIGN_TOP_MID,0,0);
    ui.dri.opt = opt;

    lv_obj_t* sd = lv_label_create(cont);
    lv_obj_set_size(sd,45,20);
    lv_obj_set_style_text_font(sd, ResourcePool::GetFont("driopt"), 0);
    lv_obj_set_style_text_color(sd,COLOR_BROWN,0);  
    lv_obj_set_style_text_align(sd,LV_TEXT_ALIGN_CENTER,0);
    lv_label_set_text(sd,"SD卡");
    lv_obj_align_to(sd,cont,LV_ALIGN_TOP_MID,0,35);
    ui.dri.sd = sd;

    lv_obj_t* flash = lv_label_create(cont);
    lv_obj_set_size(flash,45,20);
    lv_obj_set_style_text_font(flash, ResourcePool::GetFont("driopt"), 0);
    lv_obj_set_style_text_color(flash,COLOR_BROWN,0); 
    lv_obj_set_style_text_align(sd,LV_TEXT_ALIGN_CENTER,0);
    lv_label_set_text(flash,"FLASH");
    lv_obj_align_to(flash,cont,LV_ALIGN_TOP_MID,0,125);
    ui.dri.flash = flash;

    ui.dri.dri_anim = lv_anim_timeline_create();
}

void MemStoreView::FilelistCreat(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_clear_flag(cont,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(cont,LV_HOR_RES-45, 315);
    lv_obj_align(cont,LV_ALIGN_DEFAULT,35,45);
    ui.volume.cont = cont;

    lv_obj_t* flist = lv_list_create(cont);
    lv_obj_set_size(flist,LV_HOR_RES-45,315); 
    lv_obj_set_scroll_dir(flist,LV_DIR_VER);  
}

void MemStoreView::VolumeCreat(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont,LV_HOR_RES-45, 41);
    lv_obj_align(cont,LV_ALIGN_BOTTOM_RIGHT,0,-8);
    ui.volume.cont = cont;

    //总容量label
    cont = lv_obj_create(ui.volume.cont);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont,LV_HOR_RES-45, 20);
    lv_obj_align_to(cont,ui.volume.cont,LV_ALIGN_TOP_LEFT,0,0);
    ui.volume.total.cont = cont;

    lv_obj_t* label = lv_label_create(ui.volume.total.cont);
    lv_obj_set_size(label,45,20);
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("volume"), 0);
    lv_obj_set_style_text_color(label,COLOR_BROWN,0);
    lv_obj_set_style_text_align(label,LV_TEXT_ALIGN_LEFT,0);
    lv_label_set_text(label,"总容量");
    lv_obj_align_to(label,ui.volume.total.cont,LV_ALIGN_LEFT_MID,0,0);
    ui.volume.total.total = label;

    label = lv_label_create(ui.volume.total.cont);
    lv_obj_set_size(label,64,20);
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("volume"), 0);
    lv_obj_set_style_text_color(label,COLOR_BlUE,0);
    lv_obj_set_style_text_align(label,LV_TEXT_ALIGN_RIGHT,0);
    lv_label_set_text(label,"0");
    lv_obj_align_to(label,ui.volume.total.total,LV_ALIGN_RIGHT_MID,0,0);
    ui.volume.total.value = label;

    label = lv_label_create(ui.volume.total.cont);
    lv_obj_set_size(label,25,20);
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("volume"), 0);
    lv_obj_set_style_text_color(label,COLOR_BROWN,0);
    lv_obj_set_style_text_align(label,LV_TEXT_ALIGN_RIGHT,0);
    lv_label_set_text(label,"MB");
    lv_obj_align_to(label,ui.volume.total.value,LV_ALIGN_RIGHT_MID,0,0);
    ui.volume.free.unit = label;

    //剩余容量label
    cont = lv_obj_create(ui.volume.cont);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont,LV_HOR_RES-45, 20);
    lv_obj_align_to(cont,ui.volume.cont,LV_ALIGN_BOTTOM_LEFT,0,0);
    ui.volume.free.cont = cont;

    label = lv_label_create(ui.volume.free.cont);
    lv_obj_set_size(label,45,20);
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("volume"), 0);
    lv_obj_set_style_text_color(label,COLOR_BROWN,0);
    lv_obj_set_style_text_align(label,LV_TEXT_ALIGN_LEFT,0);
    lv_label_set_text(label,"剩余");
    lv_obj_align_to(label,ui.volume.free.cont,LV_ALIGN_LEFT_MID,0,0);
    ui.volume.free.free = label;

    label = lv_label_create(ui.volume.free.cont);
    lv_obj_set_size(label,64,20);
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("volume"), 0);
    lv_obj_set_style_text_color(label,COLOR_GREEN,0);
    lv_obj_set_style_text_align(label,LV_TEXT_ALIGN_RIGHT,0);
    lv_label_set_text(label,"0");
    lv_obj_align_to(label,ui.volume.free.free,LV_ALIGN_RIGHT_MID,0,0);
    ui.volume.free.value = label;

    label = lv_label_create(ui.volume.free.cont);
    lv_obj_set_size(label,25,20);
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("volume"), 0);
    lv_obj_set_style_text_color(label,COLOR_BROWN,0);
    lv_obj_set_style_text_align(label,LV_TEXT_ALIGN_RIGHT,0);
    lv_label_set_text(label,"MB");
    lv_obj_align_to(label,ui.volume.free.value,LV_ALIGN_RIGHT_MID,0,0);
    ui.volume.free.unit = label;
}

void MemStoreView::ExitCreat(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont,45, 40);
    lv_obj_align(cont,LV_ALIGN_BOTTOM_LEFT,0,-10);
    ui.exit.cont = cont;

    lv_obj_t* exit = lv_obj_create(cont);
    lv_obj_remove_style_all(exit);
    lv_obj_set_size(exit,40, 40);
    lv_obj_align_to(exit,cont,LV_ALIGN_CENTER,0,0);
    ui.exit.exit = exit;

    lv_obj_t* icon = lv_label_create(exit);
    lv_label_set_text(icon,LV_SYMBOL_LEFT);
    lv_obj_set_style_text_color(icon,lv_color_black(),0);
    lv_obj_align_to(icon,exit,LV_ALIGN_CENTER,0,0);
    ui.exit.icon = icon;
}

void MemStoreView::DriOptAnimStart(bool reverse)
{
    lv_coord_t y_start = lv_obj_get_y(ui.dri.opt);
    lv_coord_t y_end = y_start > 35 ? 35 : 125;

    lv_anim_timeline_wrapper_t wrapper[] =
    {
        ANIM_DEF(0, ui.dri.opt, y, y_start, y_end),
        LV_ANIM_TIMELINE_WRAPPER_END
    };

    lv_anim_timeline_add_wrapper(ui.dri.dri_anim, wrapper);
    lv_anim_timeline_set_reverse(ui.dri.dri_anim, reverse);
    lv_anim_timeline_start(ui.dri.dri_anim);
}

void MemStoreView::KeyboardAnimStart(bool reverse)
{    
    lv_obj_clear_flag(ui.kb.kb, LV_OBJ_FLAG_HIDDEN);
    lv_anim_timeline_set_reverse(ui.kb.kb_anim, reverse);
    lv_anim_timeline_start(ui.kb.kb_anim);
    if(reverse) lv_obj_add_flag(ui.kb.kb, LV_OBJ_FLAG_HIDDEN);
}

void MemStoreView::AppearAnimStart(bool reverse)
{
    lv_anim_timeline_set_reverse(ui.anim_timeline, reverse);
    lv_anim_timeline_start(ui.anim_timeline);
}

void MemStoreView::AddBtnToFileList(uint32_t nbtn, const char* pbtntxt, lv_event_cb_t btnclick_event_cb)
{
    lv_obj_t * btn;
    /* lv_list_add_btn 函数会生成btn */
    //lv_obj_set_size(btn,LV_HOR_RES-45,45);
    //lv_obj_set_style_text_align(btn,LV_TEXT_ALIGN_LEFT,0);  

    while(nbtn--)
    {
        //btn = lv_list_add_btn(flist, LV_SYMBOL_DUMMY, *pbtntxt); //LV_SYMBOL is used;
        btn = lv_list_add_btn(ui.filelist.flist, NULL, pbtntxt);
        lv_obj_add_event_cb(btn, btnclick_event_cb, LV_EVENT_CLICKED, NULL);

    }
}

void MemStoreView::ClearFileList()
{
    // lv_obj_del(ui.filelist.flist);
    // lv_obj_t* flist = lv_list_create(ui.filelist.cont);
    // lv_obj_set_size(flist,LV_HOR_RES-45,315); 
    // lv_obj_set_scroll_dir(flist,LV_DIR_VER);
    // ui.filelist.flist = flist;
    return lv_obj_clean(ui.filelist.flist);
}

