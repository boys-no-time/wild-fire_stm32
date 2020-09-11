/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved.
You are not allowed to copy or distribute the code without permission.
*********************************************************************/
#include "inodes_include.h"

#ifdef INODES_HAVE_ETH_BT
#error "maybe need to move your code handling data into here"

#define SUPPORT_WRITE_FLASH   0

#if SUPPORT_WRITE_FLASH
#define TX_SIZE (256)
#else
#define TX_SIZE (16)
#endif

#define REV_SIZE    (32)
#define SYNC_BYTE   (0x16)

#define POS_SYNC    0x00
#define POS_ADDR    0x01
#define POS_CMD     0x02
#define POS_PAYLOAD 0x03


#define CMM_TIMEOUT  8

#define CMM_MQ_MAX    24
#define CMM_MQ_DEEPTH 20

#define TARGET_LOST_TIME 10
#define PUBLISH_INTERNAL  (100*60*5)    //5Mins


#define UPDATE_NORMAL   1
#define UPDATE_URGENT   2


void list_mem(void);

enum {
	CMM_IDLE = 0,
	CMM_START,
	CMM_SEND,
	CMM_WAIT,
	CMM_WAITNEXT,
	CMM_RESP,
	CMM_DONE,

};

enum {
	MSTATE_START = 0,
	MSTATE_HOST,
	MSTATE_POLL,
	MSTATE_WAIT_POLL,
	MSTATE_POLL_NEXT,
	MSTATE_PUBLISH,
	MSTATE_SEND_HCMD,
};


struct _tTransCtrlBlockDef {
	int8_t cmd;
	int8_t addr;
	int8_t retry;
	int8_t error;
	uint8_t rev_buf[24];

	union PARAM {
		uint8_t byte[4];
		uint16_t word[2];
		uint32_t dword;
		int32_t result;
	} param;
};

typedef struct _tBoardData {
	uint8_t sw_state;
	uint8_t rfid[5];
} __attribute__((packed))  BoardData  ;

struct _tSubBoardDef {
	int8_t downCnt;
	int8_t targetLost;
	int8_t update;
	int8_t rev;
	union BD_DATA {
		uint8_t buf[24];
		BoardData sbb_data[4];
	} bd_data;
} __attribute__((packed)) ;

static struct _tTransCtrlBlockDef Transblk = { 0 };
static struct _tSubBoardDef SubBoard[TOTAL_SUBBOARD] = { 0 };
static rt_mq_t CmmMq;
static rt_mq_t PbMq;

static uint8_t tx_buf[TX_SIZE] MEM_ALIGNED;
static uint8_t rx_buf[REV_SIZE] MEM_ALIGNED;
static int8_t cmm_mq_buf[CMM_MQ_MAX] MEM_ALIGNED;
//static uint8_t publishdata[PUBLISH_DATA_SIZE] MEM_ALIGNED;


static void pack_publishdata(uint8_t *publish, struct _tSubBoardDef *board)
{
	int i, j, n;
	uint8_t *ptr;
	int8_t flag;
	BoardData  *pBdata;

	memset(publish, 0, PUBLISH_DATA_SIZE);
	ptr = publish;
	n = 1;
	*ptr ++  = '{';

	for (i = 0; i < TOTAL_SUBBOARD; i++) {
		if (board->targetLost) {
			for (j = 0; j < 4; j++) {
				sprintf((char *)ptr, "n_%d:0000000000,", n);
				ptr += strlen((char *)ptr);
				n++;
			}
		}
		else {
			for (j = 0; j < 4; j++) {
				pBdata = &(board->bd_data.sbb_data[j]);
				flag = (pBdata->rfid[0]) || (pBdata->rfid[1]) || (pBdata->rfid[2]) || (pBdata->rfid[3]) || (pBdata->rfid[4]) ? 1 : 0;
				if (pBdata->sw_state == 0) {
					sprintf((char *)ptr, "n_%d:0000000000,", n);
					ptr += strlen((char *)ptr);
					n++;
				}

				else  if (flag == 0) {
					sprintf((char *)ptr, "n_%d:FFFFFFFFFF,", n);
					ptr += strlen((char *)ptr);
					n++;
				}
				else {
					sprintf((char *)ptr, "n_%d:%02X%02X%02X%02X%02X,", n, pBdata->rfid[0], pBdata->rfid[1], pBdata->rfid[2], pBdata->rfid[3], pBdata->rfid[4]);
					ptr += strlen((char *)ptr);
					n++;
				}
			}
		}

		board ++;
	}
	ptr--;
	*ptr = '}';
	return ;
}

/* API called in other place */
rt_uint16_t create_publish_data(rt_uint8_t *p)
{
	pack_publishdata(p, SubBoard);
	return (rt_uint16_t)strlen((const char *)p);
}

static int32_t com_tx_pack(uint8_t *buf, struct _tTransCtrlBlockDef *tblk)
{
	uint8_t crc;
	int32_t len = 0;
	switch (tblk->cmd) {
	case CMD_GETVER:
	case CMD_GETSW:
	case CMD_GETSTATUS:
		len = 3;
		break;

	case CMD_GETRFID:
	case CMD_EXELOCK:
	case CMD_EXELED:
		len = 4;
		buf[POS_PAYLOAD] = tblk->param.byte[0];
		break;

	case CMD_CTRLSLOT:
		len = 6;
		buf[POS_PAYLOAD] = tblk->param.byte[0];
		buf[POS_PAYLOAD + 1] = tblk->param.byte[1];
		buf[POS_PAYLOAD + 2] = tblk->param.byte[2];
		break;
	}

	buf[POS_SYNC] = SYNC_BYTE;
	buf[POS_ADDR] = tblk->addr;
	buf[POS_CMD] = tblk->cmd;

	crc = Get_CRC8(buf, len);
	buf[len++] = crc;

#if 0
	{
		int i;
		DPRINTF(("[CMM] TX cmd/addr=%d/%d: ", tblk->cmd, tblk->addr));
		for (i = 0; i < len; i++) {
			DPRINTF(("0x%x ", buf[i]));
		}
		DPRINTF((" \n"));
	}
#endif

	return len;
}

static int32_t com_rx_unpack(uint8_t *buf, int32_t len,  struct _tTransCtrlBlockDef *tblk)
{
	uint8_t *payload;
	payload = &buf[POS_PAYLOAD];
	len -= 4;
	int32_t ret = 0;
	int i, n;
	BoardData   *pbd = 0;

	tblk->param.result = 0;

	switch (tblk->cmd) {
	case CMD_GETVER:
		if (len <= 0) {
			ret = -1;
			break;
		}
		if (len > 24) {
			len = 23;
			payload[23] = 0;
		}
		strcpy((char *)tblk->rev_buf, (char *)payload);
		break;

	case CMD_GETSW:
		if (len != 1) {
			EPRINTF(("[CMM] Get SW return error.\n"));
			ret = -1;
			break;
		}
		tblk->param.result = (int32_t) payload[0];
		break;

	case CMD_GETSTATUS:
		if (len != 21) {
			EPRINTF(("[CMM] Status from %d error.\n", tblk->addr));
			ret = -1;
			break;
		}
		pbd  = (BoardData *) tblk->rev_buf;
		n = 1;

#if 0
		DPRINTF(("[CMM] RX cmd/addr=%d/%d: ", tblk->cmd, tblk->addr));
		for (i = 0; i < 21; i++) {
			DPRINTF(("0x%x ", payload[i]));
		}
		DPRINTF((" \n"));
#endif

		for (i = 0; i < 4; i++) {
			pbd->sw_state = (payload[0] & (0x01 << i)) ? 1 : 0;
			memcpy(&pbd->rfid[0], &payload[n], 5);
			pbd++;
			n += 5;
		}

		break;

	case CMD_EXELOCK:
	case CMD_EXELED:
	case CMD_CTRLSLOT:
		if (len != 1) {
			EPRINTF(("[CMM] Issue Cmd %d to board return error.\n", tblk->cmd, tblk->addr));
			ret = -1;
			break;
		}
		if (payload[0] != 0) {
			EPRINTF(("[CMM] Issue Cmd %d to board %d may be wrong, err =%d. \n", tblk->cmd, tblk->addr, payload[0]));
		}

		break;
	}
	return ret;
}

void CmmMaster_task(void *parameter)
{
	UNUSED(parameter);

	int i;

	int8_t needUpdate = 0;
	int8_t cmmState =  CMM_IDLE ;
	int8_t mainState = MSTATE_START;
	uint8_t PollTarget = 0;
	uint8_t CtrlTarget = 0;
	uint8_t led_status = 0;
	int16_t sd_len = 0;
	int16_t rev_len = 0;
	tick_t cmmTimeout = 0;
	tick_t publishTime = 0;
	tick_t DelayTime = 0;
	int8_t unlock_led = 0;
	uint32_t unlock_led_stop_time  = 0;
#ifdef INODES_HAVE_LED_CTRL
	int8_t door_status = DOOR_CLOSE;
	int8_t door_check = 0;
	int8_t door_lastlevel = 0;
	int8_t door_curlevel = 0;
	int8_t door_led_on = 0;
	int8_t lost_target = 0;
	uint32_t door_checktime = 0;
#endif


	SERVERCMDTYPEDEF *ServerCmd = 0;

	do {

		if (unlock_led) {
			if (GetOSTick() > unlock_led_stop_time) {
				inodes_dataout_set(NEXTAI_SIGN_LED_UNLOCK, 0);
				unlock_led = 0;
			}
		}

#ifdef INODES_HAVE_LED_CTRL
		if (GetOSTick() > door_checktime) {
			door_curlevel = Board_GetDoorLevel();
			if (door_curlevel == door_lastlevel) {
				door_check ++;
				if (door_check > 3) {
					if (door_status != door_curlevel) {
						door_status = door_curlevel;
						if ((door_status == DOOR_CLOSE) && door_led_on) {
							door_led_on = 0;
							DOOR_LED_UNLOCK_OFF ;
							if (unlock_led) {
								inodes_dataout_set(NEXTAI_SIGN_LED_UNLOCK, 0);
								unlock_led = 0;
							}

						}
					}
					door_check  = 0;
				}
			}
			else {
				door_check  = 0;
			}
			door_lastlevel = door_curlevel;
			door_checktime = GetOSTick() + 1;
		}

		if (unlock_led == 0) {
			if ((door_status == DOOR_CLOSE) && (door_led_on)) {
				door_led_on = 0;
				DOOR_LED_UNLOCK_OFF ;
			}
			if ((door_status == DOOR_OPEN) && (door_led_on != 2)) {
				if (door_led_on == 0) {
					DOOR_LED_UNLOCK_ON;
					door_led_on  = 2;
				}
				else if (door_led_on == 1) {
					DOOR_LED_FLASH_OFF ;
					DOOR_LED_UNLOCK_ON;
					door_led_on  = 2;
				}
			}
		}
#endif


		switch (cmmState) {
		case CMM_IDLE:

			break;

		case CMM_START:
			sd_len = com_tx_pack(tx_buf, &Transblk);
			if (sd_len <= 0) {
				cmmState = CMM_IDLE;
				break;
			}
			cmmState = CMM_SEND;
			break;

		case CMM_SEND:
			if (HalCom_TxBusy()) {
				break;
			}
			HalCom_Write((int8_t *)tx_buf, (int32_t)sd_len);
			OSSleep(3);
			if ((Transblk.cmd == CMD_ERASE_FLASH) || (Transblk.cmd == CMD_WR_FLASH)) {
				cmmTimeout  = GetOSTick() + (tick_t)(CMM_TIMEOUT * 10) ;
			}
			else {
				cmmTimeout  = GetOSTick() + (tick_t) CMM_TIMEOUT ;
			}
			cmmState = CMM_WAIT;
			break;

		case CMM_WAIT:
			if (GetOSTick() > cmmTimeout) {
				if (Transblk.retry > 0) {
					OSSleep(10);
					HalCom_ResetRx();
					Transblk.retry --;
					cmmState = CMM_SEND;
					break;
				}
				EPRINTF(("[CMM] Board %d timeout.\n\r", Transblk.addr));
				Transblk.error = ERROR_TIMEOUT;
				cmmState = CMM_DONE;
				break;
			}
			if (HalCom_GetSize() == 0) {
				break;
			}
			DelayTime  = GetOSTick() + (tick_t) 2;
			cmmState = CMM_RESP;
			break;

		case CMM_RESP:
			if (GetOSTick() < DelayTime) {
				break;
			}
			rev_len = (int16_t) HalCom_Read((int8_t *) rx_buf, REV_SIZE);
			if (Get_CRC8((uint8_t *) rx_buf, (int32_t)rev_len)) {
				EPRINTF(("[CMM] CRC error.\n"));
				Transblk.error = ERROR_CRC;
				cmmState = CMM_DONE;
				break;
			}
			if ((rx_buf[POS_SYNC] != SYNC_BYTE) || (rx_buf[POS_ADDR] != 0x00)) {
				EPRINTF(("[CMM] Respone error.\n"));
				Transblk.error = ERROR_RESP;
				cmmState = CMM_DONE;
				break;
			}


			if (com_rx_unpack(rx_buf, rev_len, &Transblk) < 0) {
				EPRINTF(("[CMM] Parse respone data error.\n"));
				Transblk.error = ERROR_PARSE;
				cmmState = CMM_DONE;
				break;
			}

			cmmState = CMM_DONE;
			break;

		case CMM_DONE:
			break;
		}

		switch (mainState) {
		case MSTATE_START:
			//DPRINTF(("[CMM] Mstate start ..\n"));
			if (cmmState != CMM_IDLE) {
				break;
			}
			memset(&cmm_mq_buf[0], 0, CMM_MQ_MAX);
			if (rt_mq_recv(CmmMq, &cmm_mq_buf, CMM_MQ_MAX, 3) == RT_EOK) {
				mainState = MSTATE_HOST;
				break;
			}

			mainState = MSTATE_POLL;
			break;

		case MSTATE_POLL:
			memset((void *) &Transblk, 0, sizeof(Transblk));
			Transblk.cmd = CMD_GETSTATUS;
			Transblk.addr = PollTarget + 1;
			cmmState = CMM_START;
			mainState = MSTATE_WAIT_POLL;
			break;

		case MSTATE_WAIT_POLL:
			if (cmmState != CMM_DONE) {
				break;
			}
			cmmState  = CMM_IDLE;
			if (Transblk.error) {
				if (SubBoard[PollTarget].targetLost == 0) {
					SubBoard[PollTarget].downCnt ++;
					if (SubBoard[PollTarget].downCnt >= TARGET_LOST_TIME) {
						SubBoard[PollTarget].targetLost = 1;
						SubBoard[PollTarget].downCnt  = 0;
						SubBoard[PollTarget].update = UPDATE_NORMAL;
						memset(SubBoard[PollTarget].bd_data.buf, 0, 6 * 4);
#ifdef INODES_HAVE_LED_CTRL
						if (Hal_Led_GetAct()) {
							Hal_Led_SetAct(LED_OFF);
						}
#endif
					}
				}
			}
			else {
				if (SubBoard[PollTarget].targetLost) {
					SubBoard[PollTarget].targetLost = 0;
					SubBoard[PollTarget].update = UPDATE_NORMAL;
				}
				SubBoard[PollTarget].downCnt  = 0;
				if (memcmp(Transblk.rev_buf, SubBoard[PollTarget].bd_data.buf, 6 * 4) != 0) {
					SubBoard[PollTarget].update = UPDATE_NORMAL;
					memcpy(SubBoard[PollTarget].bd_data.buf, Transblk.rev_buf, 6 * 4);
				}
			}

			mainState = MSTATE_POLL_NEXT;
			break;


		case MSTATE_POLL_NEXT:
			PollTarget ++;
			needUpdate  = 0;

#ifdef INODES_HAVE_LED_CTRL
			lost_target  = 0;
			for (i = 0; i < TOTAL_SUBBOARD; i++) {
				if (SubBoard[i].update == UPDATE_URGENT) {
					needUpdate  = 1;
				}
				if (SubBoard[i].targetLost) {
					lost_target = 1;
				}
			}

			if ((lost_target == 0) && (Hal_Led_GetAct() == LED_OFF)) {
				Hal_Led_SetAct(LED_ON);
			}
#else
			for (i = 0; i < TOTAL_SUBBOARD; i++) {
				if (SubBoard[i].update == UPDATE_URGENT) {
					needUpdate  = 1;
					break;
				}
			}
#endif

			if (needUpdate) {
				if (PollTarget >= TOTAL_SUBBOARD) {
					PollTarget  = 0;
				}
				mainState = MSTATE_PUBLISH;
				break;
			}

			if (PollTarget >= TOTAL_SUBBOARD) {
				PollTarget  = 0;
				needUpdate  = 0;
				led_status = (led_status) ? 0 : 1;
				inodes_dataout_set(NEXTAI_SIGN_LED_ACT, led_status);
				for (i = 0; i < TOTAL_SUBBOARD; i++) {
					if (SubBoard[i].update) {
						needUpdate  = 1;
						break;
					}
				}
				if (needUpdate || (GetOSTick() - publishTime > PUBLISH_INTERNAL)) {
					mainState = MSTATE_PUBLISH;
					break;
				}
				OSSleep(20);
			}
			else {
				OSSleep(50);
			}

			//list_mem();

			mainState = MSTATE_START;
			break;

		case MSTATE_PUBLISH:
#if 0 //not do here, send event to client only
			if (MQTT_isConnected()) {
				pack_publishdata(&publishdata[0], &SubBoard[0]);
				if (rt_mq_send(PbMq, &publishdata[0], PUBLISH_DATA_SIZE) == RT_EOK) {
					publishTime  = GetOSTick();
					for (i = 0; i < TOTAL_SUBBOARD; i++) {
						if (SubBoard[i].update) {
							SubBoard[i].update = 0;
						}
					}
				}
				else {
					EPRINTF(("[CMM] Fail to send publish to MQTT client.\n"));
				}
			}
#else
			DPRINTF(("[CMM] send DO_PUBLISH! \n"));
			rt_event_send(mqtt_get_ev(), MQTT_EVENT_DO_PUBLISH);
			for (i = 0; i < TOTAL_SUBBOARD; i++) {
				if (SubBoard[i].update) {
					SubBoard[i].update = 0;
				}
			}
#endif
			OSSleep(50);
			mainState = MSTATE_START;
			break;

		case MSTATE_HOST:
			ServerCmd  = (SERVERCMDTYPEDEF *) &cmm_mq_buf[0];
			memset((void *) &Transblk, 0, sizeof(Transblk));
			Transblk.cmd = CMD_CTRLSLOT;
			Transblk.retry = 3;
			CtrlTarget = ((ServerCmd->num - 1) >> 2) & 0xff;
			Transblk.addr = CtrlTarget + 1;
			Transblk.param.byte[0] = ((ServerCmd->num  - 1) & 0x03) ;
			Transblk.param.byte[1] = ServerCmd->led;
			Transblk.param.byte[2] = ServerCmd->lock;
			if (ServerCmd->lock == 0) {
				inodes_dataout_set(NEXTAI_SIGN_LED_UNLOCK, 1);
				unlock_led = 1;
				unlock_led_stop_time = GetOSTick() + 9000;
#ifdef INODES_HAVE_LED_CTRL
				door_led_on  = 1;
				DOOR_LED_FLASH_ON ;
#endif
			}
			cmmState = CMM_START;
			mainState = MSTATE_SEND_HCMD;
			break;

		case MSTATE_SEND_HCMD:
			if (cmmState != CMM_DONE) {
				break;
			}
			cmmState  = CMM_IDLE;

			if (Transblk.error) {
				if (SubBoard[CtrlTarget].targetLost == 0) {
					SubBoard[CtrlTarget].targetLost = 1;
					SubBoard[CtrlTarget].downCnt  = 0;
					SubBoard[CtrlTarget].update = UPDATE_URGENT;
					memset(SubBoard[PollTarget].bd_data.buf, 0, 6 * 4);
				}
				char *sendbuf = rt_malloc(64);
				sprintf(sendbuf, "{num:%d,status:0,errorcode:10,id:%ld}",
				        ServerCmd->num, ServerCmd->cmdid);

				MQTT_Send2Clientmq(sendbuf, strlen(sendbuf) + 1);
				rt_free(sendbuf);
				mainState = MSTATE_PUBLISH;
				break;
			}
			else {
				char *sendbuf = rt_malloc(64);
				sprintf(sendbuf, "{num:%d,status:1,errorcode:00,id:%ld}",
				        ServerCmd->num, ServerCmd->cmdid);
				MQTT_Send2Clientmq(sendbuf, strlen(sendbuf) + 1);
				rt_free(sendbuf);
				PollTarget = CtrlTarget;
				SubBoard[CtrlTarget].update = UPDATE_URGENT;
			}
			mainState = MSTATE_POLL;
			break;
		}
		//rt_thread_yield();
	}
	while (1);
}

static int8_t pbmq_valid = 0;
static int8_t cmmmq_valid = 0;
void CmmMaster_Start(void)
{
	rt_thread_t tid;

	CmmMq = rt_mq_create("cmmmq", CMM_MQ_MAX, CMM_MQ_DEEPTH, 0);
	if (CmmMq)  {
		cmmmq_valid = 1;
	}
	else {
		EPRINTF(("[CMM] Fail to create cmmmq.\n"));
	}
	PbMq = rt_mq_create("pbmq", PUBLISH_DATA_SIZE, 1, 0);
	if (PbMq) {
		pbmq_valid = 1;
	}
	else {
		EPRINTF(("[CMM] Fail to create pbmq.\n"));
	}
	tid = rt_thread_create("cmmtask", CmmMaster_task, RT_NULL, 1024, RT_APPS_PRIORITY-1, 15);
	if (tid != RT_NULL) {
		rt_thread_startup(tid);
	}

}

rt_mq_t CmmMaster_GetPbMq(void)
{
	if (pbmq_valid) {
		return PbMq;
	}
	else {
		return NULL;
	}
}

int32_t CmmMaster_SendServerCmd(void *ServerCmd)
{
	if (cmmmq_valid == 0) {
		return -1;
	}
	if (rt_mq_send(CmmMq, ServerCmd, sizeof(SERVERCMDTYPEDEF)) != RT_EOK) {
		return -2;
	}
	return 0;
}

#endif

