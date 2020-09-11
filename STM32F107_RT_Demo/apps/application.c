/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <rtthread.h>

#ifdef RT_USING_DFS
#include <dfs_fs.h>
#include <dfs_init.h>
#include <dfs_elm.h>
#endif

#ifdef RT_USING_LWIP
#include <stm32_eth.h>
#include <netif/ethernetif.h>
extern int lwip_system_init(void);
#endif

#ifdef RT_USING_FINSH
#include <shell.h>
#include <finsh.h>
#endif

#include "inodes_include.h"

void rt_init_thread_entry(void *parameter)
{
    UNUSED(parameter);

    rt_platform_init();

    /* Filesystem Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
    /* initialize the device file system */
    dfs_init();

    /* initialize the elm chan FatFS file system*/
    elm_init();

    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0) {
        rt_kprintf("File System initialized!\r\n");
    } else {
        rt_kprintf("File System initialzation failed!\r\n");
    }
#endif /* RT_USING_DFS && RT_USING_DFS_ELMFAT */

#ifdef RT_USING_LWIP
    /* initialize lwip stack */
    /* register ethernetif device */
    eth_system_device_init();

    eth_phy_thread_startup();

    /* initialize lwip system */
    lwip_system_init();
    rt_kprintf("TCP/IP initialized!\r\n");
#endif

#ifdef RT_USING_FINSH
    /* initialize finsh */
    finsh_system_init();
    finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

    //		rt_thread_resume(MQTT_client.netconnect);
    //		rt_thread_resume(setthreah.udp);

    LED_STATUS(ON);

    //test_thread_create();
    rt_thread_detach(rt_thread_self());
}

#if 0 //never invoked

#endif

int rt_application_init(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
                           rt_init_thread_entry, RT_NULL,
                           2048, RT_THREAD_PRIORITY_MAX / 3, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);

    nextai_setting_create();

    return 0;
}

/*@}*/
