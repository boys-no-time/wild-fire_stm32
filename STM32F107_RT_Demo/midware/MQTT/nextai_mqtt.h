
#ifndef _NEXTAI_MQTT__H_
#define _NEXTAI_MQTT__H_


#define MQTTPacket_connectData_initializer {{0},0,NULL,{0},0,{0,NULL},{0,NULL},{0,NULL},{0,NULL},{0,NULL}}
#define MQTTPacket_PublishData_initializer {{0},0,{0,NULL},0,NULL}
#define MQTTPacket_SubscribeData_initializer {{0},0,0,{0,NULL},0}
#define DataBufLen  1024


//ConnectData PacketTest;
enum msgTypes {
    CONNECT = 1,
    CONNACK = 2,
    PUBLISH = 3,
    PUBACK,
    PUBREC,
    PUBREL,
    PUBCOMP,
    SUBSCRIBE,
    SUBACK,
    UNSUBSCRIBE,
    UNSUBACK,
    PINGREQ,
    PINGRESP,
    DISCONNECT
};
// This is a guard condition so that contents of this file are not included
// more than once.
///////////////////////////////////////////////////////////////////////
////??????
////////////////////////////////////////////////////////////////////////
typedef union {
    unsigned char byte; /**< the whole byte */
    struct {
        unsigned int retain : 1; /**< retained flag bit */
        unsigned int qos : 2; /**< QoS value, 0, 1 or 2 */
        unsigned int dup : 1; /**< DUP flag bit */
        unsigned int type : 4; /**< message type nibble */
    } bits;
} MQTTHeader;	  //????
///////////////////////////////////////////////////////////////////////
////???????
///////////////////////////////////////////////////////////////////////
typedef union {
    unsigned char all; /**< all connect flags */
    struct {
        unsigned int usernamebit : 1; /**< 3.1 user name */
        unsigned int passwordbit : 1; /**< 3.1 password */
        unsigned int willRetain : 1; /**< will retain setting */
        unsigned int willQoS : 2; /**< will QoS value */
        unsigned int will : 1; /**< will flag */
        unsigned int cleansession : 1; /**< clean session flag */
        unsigned int : 1; /**< unused */
    } bits;
} MQTTConnectFlags;	//?????
///////////////////////////////////////////////////////////////////////
////
///////////////////////////////////////////////////////////////////////
typedef struct {
    int len;          //
    char *data;       //
} UTF8;
///////////////////////////////////////////////////////////////////////
typedef struct {
    MQTTHeader header;
    unsigned char PacketLen;        //?? char ?? int ?? long
    char *MQTTVersion;        // LENl,LENh,mqtt,Version  = 04MQTT4
    MQTTConnectFlags ConnectFlags;
    unsigned int keepAliveInterval;
    UTF8 clientID;
    UTF8 topicName;
    UTF8 message;
    UTF8 username;
    UTF8 password;
} MQTTPacket;
//////////////////////////////////////////////////////////////////////
typedef struct {
    MQTTHeader header;
    unsigned int PacketLen;        //?? char ?? int ?? long
    UTF8 topicName;
    unsigned int PublishID;
    char *PayloadData;
} PublishPacket;
//////////////////////////////////////////////////////////////////////
typedef struct {
    MQTTHeader header;
    unsigned char PacketLen;        //?? char ?? int ?? long
    unsigned int PublishID;
    UTF8 topicName;
    unsigned char SubQos;
} SubscribePacket;
typedef struct {
    _Bool RxErr;  //????????? 1  ?? 0
    unsigned char myhead; //??????? = enum msgTypes???
    MQTTHeader head;
    unsigned char receivedLen;
    unsigned char dataH;
    unsigned char dataL;
    unsigned char Mqos;
} Return_data;        //???ACK?????????

#ifdef __cplusplus
extern "C"
{
#endif

void MQTT_connect(MQTTPacket *Connectdata, unsigned long *datalen);

char MQTT_connect_Packet_Send(unsigned char *buf, unsigned long *datalen, MQTTPacket *options);

void MQTT_Publish_Packet(PublishPacket *options, unsigned short cur_publishid, UTF8 *PubtopicName,
                         unsigned char Qos, char *PayloadData, unsigned int srcdatalen);

char MQTT_Publish_Packet_send(unsigned char *buf, unsigned int srcdatalen, unsigned long *datalen, PublishPacket *options);

void MQTT_Subscribe_Packet(SubscribePacket *options, UTF8 *SubtopicName,
                           unsigned char SubQos);

char MQTT_Subscribe_Packet_send(unsigned char *buf, unsigned long *datalen, SubscribePacket *options);

unsigned char MQTT_Receive_UnPack(unsigned char *buf, unsigned int len, Return_data *ReturnData);

void MQTT_Publish(PublishPacket *options, unsigned short publish_id, UTF8 *PubtopicName, unsigned char Qos, char *PayloadData, unsigned int srcdatalen, unsigned long *datalen);

void MQTT_Subscribe(SubscribePacket *options, UTF8 *SubtopicName, unsigned char SubQos, unsigned long *datalen);

void MQTT_heart(MQTTPacket *options, unsigned long *datalen);

unsigned short MQTT_Publish_Resend(void);

unsigned char MQTT_Publish_PUBREL(unsigned short publish_id);

unsigned char *MQTT_get_databuf(void);

#ifdef __cplusplus
}
#endif


#endif

