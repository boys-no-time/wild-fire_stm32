/*********************************************************************
Copyright (c) 2017 by inodes.
All rights reserved. You are not allowed to
copy or distribute the code without permission.
*********************************************************************/

#ifndef _NEXTAI_FLASH__H_
#define _NEXTAI_FLASH__H_

#define NEXTAI_FLASH_BASE_ADDR (0x8000000)

//保存配置文件
#define NEXTAI_SETTING_WRITE_ADDR   (0x0803f800)   //last page ,2K 

#define FLASH_BOOT_FLAG_ADDR        (0x0803f000)   //

//保存测试数据
#define NEXTAI_TEST_DATA_WRITE_ADDR  (FLASH_BOOT_FLAG_ADDR + 64 )

//保存厂测参数
#define NEXTAI_FLASH_DEFAULT_ADDR    (NEXTAI_TEST_DATA_WRITE_ADDR + 512)

#define FLASH_APP_START           (0x08004000)     //16K 
#define FLASH_BACKUP_START        (0x08021000)     //132K 
#define FLASH_APP_MAXSIZE         (0x1d000)


#define FLASH_PAGE_SIZE (2048)

enum {
    INODES_FLASH_TYPE_APPEND,
    INODES_FLASH_TYPE_WIRTE
};


#ifdef __cplusplus
extern "C"
{
#endif

void nextai_flash_init(void);

int nextai_flash_write(unsigned int addr, unsigned short *databuf, unsigned short datalen);

unsigned short nextai_flash_read(unsigned int addr, unsigned short *databuf, unsigned short datalen);

#ifdef __cplusplus
}
#endif


#endif

