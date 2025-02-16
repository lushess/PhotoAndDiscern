/*******************************************************************************
 * Size: 10 px
 * Bpp: 2
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef FONT_OPTLABEL
#define FONT_OPTLABEL 1
#endif

#if FONT_OPTLABEL

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+5B57 "字" */
    0x0, 0x40, 0x2, 0xaf, 0xa8, 0x65, 0x55, 0xc5,
    0x6a, 0x98, 0x0, 0x2c, 0x0, 0xa, 0x0, 0x7f,
    0xff, 0xc0, 0x9, 0x0, 0x0, 0x90, 0x0, 0x3c,
    0x0,

    /* U+5F62 "形" */
    0x0, 0x0, 0x47, 0xfe, 0x28, 0x18, 0xce, 0x1,
    0x8c, 0x8, 0x7f, 0xe2, 0x81, 0x8c, 0xe0, 0x14,
    0xc0, 0x92, 0x4c, 0x1c, 0x30, 0xcb, 0x5, 0xd,
    0x80, 0x0, 0x0, 0x0,

    /* U+6761 "条" */
    0x0, 0x40, 0x0, 0x3e, 0xa0, 0x1f, 0x4e, 0x2,
    0xf, 0x80, 0x6, 0xee, 0x42, 0x81, 0x19, 0xf,
    0xff, 0x80, 0x16, 0x10, 0x1c, 0x62, 0x41, 0xd,
    0x4,

    /* U+7801 "码" */
    0x7f, 0x7f, 0x81, 0x41, 0x14, 0x30, 0x22, 0x43,
    0xe6, 0x24, 0xb2, 0x7f, 0xd7, 0x20, 0x9, 0x32,
    0xfe, 0xc3, 0xe0, 0xc, 0x0, 0x7, 0x80, 0x0,
    0x0,

    /* U+7B26 "符" */
    0x4, 0x0, 0x2, 0xea, 0xe8, 0x36, 0x66, 0x1,
    0x24, 0x20, 0xd, 0x3, 0x2, 0xda, 0xb8, 0x7c,
    0x83, 0x0, 0xc6, 0x30, 0xc, 0x3, 0x0, 0xc1,
    0xe0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 160, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 25, .adv_w = 160, .box_w = 10, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 53, .adv_w = 160, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 78, .adv_w = 160, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 103, .adv_w = 160, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x40b, 0xc0a, 0x1caa, 0x1fcf
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 23383, .range_length = 8144, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 5, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 2,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t font_optlabel = {
#else
lv_font_t font_optlabel = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 11,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if FONT_OPTLABEL*/

