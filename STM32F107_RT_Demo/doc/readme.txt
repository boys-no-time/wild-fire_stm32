
#Database of STM32F107VCT6   256Kbytes FLASH   64Kbytes RAM

CPU=IRAM(0x2000 0000,0x10000)	IROM(0x0800 0000,0x40000)	CPUTYPE("Cortex-M3") CLOCK(25000000) ELITTLE
//来自于File --> Device Database
//memory CPU_CORE CLOCK
CPU=IRAM(0x20000000-0x2000FFFF) IROM(0x8000000-0x803FFFF) CLOCK(25000000) CPUTYPE("Cortex-M3")
//FLASH调试配置
FLDLL=UL2CM3(-O14 -S0 -C0 -N00("ARM Cortex-M3") -D00(1BA00477) -L00(4) -FO7 -FD20000000 -FC800 -FN1 -FF0STM32F10x_CL -FS08000000 -FL040000)
//硬件调试模式
MON=SARMCM3.DLL TCM.DLL("-pCM3")
//启动文件配置
SFILE="STARTUP\ST\STM32F10x\startup_stm32f10x_cl.s" ("STM32 Connectivity Line Startup Code")
//仿真模式
SIM=SARMCM3.DLL("-REMAP") DCM.DLL("-pCM3")
SVD=SFD\ST\STM32F1xx\STM32F107xx.sfr
//MCU头文件
REGFILE=stm32f10x.h("ST\STM32F10x")

#项目内存规划
	型号：STM32F107VCT6	 256Kbytes FLASH   64Kbytes RAM
	
	FLASH:	FLASH_BASE_ADDR (0x08000000) 	FLASH_TOTAL_SIZE (0x40000) 		 //0x08000000 - 0x0803FFFF
	//FLASH页大小(擦、写的最小粒度)
	FLASH_PAGE_SIZE 	(2048)								//128 pages of 2Kbytes eash
	//FLASH sector大小(写保护的最小粒度)
	FLASH_SECTOR_SIZE	(4096)								//2pages for hign density and connectivity lines  4pages for low and medium density
	
	BOOTLOADER :	0x0800 0000,0x4000
	APP:			0x0800 4000,0x3C000
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



	RAM:	SRAM_BASE_ADDR (0x20000000)		SRAM_TOTAL_SIZE (0x10000)        //0x20000000 - 0x2000FFFF
	
	向量表：		(0x2000 0000~0x2000 00C0)
	SysCoreClock:	(0x2000 00C0~0x2000 00C4)
	APP:			(0x2000 00C4~0x2000 FFFF)    //actually set to 0x2000 FFC4
	
				name[size]												start sddress
	
	stm_flash_buf[FLASH_PAGE_SIZE / 2]									0x20001000
	
	
	
	
	
	
	
	
	
	