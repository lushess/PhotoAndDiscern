#ifndef _sd_H
#define _sd_H

#include "system.h"


#define SD_CS PGout(14)		  //SD_CS

// SD�����Ͷ���  
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06	
   
// SD��ָ���  	   
#define CMD0    0       //���п���λ���������״̬
#define CMD1    1
#define CMD2    2       //����2�� ���п�����CID�Ĵ���ֵ��������ʶ���
#define CMD3    3       //����3�� ���п�����RCA��ֵַ����������Ե�ַ
#define CMD4    4       //����4�� [31:16]�������п���DSR����Ƶ�ʣ�[15:0]Dummy
#define CMD7    7       //����7�� [31:16]����RCA��ַ��ѡ����ȡ��RCA��ַ��ָʾ�Ŀ���[15:0]Dummy
#define CMD8    8       //����8 ��SEND_IF_COND����ȷ�ϵ�ѹ�Ƿ�����
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD13   13      //����13��ѡ����ͨ��CMD�߷���״̬�Ĵ���
#define CMD15   15      //����15��ѡ��������"inactive"״̬������Ч״̬
#define CMD16   16      //����16������BlockSize��SDHCͨ��Ϊ512�ֽڣ�Ӧ����0x00
#define CMD17   17      //����17����Block
#define CMD18   18      //����18����Multi Block
#define CMD23   23      //����23�����ö�Blockд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дBlock
#define CMD25   25      //����25��дMulti Block
#define CMD32   32      //����32�����ò�����ʼ���ַ
#define CMD33   33      //����33�����ò����������ַ
#define CMD38   38      //����32����ʼ����ָ����
#define CMD41   41      //����41��Ӧ����0x00
#define CMD42   42      //����42�����������SD��
#define CMD55   55      //����55����������ACMDָ�Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00

// SD���ض�Ӧ��ָ���
#define ACMD6   6       //����6�� [31:2]ΪDummy��[1:0]�����������߿��"00"=1bit��"10"=4bit
#define ACMD41  41      //����41��[31]������[30]HCS��OCR[30]����[29:24]������[23:0]VDD��ѹ��OCR[23:0]��, �������󿨷���֧����Ϣ��HCS����OCR�Ĵ�������
#define ACMD51  51      //����51����ȡ���üĴ���SCR



//����д���Ӧ������
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF
//SD����Ӧ�����
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF 							   						 					    	  

extern u8 SD_Type;//SD��������
//���������� 
u8 SD_SPI_ReadWriteByte(u8 data);
void SD_SPI_SpeedLow(void);
void SD_SPI_SpeedHigh(void);
u8 SD_WaitReady(void);							//�ȴ�SD��׼��
u8 SD_GetResponse(u8 Response);					//�����Ӧ
u8 SD_Init(void);							//��ʼ��
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt);		//����
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt);		//д��
u32 SD_GetSectorCount(void);   					//��������
u8 SD_GetCID(u8 *cid_data);                     //��SD��CID
u8 SD_GetCSD(u8 *csd_data);                     //��SD��CSD
 

#endif
