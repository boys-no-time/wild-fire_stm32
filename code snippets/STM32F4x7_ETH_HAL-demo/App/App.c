#include <stdio.h>
#include "lwip/tcp.h"
#include <string.h>

#include "app.h"
#include "usart.h"

#include "helloworld.h"

void App_init(void)
{
	printf("hello app init ! \r\n");
	HelloWorld_init();
}

void App_Process(void)
{

}


