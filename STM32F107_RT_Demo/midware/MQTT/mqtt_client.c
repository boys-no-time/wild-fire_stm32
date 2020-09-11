#include "inodes_include.h"

#define INODES_PUBTOPIC_HEAD "/Keybox/Notice/"		//发布主题名头缀
#define INODES_SUBTOPIC_HEAD "/Server/Command/"	    //订阅主题头头缀
#define INODES_ANSWER_HEAD "/Keybox/ReCommand/"		//回复主题头头缀

/* moved here to avoid public access randomly! */
struct _MQTT_CLIENT_TYPEDEF_ {
	//以下是SOCK有关的操作
#ifndef INODES_NO_MUTEX
	struct rt_mutex mtx;				//SOCK使用互斥信号量，未得到或已释放此信号量时切记不可操作SOCK
#endif
	struct hostent *host;
	struct sockaddr_in server_addr;
	rt_int32_t sock;
	//以下是发送与接收的缓冲定义
	rt_uint32_t bytes_recv;
	rt_uint8_t recv[RECV_MAX_BYTE];			//接收到的原始数据
	rt_uint32_t bytes_send;
	rt_uint8_t send[SEND_MAX_BYTE];			//待发数据
	rt_int32_t timeout;						//超时时间
	rt_uint16_t heartcut;					//心跳间隔时间计数
	rt_uint16_t heart_time_out_count;		//心跳间隔超时次数

	rt_uint32_t interval;					//发布间隔时间计数，为零时会进行发布

	rt_uint16_t publish_id_num;				//MQTT发布报文标识符
	//以下内容为有关于MQTT线程的内核对象
	rt_thread_t client;	//专管从连线服务器到发出第一个PUBLISH
	rt_thread_t serve;	//专管连线后与其他人交互、服务
	rt_uint8_t clientstatus;
	/*消息队列*/
	rt_mq_t mq;
	rt_uint8_t mqbuf[CLIENT_MQ_MAX];	  //收发消息的缓冲区
	/*事件队列*/
	rt_event_t ev;
};

extern rt_err_t rt_stm32_eth_init(rt_device_t dev);

MQTT_CLIENT_TYPEDEF MQTT_client;
rt_uint8_t mqtt_answer(rt_uint8_t *data, rt_uint32_t datalengh);
void mqtt_heart(void);

/* 与其他线程共同操作的部分，本文件外一概调用这些API */
rt_mq_t mqtt_get_mq()
{
	return MQTT_client.mq;
}

rt_event_t mqtt_get_ev()
{
	return MQTT_client.ev;
}

/* not called
rt_uint8_t mqtt_get_clientstatus()
{
	rt_uint8_t copy_cs; //copy MQTT_client.clientstatus
	MUTEX_TAKE(&MQTT_client.mtx, RT_WAITING_FOREVER);
	copy_cs = MQTT_client.clientstatus;
	MUTEX_RELEASE(&MQTT_client.mtx);
	return copy_cs;
}

void mqtt_set_clientstatus(rt_uint8_t cs)
{
	rt_kprintf("[MQTT] set client status to %d, old=%d\r\n", cs, MQTT_client.clientstatus);
	MUTEX_TAKE(&MQTT_client.mtx, RT_WAITING_FOREVER);
	MQTT_client.clientstatus = cs;
	MUTEX_RELEASE(&MQTT_client.mtx);
}
*/

/////////////////////////////////////////////////////////////////

#ifndef INODES_HAVE_NEW_SBB
rt_uint16_t create_publish_data(rt_uint8_t *p)
{
	rt_uint16_t lengh = 0;
	rt_uint8_t i;
	rt_uint8_t status;
//    rt_uint8_t led;
//    rt_uint8_t lock = 0;
//	rt_uint8_t keyid[5];
//	rt_uint8_t buf[11];
//	char key_n[32];

//	rt_memset(p, 0, 32 * 24);
//	*p = '{';
//	for (i = 1; i <= 24; i++) {
//		rt_memset(keyid, 0, 5);
//		rt_memset(buf, 0, 10);
//		//				rt_memset(keyid,'0',5);
//		inodes_virtual_read(i, &status, &keyid[0]);
//		sprintf((char *)buf, "%02X%02X%02X%02X%02X", keyid[0], keyid[1], keyid[2], keyid[3], keyid[4]);
//		if ((status == 0x10) || (status == 0x00)) {
//			//钥匙栓不存在
//			strcpy((char *)buf, "0000000000");
//		}
//		else if ((status == 0x12) || (status == 0x13)) {
//			//钥匙栓存在但读不到ID
//			strcpy((char *)buf, "FFFFFFFFFF");
//		}
//		rt_kprintf("publish buf is %s\r\n", buf);
//		//				led = (status>>2)&3;
//		//				lock = (status>>4)&1;
//		sprintf(key_n, "n_%d:%s,", i, buf);
//		strcat((char *)p, key_n);
//	}
	lengh = strlen((char *)p);
	p[lengh - 1] = '}';
	return lengh;
}
#else
extern rt_uint16_t create_publish_data(rt_uint8_t *p);
#endif

/* 实践证明LWIP的select()不靠谱... TMD
#define SOCK_READABLE	0x01
#define SOCK_WRITABLE	0x02
#define SOCK_EXCEPTED	0x04
static rt_int32_t mqtt_select(rt_int32_t sock, int sec)
{
	rt_int32_t ret;
	fd_set rfds, wfds, efds; //read,write,exception
	struct timeval timeout = {0, 0}; //要非阻塞就都置0
	timeout.tv_sec = sec;
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);
	FD_SET(sock,&rfds);
	FD_SET(sock,&wfds);
	FD_SET(sock,&efds);
	ret = select(sock+1, &rfds, &wfds, &efds, &timeout);
	if(ret < 0) return 0;
	ret = 0;
	if(FD_ISSET(sock, &rfds)) {
		ret |= SOCK_READABLE;
	}
	if(FD_ISSET(sock, &wfds)) {
		ret |= SOCK_WRITABLE;
	}
	if(FD_ISSET(sock, &efds)) {
		ret |= SOCK_EXCEPTED;
	}
	return ret;
}
*/

/* return value: RT_EOK: success  -RT_ERROR: failure */
static rt_int8_t mqtt_do_publish(rt_uint8_t *publishdata)
{
	rt_uint32_t getsetting_p;
	char inodes_topic_buf[64];
	PublishPacket publish_buf = {0};
	UTF8 publish_topic;

	MQTT_client.interval = setting_para_temp.interval;
	MQTT_client.heartcut = HEART_TIME;
	nextai_setting_getproperty(SETTING_ADDR_SERIAL_NUM, &getsetting_p);
	strcpy(inodes_topic_buf, INODES_PUBTOPIC_HEAD);
	strcat(inodes_topic_buf, (char *)getsetting_p);
	publish_topic.data = inodes_topic_buf;
	publish_topic.len = strlen(publish_topic.data);

	MQTT_client.publish_id_num ++;
	if (MQTT_client.publish_id_num == 0) {
			MQTT_client.publish_id_num = 1;
	}

	create_publish_data(publishdata);

	MQTT_Publish(&publish_buf, MQTT_client.publish_id_num, &publish_topic, 1, (char *)publishdata, strlen((char *)publishdata), (unsigned long *)&MQTT_client.bytes_send);

	rt_kprintf("======> MQTT publishing: %s\r\n", publishdata);
	if (send(MQTT_client.sock, MQTT_get_databuf(), MQTT_client.bytes_send, MSG_WAITALL) != -1) {
		rt_kprintf("=====> MQTT publish send success\r\n");
		return RT_EOK;
	}
	else {
		rt_kprintf("===> MQTT publish fail 1\r\n");
		return -RT_ERROR;
	}
}

#ifdef INODES_USING_SWDOG
static rt_int8_t serve_dog;
#endif

#define ERROR_CONTINUE(delay)	MUTEX_RELEASE(&MQTT_client.mtx); \
								rt_thread_delay(delay); \
								error_count ++; \
								continue
#define	MAX_ERROR_COUNT			(10)
#define ERROR_DELAY				(200)
#define SEND_DELAY				(200)
								
#ifdef __CC_ARM
#pragma diag_suppress 111 //statement is unreachable
#endif

//MQTT客户端线程(专管联网)
void mqtt_client(void *parameter)
{
	UNUSED(parameter);
	rt_uint32_t getsetting_p, timeout = 3000;
	Return_data error_data = {0};
	static rt_uint8_t error_count = 0;
	static rt_uint16_t wait_count = 1;
	char inodes_topic_buf[64];
	SubscribePacket subscribe_buf = {0};
	UTF8 subscribe_topic;
	rt_uint8_t *publishdata = rt_malloc(32 * 24);
	rt_thread_t te = rt_thread_find("ephy");

	rt_kprintf("[thread] mqtt_client starting...\r\n");
	MQTT_client.clientstatus = MQTT_CLIENT_STATUS_NONE;

	while (1) {
		/* handling status of errors */
		if(error_count >= MAX_ERROR_COUNT) {
#if 1
			//出错太多，重启
			NVIC_SystemReset();
#else
			//出错太多，设为初始状态，等网卡连接通知. 让网络模组干点啥???
			rt_thread_timeout(reset_network_thread);
			MQTT_client.clientstatus = MQTT_CLIENT_STATUS_NONE;
			error_count = 0;
			rt_thread_yield();
			continue;
#endif
		}

		rt_kprintf("[ephy] eth thread stat = %d(1-READY,2-SUSPEND/BLOCK,3-RUNNING,4-CLOSE)\r\n", te->stat);
		//初始状态下,等待物理连接完成唤醒
		while (MQTT_client.clientstatus == MQTT_CLIENT_STATUS_NONE) {
			rt_uint32_t e;
			rt_thread_delay(ERROR_DELAY); //RT_TICK_PER_SECOND is 100
			//检查有没有网卡上线消息
			if (rt_event_recv(MQTT_client.ev,
			                  MQTT_EVENT_NET_CHANGED | MQTT_EVENT_NET_ONLINE,
			                  RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_NO,
				&e) == RT_EOK) 
			{
				rt_kprintf("[mqtt_client] event got online 0x%x\r\n", e);
				MQTT_client.clientstatus = MQTT_CLIENT_STATUS_PHY_LINKUP;
				rt_kprintf("==================> link up\r\n");
			}
			/* trigger resetting after 30*MAX_ERROR_COUNT seconds */
			wait_count ++;
			if((wait_count % 30) == 0) { //30 seconds as error
				error_count ++;
			}
			rt_thread_yield();
		}

		/* 一系列常规处理,直至_ALLDONE、挂起(等待断网唤醒) */
		MUTEX_TAKE(&MQTT_client.mtx, RT_WAITING_FOREVER);
		switch(MQTT_client.clientstatus) {
		case MQTT_CLIENT_STATUS_NONE:
			MUTEX_RELEASE(&MQTT_client.mtx);
			break;
		case MQTT_CLIENT_STATUS_PHY_LINKUP:
			rt_kprintf("[PHY_LINKUP] ======> try %d\r\n", error_count + 1);
			/*域名解析*/
			nextai_setting_getproperty(SETTING_ADDR_MQTT_IPADDR, &getsetting_p);
			if ((MQTT_client.host = gethostbyname((char *)getsetting_p)) == NULL) {
				rt_kprintf("======> client get host name fail\r\n");
#ifdef SAVE_FIRST_IP
				{
					/**获取IP*/
					nextai_setting_getproperty(SETTING_ADDR_MQTT_IPADDR_2, &getsetting_p);
					if (NULL == getsetting_p || 0 == strlen(getsetting_p)) {
						rt_kprintf("======> mqtt ip not exist\r\n");
						ERROR_CONTINUE(ERROR_DELAY);
					}
					rt_kprintf("======> mqtt ip is %s\r\n", getsetting_p);
					if ((MQTT_client.host = gethostbyname((char *)getsetting_p)) == NULL) {
						rt_kprintf("======> mqtt ip saved fail\r\n");
						ERROR_CONTINUE(ERROR_DELAY);
					}
					rt_kprintf("======> mqtt ip saved suc:%s\r\n", getsetting_p);
				}
#else
				{
					/* 不使用保存的IP */
					ERROR_CONTINUE(ERROR_DELAY);
				}
#endif
			}
			else { //success
#ifdef SAVE_FIRST_IP
				{
					//存储IP到flash中备用
					nextai_setting_data *me = (nextai_setting_data *)&setting_para_temp;
					if (NULL == me->remoteIP || 0 != strcmp(me->remoteIP, inet_ntoa( *(struct in_addr*)MQTT_client.host->h_addr_list[0] ))) {
						/* 设置IP */
						rt_memset(me->remoteIP, 0, sizeof(me->remoteIP));
						strcpy(me->remoteIP, inet_ntoa( *(struct in_addr*)MQTT_client.host->h_addr_list[0] ));
						nextai_setting_save();
						rt_kprintf("======> set ip :%s\r\n", me->remoteIP);
					}
					else {
						/* ignore same IP */
						rt_kprintf("======> ip equal,skip save\r\n");
					}
				}
#endif
			}
			rt_kprintf("======> client get host name success\r\n");
			/*创建一个socket*/
			if ((MQTT_client.sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
				rt_kprintf("======> connect socket create fail\r\n");
				ERROR_CONTINUE(ERROR_DELAY);
			}
			rt_kprintf("======> connect socket create success\r\n");
			/* 因为select()不靠谱，被迫设置socket超时 */
			setsockopt(MQTT_client.sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
			setsockopt(MQTT_client.sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
			/*初始化预连接的服务器地址*/
			MQTT_client.server_addr.sin_family = AF_INET;
			nextai_setting_getproperty(SETTING_ADDR_MQTT_IPPORT, &getsetting_p);
			MQTT_client.server_addr.sin_port = htons((rt_uint16_t)getsetting_p);
			MQTT_client.server_addr.sin_addr = *((struct in_addr *) MQTT_client.host->h_addr);
			rt_memset(&(MQTT_client.server_addr.sin_zero), 0, sizeof(MQTT_client.server_addr.sin_zero));
			rt_kprintf("======> connect IP format success\r\n");
			/* 设置超时时间(不建议这么做)
			MQTT_client.timeout = 5000;
			lwip_setsockopt(MQTT_client.sock, SOL_SOCKET, SO_RCVTIMEO, &MQTT_client.timeout, sizeof(MQTT_client.timeout));
			*/
			/*连接到服务器*/
			rt_kprintf("======> net connecting...\r\n");
			if (connect(MQTT_client.sock, (struct sockaddr *)&MQTT_client.server_addr, sizeof(struct sockaddr)) == -1) {
				rt_kprintf("======> net connect fail, retry %d\r\n", error_count + 1);
				lwip_close(MQTT_client.sock);	//关闭连接，重新创建连接
				ERROR_CONTINUE(ERROR_DELAY);
			}
			else {
#if 1 //debugging
				rt_int32_t ret;
				struct sockaddr_in peer;
				socklen_t len = sizeof(struct sockaddr_in);
				ret = getpeername(MQTT_client.sock, (struct sockaddr *)&peer, &len);
				if(ret != 0) {
					rt_kprintf("===> net getpeername failed! ret=%d\r\n", ret);
				}
				else {
					rt_kprintf("===> net getpeername s_addr=0x%x!\r\n", peer.sin_addr.s_addr);
				}
#endif
				MQTT_client.clientstatus = MQTT_CLIENT_STATUS_CONNECT;
			}
			MUTEX_RELEASE(&MQTT_client.mtx);
			break;
		case MQTT_CLIENT_STATUS_CONNECT:
			MQTT_connect((MQTTPacket *)MQTT_client.send, (unsigned long *)&MQTT_client.bytes_send);
			rt_kprintf("========> MQTT connect...\r\n");
			if (send(MQTT_client.sock, MQTT_get_databuf(), MQTT_client.bytes_send, MSG_WAITALL) != -1) {
				rt_thread_delay(SEND_DELAY);
				rt_kprintf("======> MQTT connect send success\r\n");
				/* check result of CONNECT */
				MQTT_client.bytes_recv = recv(MQTT_client.sock, MQTT_client.recv, RECV_MAX_BYTE, MSG_WAITALL);
				if (MQTT_client.bytes_recv > 0 && MQTT_client.bytes_recv != 0xFFFFFFFF) {
					MQTT_Receive_UnPack(MQTT_client.recv, MQTT_client.bytes_recv, &error_data);
					MQTT_client.bytes_recv = 0;
					if (NEXTAI_TRUE == error_data.RxErr) {
						rt_kprintf("======> MQTT connect success\r\n");
						/* 下面两个变量的设置放这里比较顺眼 */
						MQTT_client.heartcut = 0xFFFF;
						MQTT_client.heart_time_out_count = HEART_TIME_OUT_MAX_COUNT;
						//进入订阅
						MQTT_client.clientstatus = MQTT_CLIENT_STATUS_SUBSCRIBE;
					}
					else {
						rt_kprintf("======> MQTT connect fail 3\r\n");
						ERROR_CONTINUE(ERROR_DELAY);
					}
				}
				else {
					rt_kprintf("======> MQTT connect fail 2\r\n");
					ERROR_CONTINUE(ERROR_DELAY);
				}
			}
			else {
				rt_kprintf("=======> MQTT connect Fail 1\r\n");
#ifdef INODES_HAVE_LED_CTRL
				LED_NET(OFF) ;
#endif
				ERROR_CONTINUE(ERROR_DELAY);
			}
			MUTEX_RELEASE(&MQTT_client.mtx);
			break;
		case MQTT_CLIENT_STATUS_SUBSCRIBE:
			nextai_setting_getproperty(SETTING_ADDR_SERIAL_NUM, &getsetting_p);
			strcpy(inodes_topic_buf, INODES_SUBTOPIC_HEAD);
			strcat(inodes_topic_buf, (char *)getsetting_p);
			subscribe_topic.data = inodes_topic_buf;
			subscribe_topic.len = strlen(subscribe_topic.data);
			MQTT_Subscribe(&subscribe_buf, &subscribe_topic, 0, (unsigned long *)&MQTT_client.bytes_send);
			rt_kprintf("=======> MQTT subscribeing...\r\n");
			if (send(MQTT_client.sock, MQTT_get_databuf(), MQTT_client.bytes_send, MSG_WAITALL) != -1) {
				rt_thread_delay(SEND_DELAY);
				rt_kprintf("======> MQTT subscribe send success\r\n");
				/* check result of SUBSCRIBE */
				MQTT_client.bytes_recv = recv(MQTT_client.sock, MQTT_client.recv, RECV_MAX_BYTE, MSG_WAITALL);
				if (MQTT_client.bytes_recv > 0 && MQTT_client.bytes_recv != 0xFFFFFFFF) {
					MQTT_Receive_UnPack(MQTT_client.recv, MQTT_client.bytes_recv, &error_data);
					MQTT_client.bytes_recv = 0;
					if (NEXTAI_TRUE == error_data.RxErr) {
						rt_kprintf("=====> MQTT subscribe success\r\n");
						//进入发布
						MQTT_client.clientstatus = MQTT_CLIENT_STATUS_PUBLISH;
						MQTT_client.interval = 0;
					}
					else {
						rt_kprintf("======> MQTT subscribe fail 3\r\n");
						ERROR_CONTINUE(ERROR_DELAY);
					}
				}
				else {
					rt_kprintf("======> MQTT subscribe fail 2\r\n");
#ifdef INODES_HAVE_LED_CTRL
					LED_NET(OFF) ;
#endif
					ERROR_CONTINUE(ERROR_DELAY);
				}
			}
			else {
				rt_kprintf("======> MQTT subscribe fail 1\r\n");
				ERROR_CONTINUE(ERROR_DELAY);
			}
			MUTEX_RELEASE(&MQTT_client.mtx);
			break;
		case MQTT_CLIENT_STATUS_PUBLISH:
			if (mqtt_do_publish(publishdata) == RT_EOK) { //success
				rt_thread_delay(SEND_DELAY);
				/* check result of PUBLISH */
				MQTT_client.bytes_recv = recv(MQTT_client.sock, MQTT_client.recv, RECV_MAX_BYTE, MSG_WAITALL);
				if (MQTT_client.bytes_recv > 0 && MQTT_client.bytes_recv != 0xFFFFFFFF) {
					MQTT_Receive_UnPack(MQTT_client.recv, MQTT_client.bytes_recv, &error_data);
					MQTT_client.bytes_recv = 0;
					if (NEXTAI_TRUE == error_data.RxErr) {
						rt_kprintf("====> MQTT publish success\r\n");
						MQTT_client.heartcut = HEART_TIME;
						MQTT_client.clientstatus = MQTT_CLIENT_STATUS_ALLDONE;
					}
					else {
						rt_kprintf("======> MQTT publish fail 3\r\n");
						ERROR_CONTINUE(ERROR_DELAY);
					}
				}
				else {
					rt_kprintf("======> MQTT publish fail 2\r\n");
#ifdef INODES_HAVE_LED_CTRL
					LED_NET(OFF) ;
#endif
					ERROR_CONTINUE(ERROR_DELAY);
				}
			}
			else {
				ERROR_CONTINUE(ERROR_DELAY);
			}
			MUTEX_RELEASE(&MQTT_client.mtx);
			break;
		case MQTT_CLIENT_STATUS_ALLDONE: {
			//PUBLISH成功,本线程挂起
#ifdef INODES_USING_SWDOG
			rt_int8_t index = serve_dog;
			rt_mq_send(swdog_get_mq(), &index, sizeof(index)); //toggle serve dog
#endif
			error_count = 0;
			MQTT_client.clientstatus = MQTT_CLIENT_STATUS_NONE;
			MUTEX_RELEASE(&MQTT_client.mtx);
			rt_kprintf("[thread] mqtt_client suspend and mqtt_serve resume\r\n");
			rt_thread_resume(MQTT_client.serve);
			rt_thread_suspend(rt_thread_self());
			rt_schedule(); //立即进行线?蘡ontext切换
#ifdef INODES_HAVE_LED_CTRL
			LED_NET(ON) ;
#endif
			}
			break;
		default:
			rt_kprintf("========> MQTT client: ran into unknown status...\r\n");
			MUTEX_RELEASE(&MQTT_client.mtx);
			break;
		}
		//顺利完成一些工作，本次让出线程(rt_schedule()后有可能还留在本线程)
		rt_thread_yield();
	}
	rt_free(publishdata);
}

//MQTT服务线程
static rt_uint8_t net_offline = 0;
void mqtt_serve(void *parameter)
{
	UNUSED(parameter);
	Return_data error_data = {0};
	static rt_uint8_t net_led_status = 0, flag_sent = 0;
	rt_uint8_t *publishdata = rt_malloc(32 * 24);
	static rt_time_t tmnow, tm_last;
		
	rt_kprintf("[thread] mqtt_serve starting...\r\n");
#ifdef INODES_USING_SWDOG
	serve_dog = swdog_create(rt_thread_self(), DOG_STATUS_SLEEP);
#endif
	while (1) {
		rt_uint32_t e;
		
		/* network status LED blinking */
		tmnow = rt_tick_get();
		if(tmnow-tm_last > 100) {
			//rt_kprintf("[mqtt_serve] interval=%d tmnow=%d...\r\n", MQTT_client.interval, tmnow);
			tm_last = tmnow;
#ifdef INODES_HAVE_LED_CTRL
			if(net_led_status == 0) {
				LED_NET(ON) ;
				net_led_status = 1;
			}
			else {
				LED_NET(OFF) ;
				net_led_status = 0;
			}
#endif
		}
		
		MUTEX_TAKE(&MQTT_client.mtx, RT_WAITING_FOREVER);

		//检查有没有网卡下线消息
		if (rt_event_recv(MQTT_client.ev,
		                  MQTT_EVENT_NET_CHANGED | MQTT_EVENT_NET_OFFLINE,
		                  RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_NO,
			&e) == RT_EOK) 
		{
			rt_kprintf("[mqtt_serve] event got offline 0x%x\r\n", e);
			MQTT_client.clientstatus = MQTT_CLIENT_STATUS_NONE;
			rt_kprintf("==================> link down!\r\n");
			net_offline = 1;
		}
		else {
			/* 深信服关掉，ETH寄存器并无变化. WTF! 试试查询socket状态 */
			static rt_time_t last_check;
			if(tmnow > last_check + 300) {
				rt_int32_t ret;
#if 0
				rt_uint32_t err = 0;
				socklen_t len = sizeof(rt_int32_t);
				ret = getsockopt(MQTT_client.sock, SOL_SOCKET, SO_ERROR, &err, &len);
				rt_kprintf("==================> link opt err=%d\r\n", err);
				if(ret != 0 || err != 0) {
					rt_kprintf("==================> link abnormal! err=%d\r\n", err);
					net_offline = 1;
				}
#else
				struct sockaddr_in peer;
				socklen_t len = sizeof(struct sockaddr_in);
				ret = getpeername(MQTT_client.sock, (struct sockaddr *)&peer, &len);
				if(ret != 0) {
					rt_kprintf("==================> link getpeername failed! ret=%d\r\n", ret);
					net_offline = 1;
				}
				else {
					rt_kprintf("==================> link getpeername 0x%x!\r\n", peer.sin_addr.s_addr);
				}
#endif
				last_check = tmnow;
			}
		}

		if(net_offline > 0) { //网络状态变了(有几处地方设置)
#ifdef INODES_USING_SWDOG
			rt_int8_t index = serve_dog;
			rt_mq_send(swdog_get_mq(), &index, sizeof(index)); //toggle serve dog
#endif
			rt_kprintf("[thread] mqtt_serve suspend and mqtt_client resume\r\n");
			net_offline = 0;
			MUTEX_RELEASE(&MQTT_client.mtx);
#if 1
			ETH_SoftwareReset();
			while (ETH_GetSoftwareResetStatus() == SET);
#else
			rt_stm32_eth_init(RT_NULL);
#endif
			/* switch thread */
			rt_thread_resume(MQTT_client.client);
			rt_thread_suspend(rt_thread_self());
			rt_schedule(); //立即进行线程contex切换
#ifdef INODES_HAVE_LED_CTRL
			LED_NET(OFF) ;
#endif
			continue;
		}

		//检查有没有PUBLISH event
		if (rt_event_recv(MQTT_client.ev, MQTT_EVENT_DO_PUBLISH, 
			RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_NO, &e) == RT_EOK) 
		{
			rt_kprintf("[mqtt_serve] ====> event got PUBLISH 0x%x\r\n", e);
			if(mqtt_do_publish(publishdata) == RT_EOK) {
				rt_thread_delay(SEND_DELAY);
				MQTT_client.bytes_recv = recv(MQTT_client.sock, MQTT_client.recv, RECV_MAX_BYTE, MSG_WAITALL);
				if (MQTT_client.bytes_recv > 0 && MQTT_client.bytes_recv != 0xFFFFFFFF) {
					rt_kprintf("=====> 1 MQTT serve recv %d byte data\r\n", MQTT_client.bytes_recv);
					//如果接收到数据
					MQTT_Receive_UnPack(MQTT_client.recv, MQTT_client.bytes_recv, &error_data);
					MQTT_client.bytes_recv = 0;
				}
			}
			else {
				//error handling here
				rt_kprintf("event PUBLISH failed...\n");
				net_offline = 1;
			}
		}
		//检查有没有因PUBLISH计数触发
		if (MQTT_client.interval-- == 0) {
			rt_kprintf("[mqtt_serve] ====> periodically PUBLISH...\r\n");
			if(mqtt_do_publish(publishdata) == RT_EOK) {
				rt_thread_delay(SEND_DELAY);
				MQTT_client.bytes_recv = recv(MQTT_client.sock, MQTT_client.recv, RECV_MAX_BYTE, MSG_WAITALL);
				if (MQTT_client.bytes_recv > 0 && MQTT_client.bytes_recv != 0xFFFFFFFF) {
					rt_kprintf("=====> 2 MQTT serve recv %d byte data\r\n", MQTT_client.bytes_recv);
					//如果接收到数据
					MQTT_Receive_UnPack(MQTT_client.recv, MQTT_client.bytes_recv, &error_data);
					MQTT_client.bytes_recv = 0;
				}
			}
			else {
				//error handling here
				rt_kprintf("periodically PUBLISH failed...\n");
				net_offline = 1;
			}
		}

#ifndef INODES_HAVE_LED_CTRL
		net_led_status ++;
		net_led_status &= 0xFF;
		inodes_dataout_set(NEXTAI_SIGN_LED_LINK, net_led_status % 2);
#endif
		rt_memset(MQTT_client.mqbuf, 0, CLIENT_MQ_MAX);
        while (rt_mq_recv(MQTT_client.mq, MQTT_client.mqbuf, CLIENT_MQ_MAX, 50) == RT_EOK) {
			//回复处理
			rt_kprintf("====> MQTT answering:%s\r\n", MQTT_client.mqbuf);
			if (mqtt_answer(MQTT_client.mqbuf, strlen((char *)MQTT_client.mqbuf))) {
				MQTT_client.heartcut = 0xFFFF;
				rt_kprintf("====> MQTT answer fail\r\n");
				lwip_close(MQTT_client.sock);	//关闭连接，重新创建连接
				MQTT_client.clientstatus = MQTT_CLIENT_STATUS_NONE;
#ifdef INODES_HAVE_LED_CTRL
				LED_NET(OFF) ;
#endif
				net_offline = 1;
				//rt_thread_delay(ERROR_DELAY);
				goto next;
			}
			flag_sent = 1;
			rt_thread_delay(SEND_DELAY);
		}

		if(flag_sent == 0) {
			rt_kprintf("====> nothing active. let's try to send heart, tmnow=%d\n", tmnow);
			//心跳包处理(函数内部收取解析了)
			mqtt_heart();
			goto next;
		}

		MQTT_client.bytes_recv = recv(MQTT_client.sock, MQTT_client.recv, RECV_MAX_BYTE, MSG_WAITALL);
		if (MQTT_client.bytes_recv > 0 && MQTT_client.bytes_recv != 0xFFFFFFFF) {
			rt_kprintf("=====> 3 MQTT serve recv %d byte data\r\n", MQTT_client.bytes_recv);
			//如果接收到数据
			MQTT_Receive_UnPack(MQTT_client.recv, MQTT_client.bytes_recv, &error_data);
			MQTT_client.bytes_recv = 0;
			flag_sent = 0;
		}
next:
		MUTEX_RELEASE(&MQTT_client.mtx);
#ifdef INODES_USING_SWDOG
		swdog_feed(serve_dog);
#endif
	}
	rt_free(publishdata);
}

//MQTT回复处理，输入需要回复的数据和长度
rt_uint8_t mqtt_answer(rt_uint8_t *data, rt_uint32_t datalengh)
{
	rt_uint32_t getsetting_p;
	//Return_data error_data = {0};
	PublishPacket publish_buf = {0};
	UTF8 publish_topic;
	char inodes_topic_buf[50];

	nextai_setting_getproperty(SETTING_ADDR_SERIAL_NUM, &getsetting_p);
	strcpy(inodes_topic_buf, INODES_ANSWER_HEAD);
	strcat(inodes_topic_buf, (char *)getsetting_p);
	publish_topic.data = inodes_topic_buf;
	publish_topic.len = strlen(publish_topic.data);

	MQTT_client.publish_id_num++;
	if (MQTT_client.publish_id_num == 0) {
		MQTT_client.publish_id_num = 1;
	}
	rt_kprintf("============= > publish topic is %s,data is %s!\r\n", publish_topic.data, data);
	MQTT_Publish(&publish_buf, MQTT_client.publish_id_num, &publish_topic, 1, (char *)data, datalengh, (unsigned long *)&MQTT_client.bytes_send);

	if (send(MQTT_client.sock, MQTT_get_databuf(), MQTT_client.bytes_send, MSG_WAITALL) == -1) {
		return 0xff;
	}
	return 0;
}

//MQTT心跳处理
void mqtt_heart(void)
{
	Return_data error_data = {0};
	MQTTPacket heart_buf = {0};
	if (MQTT_client.heartcut == 0) {
		MQTT_client.heartcut = HEART_TIME;
		MQTT_heart(&heart_buf, (unsigned long *)&MQTT_client.bytes_send);
		rt_kprintf("---------------> MQTT send heart...\r\n");
		if (send(MQTT_client.sock, MQTT_get_databuf(), MQTT_client.bytes_send, MSG_WAITALL) == -1) {
			MQTT_client.heartcut = 0xFFFF;
			MQTT_client.heart_time_out_count = HEART_TIME_OUT_MAX_COUNT;
			rt_kprintf("+++++++++++++++++> MQTT send heart fail\r\n");
			lwip_close(MQTT_client.sock);	//关闭连接，重新创建连接
			MQTT_client.clientstatus = MQTT_CLIENT_STATUS_NONE;
			net_offline = 1;
#ifdef INODES_HAVE_LED_CTRL
			LED_NET(OFF) ;
#endif
			return;
		}
		rt_kprintf("--------------> MQTT send heart success\r\n");
		rt_thread_delay(SEND_DELAY);
		MQTT_client.bytes_recv = recv(MQTT_client.sock, MQTT_client.recv, RECV_MAX_BYTE, MSG_WAITALL);
		if (MQTT_client.bytes_recv > 0 && MQTT_client.bytes_recv != 0xFFFFFFFF) {
			MQTT_Receive_UnPack(MQTT_client.recv, MQTT_client.bytes_recv, &error_data);
			MQTT_client.bytes_recv = 0;
			if (NEXTAI_TRUE == error_data.RxErr) {
				//接收到心跳应答，恢复参数
				MQTT_client.heart_time_out_count = HEART_TIME_OUT_MAX_COUNT;
				rt_kprintf("-------------------> MQTT recv heart success\r\n");
				return;
			}
		}
		MQTT_client.heart_time_out_count --;
		rt_kprintf("+++++++++++++++> MQTT recv heart fail,%d time\r\n", (HEART_TIME_OUT_MAX_COUNT - MQTT_client.heart_time_out_count));
		if(MQTT_client.heart_time_out_count == 0) {
			MQTT_client.heart_time_out_count = HEART_TIME_OUT_MAX_COUNT;
			MQTT_client.heartcut = 0xFFFF;
			lwip_close(MQTT_client.sock);	//关闭连接，重新创建连接
			MQTT_client.clientstatus = MQTT_CLIENT_STATUS_NONE;
			rt_kprintf("==================> heart out link down\r\n");
#ifdef INODES_HAVE_LED_CTRL
			LED_NET(OFF) ;
#endif
			net_offline = 1;
		}
	}
	else {
		if (MQTT_client.heartcut <= HEART_TIME) {
			MQTT_client.heartcut --;
		}
	}
}

void mqtt_client_create(void)
{
	MQTT_client.heartcut = 0xFFFF;
	MQTT_client.clientstatus = 0;

	MQTT_client.mq = rt_mq_create("mqttMQ", CLIENT_MQ_MAX, 2, RT_IPC_FLAG_FIFO);
	MQTT_client.ev = rt_event_create("mqttEV", RT_IPC_FLAG_FIFO);
	rt_kprintf("\n[MQ] mq/ev 0x%x/0x%x created\n", MQTT_client.mq, MQTT_client.ev);
#ifndef INODES_NO_MUTEX
	rt_mutex_init(&MQTT_client.mtx, "mqttMTX", /*RT_IPC_FLAG_PRIO*/RT_IPC_FLAG_FIFO);
	rt_kprintf("[mutex] mutex 0x%x created\r\n", MQTT_client.mtx);
#endif
#ifdef IAP_SLAVE //online upgrade supported
	inodes_update_init();
#endif

	/* rt_thread_create()后是_INIT状态，不是就绪状态 */
	MQTT_client.client = rt_thread_create("mqtt_client", mqtt_client, RT_NULL,
	                                      1024, RT_APPS_PRIORITY, RT_APPS_THREAD_TICKS);
	if (MQTT_client.client != RT_NULL) {
		rt_kprintf("\n[thread] mqtt_client 0x%x created\n", MQTT_client.client);
		rt_thread_startup(MQTT_client.client);
	}

	MQTT_client.serve = rt_thread_create("mqtt_serve", mqtt_serve, RT_NULL,
	                                     1024, RT_APPS_PRIORITY, RT_SERVE_THREAD_TICKS);
	if (MQTT_client.serve != RT_NULL) {
		rt_kprintf("[thread] mqtt_serve 0x%x created\n\n", MQTT_client.serve);
		rt_thread_startup(MQTT_client.serve);
		/* 一开始没你啥事，先挂起 */
		rt_thread_suspend(MQTT_client.serve);
		rt_schedule(); //立即进行线程context切换
	}
#ifdef INODES_HAVE_ETH_BT
	#error "start devices here???"
	//CmmMaster_Start();
#endif

	return ;
}

rt_int8_t server_cmd_parse(rt_uint8_t *strbuf, SERVERCMDTYPEDEF *cmd)
{
	nextai_setting_data *me = (nextai_setting_data *)&setting_para_temp;
	cJSON *root_p = 0, *cur_p = 0;

	//int ret = 0;
	rt_kprintf("[server_cmd_parse] strbuf : %s\r\n", strbuf);
	if ((0 == strbuf) || (0 == me)) {
		return -1;
	}

	root_p = cJSON_Parse((const char *)strbuf);
	if (0 == root_p) {
		rt_kprintf("json parse error!\r\n");
		return -1;
	}
	//     cJSON_Print(root_p);
	//		 if(strncmp(cJSON_GetObjectItem(root_p,"type")->valuestring, "slot", 4) == 0)
	//		      cmd->cmdtype = 0;
	//		 else if(strncmp(cJSON_GetObjectItem(root_p,"type")->valuestring, "door", 4) == 0)
	//					cmd->cmdtype = 1;
	//		 cmd->num = strtol((const char *)cJSON_GetObjectItem(root_p,"num")->valuestring, 0, 10);
	//		 cur_p = cJSON_GetObjectItem(root_p,"cmd");
	//		 if(cur_p->valuestring)
	//		 {
	//		      rt_uint8_t i;
	//					cmd->check = strtol((const char *)cur_p->valuestring, 0, 16);
	//					for(i=0;i<strlen(cur_p->valuestring);i++)
	//					{
	//					     if(cur_p->valuestring[i]=='|')
	//							 {
	//								    i++;
	//										break;
	//							 }
	//				   }
	//					 cmd->led = strtol((const char *)(cur_p->valuestring+i), 0, 16);
	//					 for(;i<strlen(cur_p->valuestring);i++)
	//					 {
	//					      if(cur_p->valuestring[i]=='|')
	//							  {
	//							       i++;
	//							       break;
	//							  }
	//					  }
	//						cmd->lock = strtol((const char *)(cur_p->valuestring+i), 0, 16);
	//			 }
	//		   cmd->cmdid = strtol((const char *)cJSON_GetObjectItem(root_p,"id"), 0, 10);

#ifdef IAP_SLAVE   // Bad ideas...just hacking code... 
	{
		uint8_t up_cmd = 0;
		char *tmpbuf;
		struct upgrade_msg   *upMsg;
		sys_mbox_t  *up_recvmbox;
		upMsg = inodes_update_get_msg();
		cur_p = root_p->child;
		while (cur_p) {
			if (strncmp(cur_p->string, "cmd", 3) == 0) {
				if (strncmp(cur_p->valuestring, "update", 6) == 0) {
					up_cmd |= 0x01;
				}
			}
			else if (strncmp(cur_p->string, "vers", 4) == 0) {
				strcpy(upMsg->ver, cur_p->valuestring);
				up_cmd |= 0x02;
			}
			else if (strncmp(cur_p->string, "addr", 4) == 0) {
				strcpy(upMsg->server_addr, cur_p->valuestring);
				up_cmd |= 0x04;
			}
			else if (strncmp(cur_p->string, "port", 4) == 0) {
				strcpy(upMsg->server_port, cur_p->valuestring);
				up_cmd |= 0x08;
			}
			else if (strncmp(cur_p->string, "file", 4) == 0) {
				strcpy(upMsg->filename, cur_p->valuestring);
				up_cmd |= 0x10;
			}
			cur_p = cur_p->next;
		}
		if ((up_cmd & 0x1f) == 0x1f) {
			MQTT_Send2Clientmq(RESP_OK, strlen(RESP_OK) + 1);
			up_recvmbox = inodes_update_get_recvmbox();
			if (up_recvmbox) {
				upMsg ->cmd = 0x01;
				sys_mbox_trypost(up_recvmbox, upMsg);
				cJSON_Delete(root_p);
				return 1;
			}
		}

		// rt_kprintf("[zxl]strbuf2 : %s\r\n",strbuf);

		up_cmd = 0;
		cur_p = root_p->child;
		while (cur_p) {
			// rt_kprintf("[zxl]cur_p->string : %s\r\n",cur_p->string);
			if (strncmp(cur_p->string, "cmd", 3) == 0) {
				if (strncmp(cur_p->valuestring, "getvers", 7) == 0) {
					up_cmd = 1;
					break;
				}
			}
			cur_p = cur_p->next;
		}
		rt_kprintf("[server_cmd_parse] up_cmd : %d\r\n", up_cmd);

		if (up_cmd) {
			tmpbuf = rt_malloc(CLIENT_MQ_MAX);
			if (tmpbuf != NULL) {
				sprintf(tmpbuf, "{\"vers\":\"%s\",\"hash\":\"%s\",\"HV\":\"%s\",\"clientId\":\"%s\",\"interval\":%d,\"localaddr\":\"%s\",\"gateway\":\"%s\",\"mask\":\"%s\",\"dns\":\"%s\",\"remoteaddr\":\"%s\",\"remoteport\":%d,\"mqtt_username\":\"%s\",\"mqtt_password\":\"%s\"}",
				        git_ver, git_info, board_getver(), me->mqtt_clientid, me->interval, me->localaddr, me->gateway, me->mask, me->dns, me->remoteaddr, me->remoteport, me->mqtt_username, me->mqtt_password);
				rt_kprintf("[server_cmd_parse] %s\n", MQTT_client.send);
				MQTT_Send2Clientmq(tmpbuf, strlen(tmpbuf));
				rt_kprintf("[server_cmd_parse] sent to server\n");
				rt_free(tmpbuf);
				cJSON_Delete(root_p);
				return 2;
			}
			else {
				rt_kprintf("Error! not enough memory @ %s %d \n", __FILE__, __LINE__);
				return -1;
			}
		}
	}
#endif
	// rt_kprintf("[zxl]strbuf2 : %s\r\n",strbuf);

	cur_p = root_p->child;
	// rt_kprintf("[zxl]cur_p : %s\r\n",cur_p);
	while (cur_p) {
		if (strncmp(cur_p->string, "type", 4) == 0) {
			if (strncmp(cur_p->valuestring, "slot", 4) == 0) {
				cmd->cmdtype = 0;
			}
			else if (strncmp(cur_p->valuestring, "door", 4) == 0) {
				cmd->cmdtype = 1;
			}
		}
		else if (strncmp(cur_p->string, "num", 3) == 0) {
			if (cur_p->valuestring) {
				cmd->num = strtol((const char *)cur_p->valuestring, 0, 10);
			}
		}
		else if (strncmp(cur_p->string, "cmd", 3) == 0) {
			if (cur_p->valuestring) {
				rt_uint8_t i;
				cmd->check = strtol((const char *)cur_p->valuestring, 0, 16);
				for (i = 0; i < strlen(cur_p->valuestring); i++) {
					if (cur_p->valuestring[i] == '|') {
						i++;
						break;
					}
				}
				cmd->led = strtol((const char *)(cur_p->valuestring + i), 0, 16);
				for (; i < strlen(cur_p->valuestring); i++) {
					if (cur_p->valuestring[i] == '|') {
						i++;
						break;
					}
				}
				cmd->lock = strtol((const char *)(cur_p->valuestring + i), 0, 16);
			}
		}
		else if (strncmp(cur_p->string, "id", 2) == 0) {
			if (cur_p->valuestring) {
				cmd->cmdid = strtol((const char *)cur_p->valuestring, 0, 10);
			}
		}

		cur_p = cur_p->next;
	}

	cJSON_Delete(root_p);

	return 0;
}

void MQTT_Publish_From_Server(rt_uint8_t *recv_buf, unsigned int recv_len)
{

	rt_int8_t flag;
	SERVERCMDTYPEDEF *cmdbuf = rt_malloc(sizeof(SERVERCMDTYPEDEF));
	recv_buf[recv_len] = 0;
	rt_kprintf("=======> recv:%s\r\n", recv_buf);
	flag = server_cmd_parse(recv_buf, cmdbuf);
	// rt_kprintf("=======> flag:%d\r\n",flag);

	// rt_kprintf("=======> cmdtype:%d\r\n",cmdbuf->cmdtype);
	// rt_kprintf("=======> num:%d\r\n",cmdbuf->num);
	// rt_kprintf("=======> check:%d\r\n",cmdbuf->check);
	// rt_kprintf("=======> led:%d\r\n",cmdbuf->led);
	// rt_kprintf("=======> lock:%d\r\n",cmdbuf->lock);
	// rt_kprintf("=======> cmdid:%d\r\n",cmdbuf->cmdid);

	if (flag == 0) {
#ifdef INODES_HAVE_ETH_BT
        #error "handling cmd from MQTT server here"
		if (CmmMaster_recvServerCmd(cmdbuf) < 0) {
			char *sendbuf = rt_malloc(64);
			sprintf(sendbuf, "{num:%d,status:0,errorcode:10,id:%ld}",
			        cmdbuf->num, cmdbuf->cmdid);
			MQTT_Send2Clientmq(sendbuf, strlen(sendbuf) + 1);
			rt_free(sendbuf);
		}
#else
//		rt_mq_send(exboard.mq, cmdbuf, sizeof(SERVERCMDTYPEDEF));
#endif
	}
	rt_free(cmdbuf);
}

void MQTT_Send2Clientmq(char *buffer, rt_size_t size)
{
	rt_mq_send(MQTT_client.mq, buffer, size);
}

#ifdef INODES_HAVE_ETH_BT
int MQTT_isConnected(void)
{
	return (MQTT_client.client->stat == RT_THREAD_SUSPEND);
}
#endif

