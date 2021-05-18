一、使用说明：
	1、通过SecureCRT 连接IAP的USART口。
	按住升级按键，重启或重新上电硬件板，然后松开升级按键进入IAP模式。
	2、准备好要烧录的bin文件。注意，FLASH起始地址: 0x08004000,	Size: 0xFC000 
	3、Bootloader占据起始的0x4000位置：
		Note: the 1st sector 0x08000000-0x08003FFF is reserved for the IAP code 
	4、调整向量表基地址至 0x08004000:
		#define VECT_TAB_OFFSET	0X4000
		
	5、选择下载固件选项，点击传输选项选择 send by ymodem，添加bin文件，等待传输完成。
	