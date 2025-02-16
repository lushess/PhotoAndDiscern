#ifndef _sd_H
#define _sd_H

#include "system.h"


#define SD_CS PGout(14)		  //SD_CS

// SD卡类型定义  
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06	
   
// SD卡指令表  	   
#define CMD0    0       //所有卡复位，进入空闲状态
#define CMD1    1
#define CMD2    2       //命令2， 所有卡返回CID寄存器值，即卡的识别号
#define CMD3    3       //命令3， 所有卡返回RCA地址值，即卡的相对地址
#define CMD4    4       //命令4， [31:16]设置所有卡的DSR，即频率，[15:0]Dummy
#define CMD7    7       //命令7， [31:16]发送RCA地址，选定或取消RCA地址所指示的卡，[15:0]Dummy
#define CMD8    8       //命令8 ，SEND_IF_COND，卡确认电压是否正常
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD13   13      //命令13，选定卡通过CMD线发送状态寄存器
#define CMD15   15      //命令15，选定卡进入"inactive"状态，即无效状态
#define CMD16   16      //命令16，设置BlockSize（SDHC通常为512字节）应返回0x00
#define CMD17   17      //命令17，读Block
#define CMD18   18      //命令18，读Multi Block
#define CMD23   23      //命令23，设置多Block写入前预先擦除N个block
#define CMD24   24      //命令24，写Block
#define CMD25   25      //命令25，写Multi Block
#define CMD32   32      //命令32，设置擦除起始块地址
#define CMD33   33      //命令33，设置擦除结束块地址
#define CMD38   38      //命令32，开始擦除指定块
#define CMD41   41      //命令41，应返回0x00
#define CMD42   42      //命令42，加锁或解锁SD卡
#define CMD55   55      //命令55，即将发送ACMD指令，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00

// SD卡特定应用指令表
#define ACMD6   6       //命令6， [31:2]为Dummy，[1:0]设置数据总线宽度"00"=1bit，"10"=4bit
#define ACMD41  41      //命令41，[31]保留，[30]HCS（OCR[30]），[29:24]保留，[23:0]VDD电压（OCR[23:0]）, 主机请求卡发送支持信息（HCS）和OCR寄存器内容
#define ACMD51  51      //命令51，读取配置寄存器SCR



//数据写入回应字意义
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF
//SD卡回应标记字
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF 							   						 					    	  

extern u8 SD_Type;//SD卡的类型
//函数申明区 
u8 SD_SPI_ReadWriteByte(u8 data);
void SD_SPI_SpeedLow(void);
void SD_SPI_SpeedHigh(void);
u8 SD_WaitReady(void);							//等待SD卡准备
u8 SD_GetResponse(u8 Response);					//获得相应
u8 SD_Init(void);							//初始化
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt);		//读块
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt);		//写块
u32 SD_GetSectorCount(void);   					//读扇区数
u8 SD_GetCID(u8 *cid_data);                     //读SD卡CID
u8 SD_GetCSD(u8 *csd_data);                     //读SD卡CSD
 

#endif
