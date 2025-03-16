#include "CameraView.h"
#include <stdarg.h>
#include <stdio.h>

using namespace Page;

#define ANIM_DEF(start_time, obj, attr, start, end) \
    {start_time, obj, LV_ANIM_EXEC(attr), start, end, 200, lv_anim_path_ease_out, true}
#define ANIM2_DEF(start_time, obj, attr, start, end) \
    {start_time, obj, LV_ANIM_EXEC(attr), start, end, 400, lv_anim_path_ease_out, true}
#define ANIM_OPA_DEF(start_time, obj) \
    ANIM_DEF(start_time, obj, opa_scale, LV_OPA_COVER,LV_OPA_TRANSP)

#define COLOR_PURPLE    lv_color_hex(0xa01eff)

void CameraView::Create(lv_obj_t* root)
{
    ShutterCreat(root);
    OptlabelCreat(root);
    StorebtnCreat(root);
}

void CameraView::ShutterCreat(lv_obj_t* par)
{
    lv_obj_t* shutterex = lv_obj_create(par);
    lv_obj_set_size(shutterex,50, 50);
    lv_obj_set_style_radius(shutterex,25,0);  //设置圆角长，取一半则为圆
    lv_obj_set_style_border_color(shutterex,lv_color_white(),0);//内框颜色
    lv_obj_set_style_border_width(shutterex,4,0);//内框厚度
    //lv_obj_set_style_pad_ver(shutterex,0,0);
    lv_obj_clear_flag(shutterex, LV_OBJ_FLAG_SCROLLABLE); //禁止滚动
    lv_obj_set_style_bg_opa(shutterex,0,0);//背景透明度
    lv_obj_align(shutterex, LV_ALIGN_CENTER, 0, 155); 
    ui.shutter.shutterex = shutterex;

    lv_obj_t* shutterin = lv_obj_create(shutterex);
    lv_obj_set_size(shutterex,36, 36);
    lv_obj_set_style_radius(shutterex,18,0);  //设置圆角长，取一半则为圆
    lv_obj_set_style_border_color(shutterin,COLOR_PURPLE,0);
    lv_obj_set_style_border_width(shutterin,2,0);
    lv_obj_align_to(shutterin,shutterex,LV_ALIGN_CENTER,0,0);
    ui.shutter.shutterin = shutterin;

}

void CameraView::OptlabelCreat(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont,35,40);
    // lv_obj_set_style_bg_opa(cont,0,0);//背景透明度
    // lv_obj_set_style_pad_ver(cont,0,0);
    lv_obj_align_to(cont,ui.shutter.shutterex,LV_ALIGN_OUT_RIGHT_MID,20,0);
    ui.optlabel.cont=cont;

    lv_obj_t* opt = lv_obj_create(cont);
    lv_obj_set_size(opt,36,16);
    lv_obj_set_style_radius(opt,5,0); //设置圆角长
    lv_obj_set_style_bg_opa(opt,0,0);//背景透明度
    //lv_obj_set_style_pad_ver(opt,0,0);
    lv_obj_set_style_border_color(opt,COLOR_PURPLE,0);//内框颜色
    lv_obj_set_style_border_width(opt,1,0);//内框厚度
    lv_obj_align_to(opt,cont,LV_ALIGN_TOP_MID,0,1);
    ui.optlabel.opt=opt;

    lv_obj_t* ean13 = lv_label_create(cont);
    lv_obj_set_size(ean13,30,14);
    lv_obj_set_style_text_font(ean13, ResourcePool::GetFont("optlabel"), 0);
    lv_obj_set_style_text_color(ean13,lv_color_white(),0);
    lv_label_set_text(ean13,"条形码");
    lv_obj_align_to(ean13,cont,LV_ALIGN_TOP_MID,0,1);
    ui.optlabel.ean13=ean13;
   
    lv_obj_t* chara = lv_label_create(cont);
    lv_obj_set_size(chara,30,14);
    lv_obj_set_style_text_font(chara, ResourcePool::GetFont("optlabel"), 0);
    lv_obj_set_style_text_color(chara,lv_color_white(),0);
    lv_label_set_text(chara,"字符");
    lv_obj_align_to(chara,cont,LV_ALIGN_BOTTOM_MID,0,-1);
    ui.optlabel.chara=chara;

    ui.optlabel.label_anim = lv_anim_timeline_create();

}

void CameraView::StorebtnCreat(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont,24,24);
    lv_obj_align(cont,LV_ALIGN_BOTTOM_LEFT,25,-16);
    ui.storebtn.cont=cont;

    lv_obj_t* btn = lv_obj_create(cont);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn,24, 24);
    lv_obj_align_to(btn,cont,LV_ALIGN_CENTER,0,0);
    ui.storebtn.btn = btn;

    lv_obj_t* icon = lv_label_create(btn);
    lv_label_set_text(icon,LV_SYMBOL_LIST);
    lv_obj_set_style_text_color(icon,lv_color_white(),0);
    lv_obj_align_to(icon,btn,LV_ALIGN_CENTER,0,0);
    ui.storebtn.icon = icon;    
}

void CameraView::Delete()
{
    if(ui.anim_timeline)
    {
        lv_anim_timeline_del(ui.anim_timeline);
        ui.anim_timeline = NULL;
    }
    if(ui.optlabel.label_anim)
    {
        lv_anim_timeline_del(ui.optlabel.label_anim);
        ui.optlabel.label_anim = NULL;
    }
}

void CameraView::AppearAnimStart(bool reverse)
{
    lv_anim_timeline_set_reverse(ui.anim_timeline,reverse);
    lv_anim_timeline_start(ui.anim_timeline);
}

void CameraView::OptlabelAnimStart(bool reverse)
{
    lv_coord_t y_chara = lv_obj_get_y(ui.optlabel.chara);
    lv_coord_t y_ean13 = lv_obj_get_y(ui.optlabel.ean13);
    lv_coord_t y_top = (y_chara < y_ean13)? y_chara : y_ean13;
    lv_coord_t y_bottom = (y_chara > y_ean13)? y_chara : y_ean13;
    lv_obj_t* top = (y_chara < y_ean13)? ui.optlabel.chara : ui.optlabel.ean13;
    lv_obj_t* bottom = (y_chara > y_ean13)? ui.optlabel.chara : ui.optlabel.ean13;

    lv_anim_timeline_wrapper_t wrapper[] =
    {
        ANIM_DEF(0, top, y, y_top, y_top+y_top-y_bottom),
        ANIM_OPA_DEF(0, top),
        ANIM2_DEF(0, bottom, y, y_bottom, y_top),
        ANIM_DEF(200, top, y, y_bottom+y_bottom-y_top, y_bottom),
        LV_ANIM_TIMELINE_WRAPPER_END
    };

    lv_anim_timeline_add_wrapper(ui.optlabel.label_anim, wrapper);
    lv_anim_timeline_set_reverse(ui.optlabel.label_anim,reverse);
    lv_anim_timeline_start(ui.optlabel.label_anim);
}


