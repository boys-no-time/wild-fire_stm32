#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include "inodes_include.h"

//#define HAVE_SUPPORT_FACTORY_TEST

void board_test_thread_entry(void *parameter)
{
    UNUSED(parameter);

    rt_uint32_t CpuID[3];
    char *buff = rt_malloc(100);
    BOARD_TEST_TYPEDEF *board_test = rt_malloc(((sizeof(BOARD_TEST_TYPEDEF) + 1) >> 1) << 1);

    CpuID[0] = *(rt_uint32_t *)(0x1ffff7e8);
    CpuID[1] = *(rt_uint32_t *)(0x1ffff7ec);
    CpuID[2] = *(rt_uint32_t *)(0x1ffff7f0);
    CpuID[0] = CpuID[0] & 0xfffff;
    CpuID[1] = CpuID[1] & 0xfffff;
    CpuID[2] = CpuID[2] & 0xfffff;
    /* printf the chipid */
    rt_kprintf("UID: %08x%08x%08x\r\n", CpuID[0], CpuID[1], CpuID[2]);

    if ((sizeof(BOARD_TEST_TYPEDEF) + 1) >> 1 == nextai_flash_read(NEXTAI_TEST_DATA_WRITE_ADDR, 
					(rt_uint16_t *)board_test, (sizeof(BOARD_TEST_TYPEDEF) + 1) >> 1)) 
	{
#ifdef HAVE_SUPPORT_FACTORY_TEST
        struct hostent *host;
        rt_int32_t  sock, timeout, errorcount;
        struct sockaddr_in server_addr;
        rt_uint32_t revcbytecut;

        if (board_test->ETHOK != 0x5A) {
            inodes_dataout_set(NEXTAI_SIGN_LED_ACT, 1);
            inodes_dataout_set(NEXTAI_SIGN_LED_LINK, 1);

            errorcount = 0;
            while (1) {
                errorcount++;

                if (errorcount > 3) {
                    while (1)
                        inodes_dataout_set(NEXTAI_SIGN_LED_ACT, 0);
                }

                /* 通过函数入口参数url获得host地址（如果是域名，会做域名解析） */
                if ((host = gethostbyname("192.168.3.157")) == NULL) {
                    rt_kprintf("ETH test get host name fail\r\n");
                    rt_thread_delay(500);
                    continue;
                }
                /* 创建一个socket，类型是SOCKET_STREAM，TCP类型 */
                if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
                    /* 创建socket失败 */
                    rt_kprintf("ETH test socket error\r\n");
                    rt_thread_delay(500);
                    continue;
                }

                timeout = 5000;
                /* 设置发送超时时间 */
                lwip_setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
                lwip_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
                /* 初始化预连接的服务端地址 */
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(1888);
                server_addr.sin_addr = *((struct in_addr *)host->h_addr);
                rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

                /* 连接到服务端 */
                if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1) {
                    lwip_close(sock);	//关闭连接，重新创建连接
                    rt_kprintf("ETH test connect fail\r\n");
                    rt_thread_delay(500);
                    continue;
                } 
				else {
                    rt_thread_delay(200);
                    rt_kprintf("ETH test connect success\r\n");
                    sprintf(buff, "UID: %08x%08x%08x", CpuID[0], CpuID[1], CpuID[2]);

                    if (send(sock, buff, strlen(buff), MSG_WAITALL) == -1) {
                        rt_memset(buff, 0, strlen(buff));
                        rt_thread_delay(10);
                        lwip_close(sock);	//关闭连接，重新创建连接
                        rt_kprintf("ETH test send UID fail\r\n");
                        rt_thread_delay(500);
                        continue;
                    }
                    rt_thread_delay(100);
                    rt_memset(buff, 0, 100);
                    revcbytecut = recv(sock, buff, 100, MSG_WAITALL);
                    if (revcbytecut == 0 || revcbytecut == 0xFFFFFFFF) {
                        rt_memset(buff, 0, strlen(buff));
                        rt_thread_delay(10);
                        lwip_close(sock);	//关闭连接，重新创建连接
                        rt_kprintf("ETH test recv UID fail\r\n");
                        rt_thread_delay(500);
                        continue;
                    }
                    if (strcmp(buff, "{test:begin}") != 0) {
                        rt_memset(buff, 0, strlen(buff));
                        rt_thread_delay(10);
                        lwip_close(sock);	//关闭连接，重新创建连接
                        rt_kprintf("ETH test send UID fail\r\n");
                        rt_thread_delay(500);
                        continue;
                    }
                    rt_thread_delay(10);

                    strcpy(buff, "{eth0:normal}");
                    rt_thread_delay(10);
                    if (send(sock, buff, strlen(buff), MSG_WAITALL) == -1) {
                        lwip_close(sock);	//关闭连接，重新创建连接
                        rt_thread_delay(500);
                        continue;
                    } 
					else {
                        lwip_close(sock);	//关闭连接，重新创建连接
                        board_test->ETHOK = 0x5A;
                    }
                    break;
                }
            }
        }

        errorcount = 0;
        if (board_test->RS485OK != 0x5A) {
            rt_device_t RS485;
            RS485 = rt_device_find("uart2");
            if (RS485 != RT_NULL) {
                rt_device_open(RS485, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
//				sprintf(buff,"UID:%010ld%010ld%010ld\r\n",
//						CpuID[0],CpuID[1],CpuID[2]);
//				RS485SendFun(&RS485,(rt_uint8_t*)buff,strlen(buff));
//				send(sock, buff, strlen(buff), MSG_WAITALL);
                rt_memset(buff, 0, 100);
                rt_thread_delay(10);
                while (board_test->RS485OK != 0x5A) {
                    errorcount ++;
                    if (errorcount > 3) {
                        while (1)
                            inodes_dataout_set(NEXTAI_SIGN_LED_LINK, 1);
                    }

                    sprintf(buff, "UID: %08x%08x%08x\r\n", CpuID[0], CpuID[1], CpuID[2]);
                    //sprintf(buff, "UID: 12345678");

                    rt_kprintf("Being tested RS485\r\n");
                    RS485SendFun(&RS485, (rt_uint8_t *)buff, strlen(buff));
                    rt_memset(buff, 0, 100);
                    rt_thread_delay(100);
                    if (rt_device_read(RS485, 0, (rt_uint8_t *)buff, 100)) {
                        rt_kprintf("Recv 485 data:%s\r\n", buff);
                        if (strncmp(buff, "TEST OK", 7) == 0) {
                            board_test->RS485OK = 0x5A;
                            //strcpy(buff,"{RS485:normal}");
                            //RS485SendFun(&RS485,(rt_uint8_t*)buff,strlen(buff));
                            //send(sock, buff, strlen(buff), MSG_WAITALL);
                            rt_kprintf("RS485 normal\r\n");

                            if (board_test->ETHOK == 0x5A) {
                                rt_memcpy(board_test->UID, CpuID, 3 * sizeof(rt_uint32_t));
                                nextai_flash_write(NEXTAI_TEST_DATA_WRITE_ADDR, (rt_uint16_t *)board_test, (sizeof(BOARD_TEST_TYPEDEF) + 1) >> 1);
                            }

                            rt_kprintf("RS485 normal\r\n");

                            errorcount = 0;
                            while (1) {
                                errorcount ^= 1;

                                if (errorcount) {
                                    inodes_dataout_set(NEXTAI_SIGN_LED_ACT, 1);
                                    inodes_dataout_set(NEXTAI_SIGN_LED_LINK, 1);
                                } 
								else {
                                    inodes_dataout_set(NEXTAI_SIGN_LED_ACT, 0);
                                    inodes_dataout_set(NEXTAI_SIGN_LED_LINK, 0);
                                }

                                rt_thread_delay(100);
                            }

                            rt_thread_suspend(rt_thread_self());
                            rt_schedule();
                        } 
						else {
                            rt_kprintf("Error Result\r\n");
                        }
                    } 
					else {
                        strcpy(buff, "{RS485:fail}");
                        rt_kprintf("%s", buff);
                        //send(sock, buff, strlen(buff), MSG_WAITALL);
                    }
                }
            }
        }

        if (rt_memcmp(board_test->UID, CpuID, 3 * sizeof(rt_uint32_t)) != 0) {
            while (1) {
                rt_kprintf("Illegal copy...\r\n");
                rt_thread_delay(300);
            }
        }
#endif
        mqtt_client_create();
#ifdef INODES_HAVE_ETH_BT
		#error "maybe you need create thread for your device"
        //exboard_thread_create();
#endif
    } 
	else {
        rt_kprintf("Test data read fail\r\n");
    }
    rt_free(buff);
    rt_free(board_test);
    rt_thread_detach(rt_thread_self());
}

void test_thread_create(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("board test",
                           board_test_thread_entry, RT_NULL,
                           1024, RT_THREAD_PRIORITY_MAX / 3, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);

}

