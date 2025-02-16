#include "StartupView.h"
#include <stdarg.h>
#include <stdio.h>

using namespace Page;

#define COLOR_PURPLE    lv_color_hex(0xa01eff)
#define COLOR_BROWN     lv_color_hex(0x804040)

void StartupView::Create(lv_obj_t* root)
{
    lv_obj_t* cont = lv_obj_create(root);
    lv_obj_remove_style_all(cont);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(cont, 110, 50);
    lv_obj_set_style_border_color(cont, COLOR_PURPLE, 0);
    lv_obj_set_style_border_side(cont, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(cont, 3, 0);
    lv_obj_set_style_border_post(cont, true, 0);
    lv_obj_center(cont);
    ui.cont = cont;

    lv_obj_t* title_cn = lv_label_create(root);
    lv_obj_set_style_text_font(title_cn, ResourcePool::GetFont("startup"), 0);
    lv_obj_set_style_text_color(title_cn, COLOR_BROWN, 0);
    lv_label_set_text(title_cn, "图像识别综合应用");
    lv_obj_set_size(title_cn,200, 30);
	lv_obj_align(title_cn, LV_ALIGN_CENTER,0,-50);
    ui.title_cn = title_cn;

    lv_obj_t * title_en = lv_label_create(root);             
	lv_obj_set_style_text_font(title_en, &lv_font_montserrat_8, 0);   
	lv_label_set_text(title_en, "PhotoAndDiscern");
    lv_obj_set_style_text_color(title_en, COLOR_BROWN, 0);
	//lv_obj_move_foreground(title_en);
    lv_obj_align_to(title_en,title_cn,LV_ALIGN_OUT_TOP_LEFT,0,-20);
    ui.title_en = title_en;

    ui.anim_timeline = lv_anim_timeline_create();

#define ANIM_DEF(start_time, obj, attr, start, end) \
     {start_time, obj, LV_ANIM_EXEC(attr), start, end, 500, lv_anim_path_ease_out, true}

    lv_anim_timeline_wrapper_t wrapper[] =
    {
        ANIM_DEF(0, ui.cont, width, 0, lv_obj_get_style_width(ui.cont, 0)),
        ANIM_DEF(500, ui.title_cn, y, lv_obj_get_style_height(ui.cont, 0), lv_obj_get_y(ui.title_cn)),
        ANIM_DEF(500, ui.title_en, y, lv_obj_get_style_height(ui.cont, 0), lv_obj_get_y(ui.title_en)),
        LV_ANIM_TIMELINE_WRAPPER_END
    };

    lv_anim_timeline_add_wrapper(ui.anim_timeline, wrapper);
}

void StartupView::Delete()
{
    if(ui.anim_timeline)
    {
        lv_anim_timeline_del(ui.anim_timeline);
        ui.anim_timeline = nullptr;
    }
}

