
#ifndef __COM_MASTER_H__
#define __COM_MASTER_H__

#include "mqtt_client.h"

#define CMD_GETVER       0x01
#define CMD_GETSW        0x02
#define CMD_GETRFID      0x03
#define CMD_EXELOCK      0x05
#define CMD_EXELED       0x06
#define CMD_GETSTATUS    0x07
#define CMD_CTRLSLOT     0x08

#define CMD_ERASE_FLASH  0x10
#define CMD_WR_FLASH     0x11

#define NO_ERROR        0
#define ERROR_TIMEOUT   1
#define ERROR_CRC       2
#define ERROR_RESP      3
#define ERROR_PARSE     4

#define TOTAL_SUBBOARD      6
#define PUBLISH_DATA_SIZE   (TOTAL_SUBBOARD *4 * 24)

rt_mq_t CmmMaster_GetPbMq(void);
int32_t CmmMaster_SendServerCmd(void *ServerCmd);
void CmmMaster_Start(void);


struct Mqtt2Cmm_CtrlBlock {
    int32_t type;
    union {
        SERVERCMDTYPEDEF servercmd;
    };
};

#endif


