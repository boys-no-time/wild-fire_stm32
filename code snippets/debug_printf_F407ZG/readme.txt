（1）将Retarget.c文件加入自己的工程

（2）提供Serial_debug.c文件，在该文件中实现sendchar和getkey()

		sendchar即为串口发送单字符函数
		在 main.h 中定义 #define SERIAL_DEBUG 启用打印