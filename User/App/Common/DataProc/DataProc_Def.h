#ifndef __DATA_PROC_DEF_H
#define __DATA_PROC_DEF_H

#include <stdint.h>

namespace DataProc
{

/* Beep */
typedef enum
{
    BEEP_Is_Used,
    BEEP_Not_Used,
} Beep_Cmd_t;

typedef struct
{
    Beep_Cmd_t cmd;
    uint32_t tim;
} Beep_Info_t;

/* Shutter */
typedef enum
{
    EAN13_OPT,
    CHARA_OPT,
} Shutter_Opt_t;

typedef struct 
{
    Shutter_Opt_t opt;
    bool photo;
} Shutter_Info_t;

/* File */
typedef enum
{
    T_BIN = 0X00,   //二进制文件
    T_LRC = 0X10,	//lrc文件
    T_NES = 0X20,	//nes文件

    T_TEXT = 0X30,	//.txt文件
    T_C	 = 0X31,	//.c文件
    T_H	 = 0X32,    //.h文件

    T_WAV = 0X40,	//WAV文件
    T_MP3 = 0X41,	//MP3文件 
    T_APE = 0X42,	//APE文件
    T_FLAC = 0X43,	//FLAC文件

    T_BMP = 0X50,	//bmp文件
    T_JPG = 0X51,	//jpg文件
    T_JPEG = 0X52,	//jpeg文件		 
    T_GIF = 0X53,	//gif文件   

    T_AVI = 0X60,	//avi文件


    T_DIR = 0xff,   //目录
} Filetype;


typedef enum
{
    STORE_CMD_OPEN,
    STORE_CMD_DIR_READ,
} Store_Cmd_t;


/* SDcard */
typedef struct 
{
    bool isReady;
    uint32_t totalSizeMB;
    uint32_t freeSizeMB;
    //const char* type;
} SDcard_Basic_Info_t;

typedef struct 
{ 
    bool isSelected;
    SDcard_Basic_Info_t b_info;
    Filetype ft;
    const char* path;
    Store_Cmd_t cmd;
    void* data;
    uint32_t size;
} SDcard_Info_t;


/* Flash */
typedef struct 
{
    bool isReady;
    uint32_t totalSizeKB;
    uint32_t freeSizeKB;
} Flash_Basic_Info_t;

typedef struct 
{ 
    bool isSelected;
    Flash_Basic_Info_t b_info;
    Filetype ft;
    const char* path;
    Store_Cmd_t cmd;
    void* data;
    uint32_t size;
} Flash_Info_t;

// /* MemStore */
// typedef enum
// {
//     SDCARD_IS_SELECTED,
//     FLASH_IS_SELECTED,
// } MemStore_Select_t;
// typedef struct 
// {
//     MemStore_Select_t select;
// } MemStore_Info_t;

/* Search */
typedef struct 
{   
    const char* dir;
    const char* namesearch;
    char* pfilename;
    uint32_t npfilename;
} Search_Info_t;

/* PicturePresent */
typedef struct 
{   
    char photopath[128];
} PicturePresent_Info_t;

/* Recorder */
typedef enum
{
    RECORDER_CMD_START,
    RECORDER_CMD_PAUSE,
    RECORDER_CMD_CONTINUE,
    RECORDER_CMD_STOP,
} Recorder_Cmd_t;

typedef struct
{
    Recorder_Cmd_t cmd;
    uint16_t time;
} Recorder_Info_t;

/* Storage */
typedef enum
{
    STORAGE_CMD_LOAD,
    STORAGE_CMD_SAVE,
    STORAGE_CMD_ADD,
    STORAGE_CMD_REMOVE
} Storage_Cmd_t;

typedef enum
{
    STORAGE_TYPE_UNKNOW,
    STORAGE_TYPE_INT,
    STORAGE_TYPE_FLOAT,
    STORAGE_TYPE_DOUBLE,
    STORAGE_TYPE_STRING
} Storage_Type_t;

typedef struct
{
    Storage_Cmd_t cmd;
    const char* key;
    void* value;
    uint16_t size;
    Storage_Type_t type;
} Storage_Info_t;

#define STORAGE_VALUE_REG(act, data, dataType)\
do{\
    DataProc::Storage_Info_t info; \
    DATA_PROC_INIT_STRUCT(info); \
    info.cmd = DataProc::STORAGE_CMD_ADD; \
    info.key = #data; \
    info.value = &data; \
    info.size = sizeof(data); \
    info.type = dataType; \
    act->Notify("Storage", &info, sizeof(info)); \
}while(0)

typedef struct
{
    bool isDetect;
    float totalSizeMB;
    float freeSizeMB;
    const char* type;
} Storage_Basic_Info_t;

/* StatusBar */
typedef enum
{
    STATUS_BAR_STYLE_TRANSP,
    STATUS_BAR_STYLE_BLACK,
} StatusBar_Style_t;

typedef enum
{
    STATUS_BAR_CMD_APPEAR,
    STATUS_BAR_CMD_SET_STYLE,
    STATUS_BAR_CMD_SET_LABEL_REC
} StatusBar_Cmd_t;

typedef struct
{
    StatusBar_Cmd_t cmd;
    union
    {
        bool appear;
        StatusBar_Style_t style;
        struct
        {
            bool show;
            const char* str;
        } labelRec;
    } param;
} StatusBar_Info_t;

/* MusicPlayer */
typedef struct
{
    const char* music;
} MusicPlayer_Info_t;

/* SysConfig */
typedef enum
{
    SYSCONFIG_CMD_LOAD,
    SYSCONFIG_CMD_SAVE,
} SysConfig_Cmd_t;

typedef struct
{
    SysConfig_Cmd_t cmd;
    float longitude;
    float latitude;
    int16_t timeZone;
    bool soundEnable;
    char language[8];
    char arrowTheme[16];
    char mapDirPath[16];
    char mapExtName[8];
    bool mapWGS84;
} SysConfig_Info_t;

/* TrackFilter */
typedef enum
{
    TRACK_FILTER_CMD_START = RECORDER_CMD_START,
    TRACK_FILTER_CMD_PAUSE = RECORDER_CMD_PAUSE,
    TRACK_FILTER_CMD_CONTINUE = RECORDER_CMD_CONTINUE,
    TRACK_FILTER_CMD_STOP = RECORDER_CMD_STOP,
} TrackFilter_Cmd_t;

typedef struct
{
    TrackFilter_Cmd_t cmd;
    void* pointCont;
    uint8_t level;
    bool isActive;
} TrackFilter_Info_t;

}

#endif
