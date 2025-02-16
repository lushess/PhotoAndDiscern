/**
 * @file lv_port_fs_Fatfs.c
 *
 */

 /*Copy this file as "lv_port_fs.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_fs_Fatfs.h"
#include <stdbool.h>
#include <string.h>
#include "lvgl/lvgl.h"
#include "FATFS/FATFS_APP/fatfs_app.h"
/*********************
 *      DEFINES
 *********************/
#define SDCARD_LETTER  '0'
#define FLASH_LETTER   '1'
/**********************
 *      TYPEDEFS
 **********************/
typedef unsigned char filemode_t;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);
static bool sdcard_ready(lv_fs_drv_t * drv);
static bool flash_ready(lv_fs_drv_t * drv);
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_size(lv_fs_drv_t * drv, void * file_p, uint32_t * size_p);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

static void * fs_dir_open(lv_fs_drv_t * drv, const char *path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * rddir_p, char * fn);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * rddir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
extern FATFS *fs[_VOLUMES];  
extern FIL *file;	 
extern FIL *ftemp;	 
extern UINT br,bw;
extern FILINFO *fileinfo;
extern DIR *dir;
extern u8 fs_ready_status[_VOLUMES];
extern u8 *fatbuf;//数据缓存区,1024byte
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_fs_init(void)
{
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    /*Add a simple drive to open images*/
    static lv_fs_drv_t sd_drv;
	  static lv_fs_drv_t flash_drv;
    lv_fs_drv_init(&sd_drv);

    /*SDCard Set up fields...*/
    sd_drv.letter = SDCARD_LETTER;
	  sd_drv.ready_cb = sdcard_ready;
    sd_drv.open_cb = fs_open;
    sd_drv.close_cb = fs_close;
    sd_drv.read_cb = fs_read;
    sd_drv.write_cb = fs_write;
    sd_drv.seek_cb = fs_seek;
    sd_drv.tell_cb = fs_tell;

    sd_drv.dir_close_cb = fs_dir_close;
    sd_drv.dir_open_cb = fs_dir_open;
    sd_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&sd_drv);
		
		/*Flash Set up fields...*/
		flash_drv.letter = FLASH_LETTER;
	  flash_drv.ready_cb = flash_ready;
    flash_drv.open_cb = fs_open;
    flash_drv.close_cb = fs_close;
    flash_drv.read_cb = fs_read;
    flash_drv.write_cb = fs_write;
    flash_drv.seek_cb = fs_seek;
    flash_drv.tell_cb = fs_tell;

    flash_drv.dir_close_cb = fs_dir_close;
    flash_drv.dir_open_cb = fs_dir_open;
    flash_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&flash_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
/*Initialize your Storage device and File system.*/
static void fs_init(void)
{
	 u8 res;
	 res = lv_FATFS_Init(fs,fs_ready_status);
	 //FS_ASSERT(res == 0);
	//res!=0，弹出错误信息
}

static bool sdcard_ready(lv_fs_drv_t * drv)
{
	 if(fs_ready_status[0] == 0) return true;			
   else return false;
}

static bool flash_ready(lv_fs_drv_t * drv)
{
	 if(fs_ready_status[1] == 0) return true;			
   else return false;
}

/**
 * Open a file
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return          a file descriptor or NULL on error
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
	  filemode_t fmode;
	  FRESULT res;
	
    if(mode == LV_FS_MODE_WR)
    {
        /*Open a file for write*/
        fmode = FA_OPEN_ALWAYS|FA_WRITE;       
    }
    else if(mode == LV_FS_MODE_RD)
    {
        /*Open a file for read*/
        fmode = FA_OPEN_EXISTING|FA_READ;
    }
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
    {
        /*Open a file for read and write*/
        fmode = FA_OPEN_ALWAYS|FA_READ|FA_WRITE;
    }
    res = f_open(file,path,fmode);
		if(res) return NULL;
		else return file;
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    lv_fs_res_t res = f_close(file_p)? LV_FS_RES_FS_ERR : LV_FS_RES_OK;
	
    return res;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
	  UINT byteread;
    lv_fs_res_t res = f_read(file_p,buf,btr,&byteread)? LV_FS_RES_FS_ERR : LV_FS_RES_OK;
	
    *br=byteread;
	
    return res;
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btr       Bytes To Write
 * @param br        the number of real written bytes (Bytes Written). NULL if unused.
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
	  UINT bytewrote;
    lv_fs_res_t res = f_write(file_p,buf,btw,&bytewrote)? LV_FS_RES_FS_ERR : LV_FS_RES_OK;

    *bw = bytewrote;

    return res;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open )
 * @param pos       the new position of read write pointer
 * @param whence    tells from where to interpret the `pos`. See @lv_fs_whence_t
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
	  FRESULT res;
	
    if(whence == LV_FS_SEEK_SET)
    {
        res = f_lseek(file_p,pos);
    }
    else if(whence == LV_FS_SEEK_CUR)
    {
        res = f_lseek(file_p,f_tell((FIL *)file_p)+pos); //跳到当前位置+pos位
    }
    else if(whence == LV_FS_SEEK_END)
    {
        res = f_lseek(file_p,f_size((FIL *)file_p));
    }
    else
    {
        return LV_FS_RES_UNKNOWN;
    }
    
		if(res != FR_OK) return LV_FS_RES_FS_ERR;
    else return LV_FS_RES_OK;
}
/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param pos_p     pointer to to store the result
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    *pos_p = f_tell((FIL *)file_p);

    return LV_FS_RES_OK;
}

/**
 * Initialize a 'lv_fs_dir_t' variable for directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to a directory
 * @return          pointer to the directory read descriptor or NULL on error
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char *path)
{
	  FRESULT res;
	
    res = f_opendir(dir,path);
    if(res != FR_OK) return NULL;
    else return &dir;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @param fn        pointer to a buffer to store the filename
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * rddir_p, char *fn)
{
    FRESULT res;
    
	  do
	  {
			res = f_readdir(rddir_p,fileinfo);       /* Read a directory item */
			if(res != FR_OK || fileinfo->fname[0] == NULL)/* Break on error or end of dir */
			{	
				fn[0]='\0';
				return LV_FS_RES_UNKNOWN;  
			}
			if(fileinfo->fattrib & AM_DIR)  /* It is a directory */
			{                   
				fn[0]= '/';
#if _USE_LFN
				strcpy(&fn[1], fileinfo->lfname);
#else
				strcpy(&fn[1], fileinfo->fname);
#endif /*_USE_LFN*/			
			} 
			else 
			{
#if _USE_LFN				
				strcpy(fn, fileinfo->lfname);/* It is a file. */
#else
				strcpy(fn, fileinfo->fname);
#endif /*_USE_LFN*/	
			}
    }while(strcmp(fn,"/.") == 0 || strcmp(fn,"/..") == 0);
	
    return LV_FS_RES_OK;
}

/**
 * Close the directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * rddir_p)
{
    lv_fs_res_t res = f_closedir(rddir_p)? LV_FS_RES_FS_ERR : LV_FS_RES_OK;

    return res;
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
