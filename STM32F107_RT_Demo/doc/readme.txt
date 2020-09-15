
#Database of STM32F107VCT6   256Kbytes FLASH   64Kbytes RAM

CPU=IRAM(0x2000 0000,0x10000)	IROM(0x0800 0000,0x40000)	CPUTYPE("Cortex-M3") CLOCK(25000000) ELITTLE
//������File --> Device Database
//memory CPU_CORE CLOCK
CPU=IRAM(0x20000000-0x2000FFFF) IROM(0x8000000-0x803FFFF) CLOCK(25000000) CPUTYPE("Cortex-M3")
//FLASH��������
FLDLL=UL2CM3(-O14 -S0 -C0 -N00("ARM Cortex-M3") -D00(1BA00477) -L00(4) -FO7 -FD20000000 -FC800 -FN1 -FF0STM32F10x_CL -FS08000000 -FL040000)
//Ӳ������ģʽ
MON=SARMCM3.DLL TCM.DLL("-pCM3")
//�����ļ�����
SFILE="STARTUP\ST\STM32F10x\startup_stm32f10x_cl.s" ("STM32 Connectivity Line Startup Code")
//����ģʽ
SIM=SARMCM3.DLL("-REMAP") DCM.DLL("-pCM3")
SVD=SFD\ST\STM32F1xx\STM32F107xx.sfr
//MCUͷ�ļ�
REGFILE=stm32f10x.h("ST\STM32F10x")

#��Ŀ�ڴ�滮
	�ͺţ�STM32F107VCT6	 256Kbytes FLASH   64Kbytes RAM
	
	FLASH:	FLASH_BASE_ADDR (0x08000000) 	FLASH_TOTAL_SIZE (0x40000) 		 //0x08000000 - 0x0803FFFF
	//FLASHҳ��С(����д����С����)
	FLASH_PAGE_SIZE 	(2048)								//128 pages of 2Kbytes eash
	//FLASH sector��С(д��������С����)
	FLASH_SECTOR_SIZE	(4096)								//2pages for hign density and connectivity lines  4pages for low and medium density
	
	BOOTLOADER :	0x0800 0000,0x4000
	APP:			0x0800 4000,0x3C000
//���������ļ�
#define NEXTAI_SETTING_WRITE_ADDR   (0x0803f800)   //last page ,2K 
#define FLASH_BOOT_FLAG_ADDR        (0x0803f000)   //
//�����������
#define NEXTAI_TEST_DATA_WRITE_ADDR  (FLASH_BOOT_FLAG_ADDR + 64 )
//���泧�����
#define NEXTAI_FLASH_DEFAULT_ADDR    (NEXTAI_TEST_DATA_WRITE_ADDR + 512)

#define FLASH_APP_START           (0x08004000)     //16K 
#define FLASH_BACKUP_START        (0x08021000)     //132K 
#define FLASH_APP_MAXSIZE         (0x1d000)



	RAM:	SRAM_BASE_ADDR (0x20000000)		SRAM_TOTAL_SIZE (0x10000)        //0x20000000 - 0x2000FFFF
	
	������		(0x2000 0000~0x2000 00C0)
	SysCoreClock:	(0x2000 00C0~0x2000 00C4)
	APP:			(0x2000 00C4~0x2000 FFFF)    //actually set to 0x2000 FFC4
	
				name[size]												start sddress
	
	stm_flash_buf[FLASH_PAGE_SIZE / 2]									0x20001000
	
	
	
	
	
	
	
	
	
	