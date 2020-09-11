/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/
#include "inodes_include.h"

//#ifdef INODES_HAVE_ETH_BT
//#error "your own code for communication"
//#else

//#define INODES_KEY_MODULE_LEN (6)
//#define INODES_MAX_VIRTUAL_ADDR (256)
//#define INODES_MAX_EXBOARD (24)

//EXBOARDTYPEDEF exboard;

//static rt_uint32_t data_led[INODES_MAX_EXBOARD];
//static rt_uint8_t loop_flag = 0;
//static rt_uint32_t last_loop_time = 0;

////小板状态
//static rt_uint8_t data_map_buf[INODES_MAX_VIRTUAL_ADDR];
//static rt_uint8_t data_map_buf_temp[INODES_MAX_VIRTUAL_ADDR];
//static rt_uint8_t single_channel_flag[INODES_MAX_EXBOARD] = {0};
//static rt_uint8_t single_time_out_flag[INODES_MAX_EXBOARD] = {0};

//void readRS485(rt_uint8_t clientId, rt_uint8_t len);

//#ifdef INODES_SUPPORT_SELF
///*********************************************************************************************
//包头(1) 终端ID  指令  长度  数据  校验  包尾
//0x88       1     1      1    n     2     0x66
//*********************************************************************************************/
//static rt_uint32_t inodes_data_packet(rt_uint8_t cmd, rt_uint8_t clientid, rt_uint8_t *dntbuf, rt_uint8_t *dntlen)
//{
//    rt_uint8_t *cur_p = dntbuf, *crc_p = dntbuf;
//    rt_uint16_t dnt_crc = 0;

//    *cur_p++ = 0x88;
//    *cur_p++ = clientid;
//    *cur_p++ = cmd;
//    *cur_p++ = 0;

//    while (crc_p < cur_p) {
//        dnt_crc = (dnt_crc + *crc_p++) & 0xffff;
//    }
//    *cur_p++ = dnt_crc & 0xff;
//    *cur_p++ = (dnt_crc >> 8) & 0xff;

//    *cur_p = 0x66;

//    *dntlen = 7;

//    return 0;
//}

//static rt_int32_t inodes_data_unpacket(rt_uint8_t *srcdata, rt_uint8_t srclen, rt_uint8_t *respcmd, rt_uint8_t *clientid, rt_uint8_t *dntbuf, rt_uint8_t *dntlen)
//{
//    rt_uint8_t *cur_p = srcdata, *crc_p = srcdata, *write_p = dntbuf;
//    rt_uint32_t dnt_crc = 0, src_crc = 0;
//    int i = 0;

//    if (*cur_p++ != 0x88) {
//        return -1;
//    }
//    if (srclen < 7) {
//        return -1;
//    }

//    *clientid = *cur_p++;
//    *respcmd = *cur_p++;
//    *dntlen = *cur_p++;

//    for (i = 0; i < *dntlen; i++) {
//        *write_p++ = *cur_p++;
//    }

//    src_crc = *cur_p | (*(cur_p + 1) << 8);

//    while (crc_p < cur_p) {
//        dnt_crc = (dnt_crc + *crc_p++) & 0xffff;
//    }
//    if (dnt_crc != src_crc) {
//        return -1;
//    }
//    cur_p += 2;
//    if (0x66 != *cur_p) {
//        return -1;
//    }

//    return 0;
//}

//#else
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//static const unsigned char table_crc_hi[] = {
//    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
//    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
//    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
//    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
//    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
//    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
//    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
//    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
//    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
//    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
//    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
//    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
//    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
//    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
//    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
//    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
//    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
//    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
//    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
//    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
//    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
//    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
//    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
//    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
//    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
//    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
//};

///*********************************************************************
//*** crc ??????
//*********************************************************************/
//static const unsigned char table_crc_lo[] = {
//    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
//    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
//    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
//    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
//    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
//    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
//    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
//    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
//    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
//    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
//    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
//    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
//    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
//    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
//    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
//    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
//    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
//    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
//    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
//    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
//    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
//    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
//    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
//    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
//    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
//    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
//};



///********************************************************************************
//*** CRC
//********************************************************************************/
//static unsigned short crc16(unsigned char *buffer, unsigned short buffer_length)
//{
//    unsigned char crc_hi = 0xFF; /* high CRC byte initialized */
//    unsigned char crc_lo = 0xFF; /* low CRC byte initialized */
//    unsigned int i = 0;

//    /* pass through message buffer */
//    while (buffer_length--) {
//        i = crc_hi ^ *buffer++; /* calculate the CRC  */
//        crc_hi = crc_lo ^ table_crc_hi[i];
//        crc_lo = table_crc_lo[i];
//    }

//    return (crc_hi << 8 | crc_lo);
//}


//static rt_uint32_t inodes_data_packet(rt_uint8_t cmd, rt_uint8_t clientid, rt_uint8_t *dntbuf, rt_uint8_t *dntlen)
//{
//    unsigned char *cur_p = dntbuf, *crc_p = 0;
//    unsigned short crc_value = 0;

//    crc_p = cur_p;
//    *cur_p++ = 0x20;
//    if (cmd == INODES_CMD_GETSTATUS) {
//        *cur_p++ = 0x12;
//    }
//    else if (cmd == INODES_CMD_OPENKEY) {
//        *cur_p++ = 0x21;
//    }
//    else if (cmd == INODES_CMD_CLOSEKEY) {
//        *cur_p++ = 0x22;
//    }
//    else if (cmd == INODES_CMD_CLOSELED) {
//        *cur_p++ = 0x31;
//    }
//    else if (cmd == INODES_CMD_OPENREDLED) {
//        *cur_p++ = 0x32;
//    }
//    else if (cmd == INODES_CMD_OPENBLUELED) {
//        *cur_p++ = 0x33;
//    }

//    *cur_p++ = 8;
//    *cur_p++ = clientid;

//    crc_value = crc16(crc_p, 4);

//    *cur_p++ = (crc_value >> 8) & 0xff;
//    *cur_p++ = crc_value & 0xff;

//    *dntlen = 6;

//    return 0;
//}


//static int inodes_data_unpacket(unsigned char *srcdata, unsigned int srclen, INODES_KEY_INFO *cur_key_data)
//{
////	   for(int i = 0;i < srclen;i++)
////	   {
////		      rt_kprintf("cur_p %d is %d\r\n",i + 1,*(srcdata+i));
////		 }

//    unsigned char *cur_p = srcdata, *crc_p = 0;
//    unsigned int dnt_crc = 0, src_crc = 0, loop_i = 0;
//    if (!((*cur_p == 0x10) || (*cur_p == 0x11) || (*cur_p == 0x12) || (*cur_p == 0x13))) {
//        return -1;
//    }

//    for (loop_i = 0; loop_i < srclen;) {
//        //rt_kprintf("%x, %x, %x, %x, %x, %x, %x, %x\r\n", srcdata[loop_i], srcdata[loop_i + 1], srcdata[loop_i + 2], srcdata[loop_i + 3], srcdata[loop_i + 4], srcdata[loop_i + 5], srcdata[loop_i + 6], srcdata[loop_i + 7]);
//        loop_i += 8;
//    }



//    crc_p = cur_p;
//    cur_key_data->respret = *cur_p++;
//    if (*cur_p == 0x12) {
//        cur_key_data->respcmd = INODES_CMD_GETSTATUS;
//    }
//    else if (*cur_p == 0x21) {
//        cur_key_data->respcmd = INODES_CMD_OPENKEY;
//    }
//    else if (*cur_p == 0x22) {
//        cur_key_data->respcmd = INODES_CMD_CLOSEKEY;
//    }
//    else if (*cur_p == 0x31) {
//        cur_key_data->respcmd = INODES_CMD_CLOSELED;
//    }
//    else if (*cur_p == 0x32) {
//        cur_key_data->respcmd = INODES_CMD_OPENREDLED;
//    }
//    else if (*cur_p == 0x33) {
//        cur_key_data->respcmd = INODES_CMD_OPENBLUELED;
//    }
//    else {
//        return -1;
//    }
//    cur_p += 1;
//    cur_key_data->boxaddr = *cur_p++;
//    cur_key_data->chanaddr = *cur_p++;
//    if (cur_key_data->respcmd == INODES_CMD_GETSTATUS) {

//        memcpy(cur_key_data->keyid, cur_p, 5);
//        cur_p += 5;
//        dnt_crc = crc16(crc_p, 9);
//    }
//    else {
//        rt_memset(cur_key_data->keyid, 0, 5);
//        dnt_crc = crc16(crc_p, 4);
//    }
//    src_crc = (*cur_p << 8) | (*(cur_p + 1));

//    if (dnt_crc != src_crc) {
//        return -1;
//    }

//    cur_p += 2;
//    if (*cur_p != 0x03) {
//        return -1;
//    }


//    return 0;
//}


//#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////小板轮询
//void exboardloop(void)
//{
//    int8_t do_retry = 0;
//    static rt_uint8_t i = 0;
//    static rt_uint8_t loop_led_status = 0;
//    static rt_uint8_t loop_timeout = 0;
//    rt_int32_t parser_ret = -1;
////		rt_uint8_t reqcmd = 0, deviceid = 0;
//    rt_uint8_t key_status = 0;
//    INODES_KEY_INFO cur_key_data = {0};

//    i++;
//    if ((i > 24)) {
////			  rt_thread_delay(500);
//        last_loop_time = rt_tick_get();
//        loop_timeout = 0;
//        i = 0;
//        return;
//    }


////		for(i=1;i<=24;i++)
//    {
////				rt_kprintf("send client %d get status order !\r\n", i);
//        inodes_data_packet(INODES_CMD_GETSTATUS, i, exboard.send, &exboard.bytes_send);

////				rt_kprintf("%d, %d, %d, %d, %d, %d!\r\n", exboard.send[0], exboard.send[1], exboard.send[2], exboard.send[3], exboard.send[4], exboard.send[5]);

//        do_retry  = 0;
//retry:
//        RS485SendFun(&exboard.RS485, exboard.send, exboard.bytes_send);
//        // rt_thread_delay(80);
//        // exboard.bytes_revc = rt_device_read(exboard.RS485, 0, exboard.revc, EXBOARD_REVC_MAX);
//        readRS485(i, 12);
//        if (exboard.bytes_revc) {
//            parser_ret = inodes_data_unpacket(exboard.revc, exboard.bytes_revc, &cur_key_data);
//            if (0 == parser_ret) {
//                //								rt_kprintf("%d parse success!\r\n", i);
//                if (cur_key_data.respcmd == INODES_CMD_GETSTATUS) {
//                    key_status = exboard.databuf[0];
//                    inodes_virtual_write(cur_key_data.chanaddr, cur_key_data.respret, &cur_key_data.keyid[0]);
//                    // rt_kprintf("write deviceid %d, status %d!\r\n", i, key_status);
//                }
//            }
//            else {
//                rt_kprintf("The CH %d error code is %d\r\n", i, parser_ret);
//            }
//        }
//        else {
//            if (++do_retry < 2) {
//                rt_kprintf("The CH %d timeout,retry = %d \r\n", i, do_retry);
//                goto retry;
//            }
//            key_status = 2;
//            rt_memset(exboard.databuf, 0, CLIENT_MAX_DATABUF_LENGTH);
//            inodes_virtual_write(i, key_status, &exboard.databuf[1]);
//            loop_timeout = 1;
//            rt_kprintf("The CH %d timeout \r\n", i, parser_ret);
//        }
//    }

//    {
//        if (data_led[i - 1] > 0) {
//            //	  rt_kprintf("The time is %ld ,%ld\r\n",data_led[i - 1],rt_tick_get());
//            if ((rt_tick_get() - data_led[i - 1]) >= 10 * 100) {
//                //led open 10s,close
//                rt_kprintf("The led %d is open 10s,send close order !\r\n", i);
//                inodes_data_packet(INODES_CMD_CLOSELED, i, exboard.send, &exboard.bytes_send);
//                rt_kprintf("%d, %d, %d, %d, %d, %d!\r\n", exboard.send[0], exboard.send[1], exboard.send[2], exboard.send[3], exboard.send[4], exboard.send[5]);
//                do_retry  = 0;
//retry_close_led:
//                RS485SendFun(&exboard.RS485, exboard.send, exboard.bytes_send);
//                // rt_thread_delay(50);
//                // exboard.bytes_revc = rt_device_read(exboard.RS485, 0, exboard.revc, EXBOARD_REVC_MAX);
//                readRS485(i, 7);
//                if (exboard.bytes_revc) {
//                    parser_ret = inodes_data_unpacket(exboard.revc, exboard.bytes_revc, &cur_key_data);
//                    if (0 == parser_ret) {
//                        if (cur_key_data.respcmd == INODES_CMD_CLOSELED) {
//                            rt_kprintf("led %d close success!\r\n", i);
//                            data_led[i - 1] = 0;
//                        }
//                    }
//                }
//                else {
//                    if (++do_retry < 2) {
//                        rt_kprintf("led %d close time out,retry = %d \r\n", i, do_retry);
//                        goto retry_close_led;
//                    }
//                }
//            }
//        }
//    }

//    if ((i == 24) /*&& (mqtt_get_clientstatus() == MQTT_CLIENT_STATUS_PUBLISH)*/) {
//        //发生变化
//        if (inodes_virtual_cmp()) {
//            //唤醒立即进行发布
//            //rt_thread_resume(MQTT_client.client);
//            //rt_schedule();
//            //rt_kprintf("loop end,state has changed");

//            /* 使用event通知serve线程做事情，杜绝全局变量存取!
//            mqtt_set_interval(0); */
//			rt_kprintf("[exboardloop] 1 send DO_PUBLISH!\r\n");
//			rt_event_send(mqtt_get_ev(), MQTT_EVENT_DO_PUBLISH);
//        }
//    }

//    if ((i == 24) && (loop_timeout == 0) /*&& (mqtt_get_clientstatus() == MQTT_CLIENT_STATUS_PUBLISH)*/) {
//        if (loop_flag == 1) {
//            //first time publish
//            // rt_kprintf("loop end,state has changed by loop_flag == 1");
//            /* 使用event通知serve线程做事情，杜绝全局变量存取!
//            mqtt_set_interval(0); */
//			rt_kprintf("[exboardloop] 2 send DO_PUBLISH!\r\n");
//			rt_event_send(mqtt_get_ev(), MQTT_EVENT_DO_PUBLISH);
//            loop_flag = 2;
//        }
//        if (loop_flag == 0) {
//            loop_flag = 1;
//        }
//        loop_led_status ++;
//        loop_led_status &= 0xFF;
//        inodes_dataout_set(NEXTAI_SIGN_LED_ACT, loop_led_status % 2);
//    }
//}

//void servercmdprocess(SERVERCMDTYPEDEF *cmdbuf)
//{
//    //处理外部数据事件
//    rt_uint8_t status = 1;
//    rt_uint8_t statustemp;
//    rt_uint8_t errorcode = 0;
//    rt_uint8_t do_retry = 0;//重试时间
//    rt_int32_t parser_ret = -1;
//    //rt_uint8_t reqcmd = 0, deviceid = 0;
//    //rt_uint8_t key_status = 0;
//    rt_uint8_t ledcmd[3] = {INODES_CMD_CLOSELED, INODES_CMD_OPENREDLED, INODES_CMD_OPENBLUELED};
//    rt_uint8_t lockcmd[2] = {INODES_CMD_OPENKEY, INODES_CMD_CLOSEKEY};
//    rt_uint32_t cmdid = cmdbuf->cmdid;
//    INODES_KEY_INFO cur_key_data = {0};

//    exboard.cur_out_clientnum = cmdbuf->num;
//    rt_kprintf("checke cmd is %d ,led cmd is : %d,lock cmd is : %d\r\n", cmdbuf->check, cmdbuf->led, cmdbuf->lock);
//    //处理LED指令
//    if (cmdbuf->led != 0xFF) {
//        //0：关闭，1：红灯，2：绿灯,FF不做操作
//        statustemp = 0;
//        if (cmdbuf->led <= 2) {
//            exboard.cur_out_event = ledcmd[cmdbuf->led];
//            inodes_data_packet(exboard.cur_out_event, exboard.cur_out_clientnum, exboard.send, &exboard.bytes_send);
//            if (exboard.bytes_send > 0) {
//                do_retry = 0;
//retry_open_led:
//                RS485SendFun(&exboard.RS485, exboard.send, exboard.bytes_send);
//                // rt_thread_delay(50);
//                // exboard.bytes_revc = rt_device_read(exboard.RS485, 0, exboard.revc, EXBOARD_REVC_MAX);
//                readRS485(exboard.cur_out_clientnum, 7);
//                if (exboard.bytes_revc) {
//                    parser_ret = inodes_data_unpacket(exboard.revc, exboard.bytes_revc, &cur_key_data);
//                    if (0 == parser_ret) {
//                        rt_kprintf("led operate parse success, respcmd is %d,cur_out_event is %d!\r\n", cur_key_data.respcmd,
//                                   exboard.cur_out_event);
//                        if (exboard.cur_out_event == ledcmd[0]) {
//                            data_led[exboard.cur_out_clientnum - 1] = 0;
//                        }
//                        else {
//                            data_led[exboard.cur_out_clientnum - 1] = rt_tick_get();
//                        }
//                        if (cur_key_data.respcmd == exboard.cur_out_event) {
//                            statustemp = 1;
//                        }
//                    }
//                }
//                else {
//                    if (++do_retry < 2) {
//                        rt_kprintf("led %d open time out,retry = %d \r\n", exboard.cur_out_clientnum, do_retry);
//                        goto retry_open_led;
//                    }
//                }
//            }
//        }
//        if (statustemp == 0) {
//            //执行失败
//            errorcode |= 1;	//定义错误码
//            status = 0;
//        }
//    }
//    //处理锁指令
//    if (cmdbuf->lock != 0xFF) {
//        //0：打开，1：关闭,FF不做操作
//        statustemp = 0;
//        if (cmdbuf->lock <= 1) {
//            exboard.cur_out_event = lockcmd[cmdbuf->lock];
//            inodes_data_packet(exboard.cur_out_event, exboard.cur_out_clientnum, exboard.send, &exboard.bytes_send);
//            if (exboard.bytes_send > 0) {
//                do_retry = 0;
//retry_open_lock:
//                RS485SendFun(&exboard.RS485, exboard.send, exboard.bytes_send);
//                // rt_thread_delay(50);
//                // exboard.bytes_revc = rt_device_read(exboard.RS485, 0, exboard.revc, EXBOARD_REVC_MAX);
//                readRS485(exboard.cur_out_clientnum, 7);
//                if (exboard.bytes_revc) {
//                    parser_ret = inodes_data_unpacket(exboard.revc, exboard.bytes_revc, &cur_key_data);
//                    if (0 == parser_ret) {
//                        rt_kprintf("lock operate parse success, respcmd is %d,cur_out_event is %d!\r\n", cur_key_data.respcmd,
//                                   exboard.cur_out_event);
//                        if (cur_key_data.respcmd == exboard.cur_out_event) {
//                            statustemp = 1;
//                        }
//                    }
//                }
//                else {
//                    if (++do_retry < 2) {
//                        rt_kprintf("lock %d open time out,retry = %d \r\n", exboard.cur_out_clientnum, do_retry);
//                        goto retry_open_lock;
//                    }
//                }
//            }
//        }
//        if (statustemp == 0) {
//            //执行失败
//            errorcode |= 2;	//定义错误码
//            status = 0;
//        }
//    }

//    //在此向服务器回复内容
//    rt_memset(exboard.mqbuf, 0, BOARD_MQ_MAX);
//    sprintf((char *)exboard.mqbuf, "{num:%d,status:%d,errorcode:%2x,id:%ld}",
//            exboard.cur_out_clientnum, status, errorcode, cmdid);
//    rt_mq_send(mqtt_get_mq(), exboard.mqbuf, strlen((char *)exboard.mqbuf) + 1);
//    {

//        inodes_data_packet(INODES_CMD_GETSTATUS, exboard.cur_out_clientnum, exboard.send, &exboard.bytes_send);
//        do_retry = 0;
//retry_get_lock_status:
//        RS485SendFun(&exboard.RS485, exboard.send, exboard.bytes_send);
//        // rt_thread_delay(50);
//        // exboard.bytes_revc = rt_device_read(exboard.RS485, 0, exboard.revc, EXBOARD_REVC_MAX);
//        readRS485(exboard.cur_out_clientnum, 12);
//        if (exboard.bytes_revc) {
//            parser_ret = inodes_data_unpacket(exboard.revc, exboard.bytes_revc, &cur_key_data);
//            if (0 == parser_ret) {
//                if (cur_key_data.respcmd == INODES_CMD_GETSTATUS) {
//                    inodes_virtual_write(cur_key_data.chanaddr, cur_key_data.respret, &cur_key_data.keyid[0]);
//                }
//            }
//        }
//        else {
//            if (++do_retry < 2) {
//                rt_kprintf("client %d get status time out,retry = %d \r\n", exboard.cur_out_clientnum, do_retry);
//                goto retry_get_lock_status;
//            }
//        }
//        //if (mqtt_get_clientstatus() == MQTT_CLIENT_STATUS_PUBLISH) 
//        {
//            //发生变化
//            //rt_kprintf("clientid%d status has changed", exboard.cur_out_clientnum);
//            /* 使用event通知serve线程做事情，杜绝全局变量存取!
//            mqtt_set_interval(0); */
//			rt_kprintf("[servercmdprocess] send DO_PUBLISH!\r\n");
//			rt_event_send(mqtt_get_ev(), MQTT_EVENT_DO_PUBLISH);
//        }
//    }

//}

//void exboard_thread_entry(void *parameter)
//{
//    //rt_uint8_t i;
//    //rt_int32_t parser_ret = -1;
//    //rt_uint8_t reqcmd = 0, deviceid = 0;
//    //rt_uint8_t key_status = 0;
//    UNUSED(parameter);
//#ifdef INODES_USING_SWDOG
//    static rt_int8_t dog_index;
//    dog_index = swdog_create(rt_thread_self(), DOG_STATUS_AWEEK);
//#endif
//    exboard.RS485 = rt_device_find("uart2");

//    if (exboard.RS485 != RT_NULL) {
//        rt_device_open(exboard.RS485, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
//        while (1) {
//#ifdef INODES_USING_SWDOG
//            swdog_feed(dog_index);
//#endif
//            //轮询
//            if ((rt_tick_get() - last_loop_time) > 2 * 100) {
//                exboardloop();
//            }
//            //rt_thread_delay(50);
//            //处理服务器指令
//            while (rt_mq_recv(exboard.mq, exboard.mqbuf, BOARD_MQ_MAX, 1) == RT_EOK) {
//                rt_kprintf("[servercmdprocess] recv msg!\r\n");
//                servercmdprocess((SERVERCMDTYPEDEF *)exboard.mqbuf);
//            }
//            rt_thread_yield();
//        }
//    }
//}

//void Exstate(void *parameter)
//{
//    rt_uint8_t i = 1; //INODES_CMD_OPENKEY;
//    UNUSED(parameter);
//    while (1) {
//        nextai_client_out_proc(3, i);
//        if (i >= 2) { //INODES_CMD_CLOSELED)
//            i = 0;    //INODES_CMD_OPENKEY;
//        }
//        else {
//            i++;
//        }
//        rt_thread_delay(1000);
//    }
//}

//void readRS485(rt_uint8_t clientId, rt_uint8_t len)
//{
//    rt_uint8_t revc_data[64] = {0};
//    rt_uint8_t revc_len;
//    rt_uint32_t last_read = 0;
//    rt_uint8_t read_count = 0;

//    UNUSED(clientId);
//    //add by zxl 每隔100ms读取一次，最长读取1s
//    last_read = rt_tick_get();
//    // rt_kprintf("last_read is %d\r\n",last_read);
//    memset(exboard.revc, 0, EXBOARD_REVC_MAX);
//    exboard.bytes_revc = 0;
//    read_count = 0;
//    do {
//        //等待100ms读取
//        rt_thread_delay(10);
//        memset(revc_data, 0, 64);
//        revc_len = rt_device_read(exboard.RS485, 0, revc_data, 64);
//        // exboard.bytes_revc = rt_device_read(exboard.RS485, 0, exboard.revc, EXBOARD_REVC_MAX);
//        // rt_kprintf("client %d read count is %d,exboard.bytes_revc is %d,revc_len is %d\r\n",clientId,read_count,exboard.bytes_revc,revc_len);
//        //读到数据或者读取时间超过1s跳出循环
//        // rt_kprintf("rt_tick_get is %d",rt_tick_get());
//        if (revc_len > 0) {
//            // rt_kprintf("revc_data is:");
//            // for (int j = 0; j < revc_len; j++){
//            //     rt_kprintf("%02x,",revc_data[j]);
//            // }
//            rt_memcpy(exboard.revc + exboard.bytes_revc, revc_data, revc_len);
//            exboard.bytes_revc += revc_len;
//            // rt_kprintf("\r\nexboard.revc is:");
//            // for (int j = 0; j < exboard.bytes_revc; j++){
//            //     rt_kprintf("%02x,",exboard.revc[j]);
//            // }
//            // rt_kprintf("\r\n");
//        }
//        read_count++;
//    }
//    while ((exboard.bytes_revc < len) && ((rt_tick_get() - last_read) < 1 * 100));
//}


//rt_uint8_t nextai_client_out_proc(rt_uint8_t clientnum, rt_int32_t cur_cmd)
//{
//    static rt_uint32_t id = 1;
//    char p[64];// = "{\"type\":\"slot\",\"num\":\"1\",\"cmd\":\"FF|2|0\",\"id\":\"123\"}";
//    //系统正忙
//    //     if(exboard.cur_out_event != 0xff)
//    //     {
//    //          rt_kprintf("The system is busying...\n");
//    //          return 0xff;
//    //     }
//    //     else
//    //     {
//    //          exboard.cur_out_event = cur_cmd;                 //外部写事件，作上下文用
//    //          exboard.cur_out_clientnum = clientnum;
//    //					rt_thread_resume(exboard.tid);
//    //          return 0;
//    //     }
//    sprintf(p, "{\"type\":\"slot\",\"num\":\"%d\",\"cmd\":\"FF|%d|FF\",\"id\":\"%ld\"}", clientnum, (rt_uint8_t)cur_cmd, id);
//    id++;
//    MQTT_Publish_From_Server((rt_uint8_t *)p, sizeof(p));
//    return 0;
//}

//void exboard_thread_create(void)
//{
//    //rt_thread_t tid;

//    exboard.cur_out_event = 0xff;

//    exboard.mq = rt_mq_create("exboardmq", BOARD_MQ_MAX, 10, 0);

//    exboard.tid = rt_thread_create("exboard", exboard_thread_entry, RT_NULL,
//                                   2048, RT_APPS_PRIORITY, 15);
//    if (exboard.tid != RT_NULL) {
//        rt_thread_startup(exboard.tid);
//    }
//    rt_memset(data_led, 0, INODES_MAX_EXBOARD);
//    //	  tid = rt_thread_create("exstate",
//    //          Exstate, RT_NULL,
//    //          512, RT_THREAD_PRIORITY_MAX/3, 24);
//    //    if (tid != RT_NULL) rt_thread_startup(tid);
//}


//rt_uint8_t inodes_virtual_write(rt_uint8_t clientid, rt_uint8_t status, rt_uint8_t *keyid)
//{
//    rt_uint32_t i = 0;
//    rt_uint8_t *virtual_write_p = &data_map_buf[0];

//    if ((clientid > 24) || (clientid == 0)) {
//        return 0xff;
//    }

//    virtual_write_p += (clientid - 1) * INODES_KEY_MODULE_LEN;
////     virtual_tmp_p += (clientid - 1) * INODES_KEY_MODULE_LEN;
//    if (status == 0x02) {
//        //读超时10次才修改数据
//        single_time_out_flag[clientid - 1]++;
//        // rt_kprintf("clientid %d time out %d\r\n",clientid,single_time_out_flag[clientid - 1]);
//        if (single_time_out_flag[clientid - 1] >= 10) {
////               rt_memcpy(virtual_tmp_p, virtual_write_p, INODES_KEY_MODULE_LEN);
//            *virtual_write_p++ = status;
//            for (i = 0; i < 5; i++) {
//                *virtual_write_p++ = keyid[i];
//            }
//            single_time_out_flag[clientid - 1] = 0;
//        }
//    }
//    else if (status == 0x12 || status == 0x13) {
//        single_time_out_flag[clientid - 1] = 0;
//        single_channel_flag[clientid - 1]++;
//        // rt_kprintf("clientID %d error,add flag,time %d\r\n",clientid,single_channel_flag[clientid - 1]);
//        if (single_channel_flag[clientid - 1] >= 3) {
////               rt_memcpy(virtual_tmp_p, virtual_write_p, INODES_KEY_MODULE_LEN);
//            *virtual_write_p++ = status;

//            for (i = 0; i < 5; i++) {
//                *virtual_write_p++ = keyid[i];
//            }
//            single_channel_flag[clientid - 1] = 0;
//        }
//    }
//    else if (status == 0x10 || status == 0x11) {
//        // rt_kprintf("right clientID,write to virtual\r\n");
////          rt_memcpy(virtual_tmp_p, virtual_write_p, INODES_KEY_MODULE_LEN);
//        single_channel_flag[clientid - 1] = 0;
//        single_time_out_flag[clientid - 1] = 0;
//        *virtual_write_p++ = status;

//        for (i = 0; i < 5; i++) {
//            *virtual_write_p++ = keyid[i];
//        }
//    }

////     *virtual_write_p++ = status;

////     for(i = 0; i < 5; i++)
////          *virtual_write_p++ = keyid[i];

//    return 0;
//}

////id从1开始编号
//rt_uint8_t inodes_virtual_read(rt_uint8_t clientid, rt_uint8_t *status, rt_uint8_t *keyid)
//{
//    rt_uint32_t i = 0;
//    rt_uint8_t *virtual_read_p = &data_map_buf[0];

//    if ((clientid > 24) || (clientid == 0)) {
//        return 0xff;
//    }

//    virtual_read_p += (clientid - 1) * INODES_KEY_MODULE_LEN;
//    *status = *virtual_read_p++;

//    for (i = 0; i < 8; i++) {
//        keyid[i] = *virtual_read_p++;
//    }

//    return 0;
//}

//rt_int8_t inodes_virtual_cmp(void)
//{
//#if 0
//    rt_kprintf("data map:\r\n");
//    for (int i = 0; i < 24 * INODES_KEY_MODULE_LEN; i++) {
//        if ( i % INODES_KEY_MODULE_LEN == 0) {
//            rt_kprintf("client %d:", (i / INODES_KEY_MODULE_LEN + 1));
//        }

//        rt_kprintf("%x,", data_map_buf[i]);
//    }
//    rt_kprintf("\r\ndata map temp:\r\n");
//    for (int i = 0; i < 24 * INODES_KEY_MODULE_LEN; i++) {
//        if ( i % INODES_KEY_MODULE_LEN == 0) {
//            rt_kprintf("client %d:", (i / INODES_KEY_MODULE_LEN + 1));
//        }
//        rt_kprintf("%x,", data_map_buf_temp[i]);
//    }
//    rt_kprintf("\r\n");
//#endif
//    if (rt_memcmp(data_map_buf, data_map_buf_temp, 24 * INODES_KEY_MODULE_LEN)) {
//        rt_memcpy(data_map_buf_temp, data_map_buf, 24 * INODES_KEY_MODULE_LEN);
//        return -1;
//    }
//    return 0;
//}

//#endif //INODES_HAVE_ETH_BT

