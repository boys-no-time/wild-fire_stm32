#include "inodes_include.h"

#define WRITE_BLOCK_SIZE    FLASH_PAGE_SIZE

pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t FirstAppCode;

struct up_flash_info *flash_info;
struct up_flash_info my_up_info;

static void UpgradeSetDone(void)
{
    struct up_flash_info *flash_info;

    flash_info = (struct up_flash_info *)FLASH_BOOT_FLAG_ADDR;

    if (flash_info->bootflag != FLAG_NORMAL) {
        memcpy((void *) &my_up_info, (void *) flash_info, sizeof(struct up_flash_info));
        my_up_info.bootflag = FLAG_NORMAL;
        nextai_flash_write(FLASH_BOOT_FLAG_ADDR, (uint16_t *) & my_up_info, (sizeof(my_up_info) + 1) / 2);
    }
}


int main(void)
{
    int32_t result;
    int32_t retry = 0;
    int32_t error = 0;

    uint32_t flash_src = 0;
    uint32_t flash_dst = 0;
    uint32_t total_len = 0;
    uint32_t write2_len = 0;
    uint32_t written_len = 0;

    SystemCoreClockUpdate();
    Usart_Init();

    printf("\r\n\r\nBootloader booting ....\r\n");
	inodes_print_ver(printf);

    flash_info = (struct up_flash_info *)FLASH_BOOT_FLAG_ADDR;

    if ((flash_info->bootflag == FLAG_UPGRADE) && ((flash_info->filelen > 4096) && (flash_info->filelen <= FLASH_APP_MAXSIZE))) {
        goto BootMode;
    }

    FirstAppCode = (*(__IO uint32_t *)APPLICATION_ADDRESS);
    printf("Jumping to application code.\n");
    if ((FirstAppCode & 0x2FF80000) == 0x20000000) {
        DisableMIRQ ;
        JumpAddress = *(__IO uint32_t *)(APPLICATION_ADDRESS + 4);
        Jump_To_Application = (pFunction) JumpAddress;
        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t *) APPLICATION_ADDRESS);
        Jump_To_Application();
    }

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    printf("Failed to jump to application, no app code found.\n");
    while (1);

BootMode:
    //SysTick_Config(SystemCoreClock / 1000);
    printf("Enter Boot mode...\n");

    if ((flash_info->bootflag == FLAG_UPGRADE) && ((flash_info->filelen > 4096) && (flash_info->filelen <= FLASH_APP_MAXSIZE))) {
        printf("Upgrading firmware len: %d ver: %s\n", flash_info->filelen, flash_info->ver);

        total_len = ((flash_info->filelen + 3) >> 2) << 2 ;

        write2_len = 0;
        written_len = 0;
        flash_src = FLASH_BACKUP_START;
        flash_dst = FLASH_APP_START;
        do {
            write2_len = (total_len - written_len > WRITE_BLOCK_SIZE) ? WRITE_BLOCK_SIZE : (total_len - written_len);
            retry = 0;
            error = 1;
            do {
                result = nextai_flash_write(flash_dst, (uint16_t *) flash_src,  write2_len / 2);
                if (result == (int32_t)(write2_len / 2)) {
                    error = 0;
                    break;
                }
            } while (++retry < 5);
            printf("Writing flash %d/%d.\n", written_len, total_len);
            if (error) {
                printf("Write flash fail.\n");
                break;
            }
            written_len += write2_len ;
            flash_src += write2_len;
            flash_dst += write2_len;
        } while (written_len < total_len);
    }

    if ((error == 0) && (written_len >= total_len)) {
        printf("Writing Flash successfully. Rebooting......\n");
        UpgradeSetDone();
        NVIC_SystemReset();
    } else {
        printf("Can't upgrade code successfully. Maybe reboot to try it again......\n");
        Delay(10000000);
        NVIC_SystemReset();
    }

    return 0;
}

void assert_failed(u8 *file, u32 line)
{
    printf("\n\r[Bootloader] Wrong parameter value detected on\r\n");
    printf("       file  %s\r\n", file);
    printf("       line  %d\r\n", line);

    while (1) ;
}

