#include "inodes_include.h"

#ifdef __CC_ARM
unsigned short stm_flash_buf[FLASH_PAGE_SIZE / 2] __attribute__((at(0x20001000)));
#else //__GNUC__
unsigned short stm_flash_buf[FLASH_PAGE_SIZE / 2] __attribute__((aligned(4)));
#endif

static void nextai_write_nocheck(unsigned int write_addr, unsigned short *w_buf, unsigned short datalen);


void nextai_flash_init(void)
{


}


static void nextai_write_nocheck(unsigned int write_addr, unsigned short *w_buf, unsigned short datalen)
{
    unsigned short i = 0;

    for (i = 0; i < datalen; i++) {
        FLASH_ProgramHalfWord(write_addr + i * 2, w_buf[i]);
    }
}



int nextai_flash_write(unsigned int addr, unsigned short *databuf, unsigned short datalen)
{
    unsigned short i = 0, j = 0;
    unsigned int offset_addr = 0;        //相对基址的偏移地址
    unsigned int sec_pos = 0;            //扇区地址
    unsigned short sec_offset = 0;       //扇区内偏移地址
    unsigned short sec_remain = 0;       //扇区内剩余地址
    unsigned char next_sec_flag = 0;     //是否跨扇区 1 跨扇区
    unsigned int w_addr = addr;
    unsigned short *w_buf = databuf;
    unsigned short w_datalen = datalen;

    if (datalen > (FLASH_PAGE_SIZE >> 1))
        return -1;

    /* Unlock the Flash Program Erase controller */
    FLASH_Unlock();

    offset_addr = w_addr - NEXTAI_FLASH_BASE_ADDR;


    /* Define the number of page to be erased */
    sec_pos = offset_addr / FLASH_PAGE_SIZE;
    sec_offset = (offset_addr % FLASH_PAGE_SIZE) / 2;    //扇区内偏移（2字节基本单位）
    sec_remain = FLASH_PAGE_SIZE / 2 - sec_offset;       //扇区剩余空间

    if (datalen > sec_remain) {
        next_sec_flag = 1;
    } else {
        sec_remain = datalen;
    }

    while (1) {
        nextai_flash_read(NEXTAI_FLASH_BASE_ADDR + sec_pos * FLASH_PAGE_SIZE, (unsigned short *)stm_flash_buf, FLASH_PAGE_SIZE / 2);

        for (i = 0; i < sec_remain; i++) {
            if (stm_flash_buf[sec_offset + i] != 0xffff)break;
        }

        if (i < sec_remain) {
            FLASH_ErasePage(sec_pos * FLASH_PAGE_SIZE + NEXTAI_FLASH_BASE_ADDR);

            for (j = 0; j < sec_remain; j++) {
                stm_flash_buf[sec_offset + j] = w_buf[j];
            }

            nextai_write_nocheck(sec_pos * FLASH_PAGE_SIZE + NEXTAI_FLASH_BASE_ADDR, stm_flash_buf, FLASH_PAGE_SIZE / 2);
        } else
            nextai_write_nocheck(w_addr, w_buf, sec_remain);

        if (next_sec_flag) {
            sec_pos++;
            sec_offset = 0;

            w_addr += sec_remain * 2;
            w_buf += sec_remain;
            w_datalen -= sec_remain;

            if (w_datalen > FLASH_PAGE_SIZE / 2) {
                sec_remain = FLASH_PAGE_SIZE / 2;
            } else {
                sec_remain = w_datalen;
                next_sec_flag = 0;
            }
        } else
            break;
    }

    FLASH_Lock();

    return datalen;
}

unsigned short nextai_flash_read(unsigned int addr, unsigned short *databuf, unsigned short datalen)
{
    unsigned short i = 0, read_len = datalen;

    if (read_len > 2 * 1024)
        read_len = 2 * 1024;

    for (i = 0; i < read_len; i++) {
        databuf[i] = *(volatile unsigned short *)(addr + (i << 1));
    }

    return read_len;
}


