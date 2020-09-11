/*********************************************************************
Copyright (c) 2014 by Automation Interactive.
All rights reserved. You are not allowed to
copy or distribute the code without permission.
*********************************************************************/
#include "inodes_include.h"

///////////////////////////////////////
//
////////////////////////////////////////
//unsigned char send_100data[DataBufLen] = { 0 };
//unsigned char UartRXdata[DataBufLen] = { 0 };

unsigned char send_100data[DataBufLen]; //__attribute__((at(0x20007C48)));
unsigned char UartRXdata[DataBufLen]; //__attribute__((at(0x20008048)));

unsigned int PublishID = 1;
char mqtt_rp = 0;
//const char clientID[] = "yishanServer123456";
//#define clientIDLen (18)
//const char topicName[] = "apptopic";
//#define topicNameLen (8)
const char Willmessage[] = "{}";
#define WillmessageLen (2)
//const char username[] = "adminwls";
#define usernameLen strlen(username)
//const char password[] = "Inodes2716";
//#define passwordLen (10)
const unsigned char ProtocolName[] = { 0, 4, 'M', 'Q', 'T', 'T', 4 }; // 0 4 MQTT 4
#define Message_Identifier_Len 2

MQTTPacket Connectdata = MQTTPacket_connectData_initializer; //
PublishPacket PowerOnPublishPacket = MQTTPacket_PublishData_initializer; //
UTF8 PowerOnPubtopicName = { 0, NULL };                     //
UTF8 SubtopicName = { 0, NULL };                            //
SubscribePacket ClientSubscribePacket;                      //


/////////////////////////////////////////////////////////////////////
//基础函数
/////////////////////////////////////////////////////////////////////
void WriteChar(unsigned char **pptr, char c)
{
    **pptr = c;
    (*pptr)++;
}

void WriteInt(unsigned char **pptr, int anInt)
{
    **pptr = (unsigned char)(anInt / 256);
    (*pptr)++;
    **pptr = (unsigned char)(anInt % 256);
    (*pptr)++;
}

void WriteCString(unsigned char **pptr, const char *string)
{
    int len = strlen(string);
    WriteInt(pptr, len);
    memcpy(*pptr, string, len);
    *pptr += len;
}


void MQTTwriteLen(unsigned char **pptr, unsigned char *buf, int length)
{
    int rc = 0;
    do {
        char d = length % 128;
        length /= 128;
        /* if there are more digits to encode, set the top bit of this digit */
        if (length > 0)
            d |= 0x80;
        buf[rc++] = d;
        (*pptr)++;
    } while (length > 0);
}


/****************************************************************************************
//MQTT_connect_Packet_Send
****************************************************************************************/
void MQTT_connect_Packet(MQTTPacket *Connectdata)
{
    nextai_uint32 p_addr;
    Connectdata->header.bits.type = CONNECT;
    //
    Connectdata->MQTTVersion = (char *) ProtocolName;
    Connectdata->ConnectFlags.all = 0xC2;
    Connectdata->keepAliveInterval = 0x0014;
    nextai_setting_getproperty(SETTING_ADDR_SERIAL_NUM, &p_addr);
    Connectdata->clientID.data = (char *)p_addr;
    Connectdata->clientID.len = strlen(Connectdata->clientID.data);//clientIDLen; //????????????????,?????????8?MCU ?????
    //Connectdata->topicName.data = (char*) topicName;
    //Connectdata->topicName.len = topicNameLen;
    //Connectdata->message.data = (char*) Willmessage;
    //Connectdata->message.len = WillmessageLen;
    //Connectdata->username.data = (char*) username;
    nextai_setting_getproperty(SETTING_ADDR_MQTT_USERNAME, &p_addr);
    Connectdata->username.data = (char *)p_addr;
    Connectdata->username.len = strlen(Connectdata->username.data);//usernameLen;
    //Connectdata->password.data = (char*) password;
    nextai_setting_getproperty(SETTING_ADDR_MQTT_PASSWORD, &p_addr);
    Connectdata->password.data = (char *)p_addr;
    Connectdata->password.len = strlen(Connectdata->password.data);//passwordLen;
    //////////////////////////////////////////////////////
    //Connectdata->PacketLen = 7 + 1 + 2 + clientIDLen + 2 + usernameLen + 2 + passwordLen + 2;
    Connectdata->PacketLen = 7 + 1 + 2 + Connectdata->clientID.len + 2 + Connectdata->username.len + 2 + Connectdata->password.len + 2;
}


char MQTT_connect_Packet_Send(unsigned char *buf, unsigned long *datalen, MQTTPacket *options)
{
    unsigned char *P = buf;
    unsigned int i = 0;
    MQTTHeader header = { 0 };
    //MQTTConnectFlags flags = {0};
    for (i = 0; i < DataBufLen; i++)
        buf[i] = 0; //
    header.byte = 0;
    header.bits.type = CONNECT;
    WriteChar(&P, header.byte);   //
    MQTTwriteLen(&P, P, options->PacketLen); //
    memcpy(P, options->MQTTVersion, 7);
    P += 7; //
    WriteChar(&P, options->ConnectFlags.all); //
    WriteInt(&P, options->keepAliveInterval); //
    WriteCString(&P, options->clientID.data);
    //WriteCString(&P, options->topicName.data);
    //WriteCString(&P, options->message.data);
    WriteCString(&P, options->username.data);
    WriteCString(&P, options->password.data);

    *datalen = P - buf;

    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//?????? 4?? toicName ,PublishID ,PayloadData QOS
//?4????? PublishPacket* options ????
void MQTT_Publish_Packet(PublishPacket *options, unsigned short cur_publishid, UTF8 *PubtopicName,
                         unsigned char Qos, char *PayloadData, unsigned int srcdatalen)
{
    options->header.bits.type = PUBLISH;
    options->header.bits.qos = Qos;
    //
    options->topicName.len = PubtopicName->len;
    options->topicName.data = PubtopicName->data;
    if (options->header.bits.qos != 0)
        options->PublishID = cur_publishid;
    else
        options->PublishID = 0;
    options->PayloadData = PayloadData;
    if (options->header.bits.qos != 0)
        options->PacketLen = 2 + strlen(PubtopicName->data) + Message_Identifier_Len + srcdatalen;
    else
        options->PacketLen = 2 + strlen(PubtopicName->data) + srcdatalen;
}



char MQTT_Publish_Packet_send(unsigned char *buf, unsigned int srcdatalen, unsigned long *datalen, PublishPacket *options)
{
    unsigned char *P = buf;
    unsigned int i = 0;
    for (i = 0; i < DataBufLen; i++)
        buf[i] = 0; //???????

    WriteChar(&P, options->header.byte);   //????????
    MQTTwriteLen(&P, P, options->PacketLen); // ????????????
    WriteCString(&P, options->topicName.data); //?????????
    if (options->PublishID != 0)
        WriteInt(&P, options->PublishID);       // ???????
    memcpy(P, options->PayloadData, srcdatalen);
    P += srcdatalen;//strlen(options->PayloadData);

    *datalen = P - buf;

    return 0; // ????
}


void MQTT_Subscribe_Packet(SubscribePacket *options, UTF8 *SubtopicName,
                           unsigned char SubQos)
{
    options->header.bits.type = SUBSCRIBE;
    options->header.bits.qos = 1;
    options->PublishID = PublishID;
    options->topicName.len = SubtopicName->len;
    options->topicName.data = SubtopicName->data;
    options->SubQos = SubQos;
    options->PacketLen = 2 + 2 + strlen(SubtopicName->data) + 1;
}


char MQTT_Subscribe_Packet_send(unsigned char *buf, unsigned long *datalen, SubscribePacket *options)
{
    unsigned char *P = buf;
    unsigned int i = 0;
    for (i = 0; i < DataBufLen; i++)
        buf[i] = 0;                              //
    WriteChar(&P, options->header.byte);          //
    MQTTwriteLen(&P, P, options->PacketLen);      //
    WriteInt(&P, options->PublishID);             //
    WriteCString(&P, options->topicName.data);    //
    WriteChar(&P, options->SubQos);               //

    *datalen = P - buf;
    return 0;    //
}
/////////////////////////////////////////////////
// char* MQTT_Receive_UnPack(char*buf)
// ??????????,??????,?????????? ?????????????
// ??????????
////////////////////////////////////////////////
void MQTT_Packet_UnPack(unsigned char *buf, unsigned int len, Return_data *ReturnData)
{
    MQTTHeader ReceHead = { 0 };
    ReceHead.byte = buf[0];
    ReturnData->myhead = ReceHead.bits.type;
    ReturnData->head.byte = ReceHead.byte;
    ReturnData->receivedLen = buf[1];
    ReturnData->dataH = buf[2];
    ReturnData->dataL = buf[3];
    if (len > 4 && len == 5)
        ReturnData->Mqos = buf[4];

    switch (ReturnData->myhead) {
        case CONNACK:
            if (ReturnData->head.byte == 0x20 && ReturnData->receivedLen == 2) {
                ReturnData->RxErr = NEXTAI_TRUE;
            } else
                ReturnData->RxErr = NEXTAI_FALSE;
            break;
        case PUBACK:
            if (ReturnData->head.byte == 0x40 && ReturnData->receivedLen == 2) {
                ReturnData->RxErr = NEXTAI_TRUE;
            } else
                ReturnData->RxErr = NEXTAI_FALSE;
            break;
        case PUBREC:
            if (ReturnData->head.byte == 0x50 && ReturnData->receivedLen == 2) {
                ReturnData->RxErr = NEXTAI_TRUE;
            } else
                ReturnData->RxErr = NEXTAI_FALSE;
            break;
        case PUBREL:
            if (ReturnData->head.byte == 0x60 && ReturnData->receivedLen == 2) {
                ReturnData->RxErr = NEXTAI_TRUE;
            } else
                ReturnData->RxErr = NEXTAI_FALSE;
            break;
        case PUBCOMP:
            if (ReturnData->head.byte == 0x70 && ReturnData->receivedLen == 2) {
                ReturnData->RxErr = NEXTAI_TRUE;
            } else
                ReturnData->RxErr = NEXTAI_FALSE;
            break;
        case SUBACK:
            if (ReturnData->head.byte == 0x90 && ReturnData->receivedLen == 3) {
                ReturnData->RxErr = NEXTAI_TRUE;
            } else
                ReturnData->RxErr = NEXTAI_FALSE;
            break;
        case UNSUBACK:
            if (ReturnData->head.byte == 0xB0 && ReturnData->receivedLen == 2
                && (ReturnData->dataH * 256 + ReturnData->dataL) == (int) PublishID) {
                ReturnData->RxErr = NEXTAI_TRUE;
            } else
                ReturnData->RxErr = NEXTAI_FALSE;
            break;
        case PINGRESP:
            if (ReturnData->head.byte == 0xD0 && ReturnData->receivedLen == 0) {
                ReturnData->RxErr = NEXTAI_TRUE;
            } else {
                ReturnData->RxErr = NEXTAI_FALSE;
            }
            break;
        default:
            ReturnData->RxErr = NEXTAI_FALSE;
    }
}

void MQTT_Publish_UnPack(unsigned char *buf, unsigned int len, Return_data *ReturnData)
{
    // unsigned char Qos_value = 0;
    unsigned int datalen = 0;
    unsigned short topic_len = 0;
    unsigned char *cur_p = buf;
    unsigned char i, j;

    //Qos_value = buf[0] & 0x06;

    for (j = 0, i = 1, datalen = buf[i] & 0x7F; (buf[i] & 0x80) != 0; i++) {
        datalen |= ((buf[i + 1] & 0x7F) << (7 * i)) ;
        j++;
    }

    datalen += j;

    if (len != datalen + 2) {
        ReturnData->RxErr = NEXTAI_FALSE;
        return;
    }

    cur_p += 2;
    cur_p += j;
    topic_len = buf[2 + j] << 8 | buf[3 + j];
    cur_p += 2;
    cur_p += topic_len;

    MQTT_Publish_From_Server(cur_p, (len - topic_len - 4));	//取出来自服务器的发布数据
}

unsigned char MQTT_Receive_UnPack(unsigned char *buf, unsigned int len, Return_data *ReturnData)
{
    MQTTHeader ReceHead = { 0 };
    unsigned char ret = 0;
    ReceHead.byte = buf[0];
    mqtt_rp = ReceHead.bits.type;

//     if (buf[1] <= 85)
    {
        switch (mqtt_rp) {
            case CONNACK:
                MQTT_Packet_UnPack(buf, len, ReturnData);
                break;
            case PUBLISH:
                MQTT_Publish_UnPack(buf, len, ReturnData);
                break;
            case PUBACK:
                MQTT_Packet_UnPack(buf, len, ReturnData);
                break;
            case PUBREC:
                MQTT_Packet_UnPack(buf, len, ReturnData);
                break;
            case PUBREL:
                MQTT_Packet_UnPack(buf, len, ReturnData);
                break;
            case PUBCOMP:
                MQTT_Packet_UnPack(buf, len, ReturnData);
                break;
            case SUBACK:
                MQTT_Packet_UnPack(buf, len, ReturnData);
                break;
            case UNSUBACK:
                MQTT_Packet_UnPack(buf, len, ReturnData);
                break;
            case PINGRESP:
                if ((buf[0] == 0xd0) && (0 == buf[1])) {
                    ReturnData->RxErr = NEXTAI_TRUE;
                } else {
                    ReturnData->RxErr = NEXTAI_FALSE;
                }
                break;
            default:
                return 0xff;
        }
    }
//     else
//          ret = 0xff;

    return ret;
}


////////////////////////////////////////////////////////////
//// MQTT_Publish
//// options
//// PubtopicName
//// Qos
//// PayloadData
///////////////////////////////////////////////////////////
unsigned char MQTT_Publish_PUBREL(unsigned short publish_id)
{
    send_100data[0] = 0x62;
    send_100data[1] = 0x02;
    send_100data[2] = (publish_id >> 8) & 0xFF;
    send_100data[3] = (publish_id) & 0xFF;
    return 4;
}

//MQTT发布重发函数，将DUP置1，返回需要发送的长度
unsigned short MQTT_Publish_Resend(void)
{
    unsigned char i, j;
    unsigned short datalen;
    PublishPacket *options = (PublishPacket *)send_100data;
    //将DUP位置1
    options->header.bits.dup = 1;
    //取得需重发的数据长度
    for (j = 0, i = 1, datalen = send_100data[i] & 0x7F; (send_100data[i] & 0x80) != 0; i++) {
        datalen |= ((send_100data[i + 1] & 0x7F) << (7 * i)) ;
        j++;
    }
    datalen += (j + 2);

    return datalen;
}

////////////////////////////////////////////////////////////
//// MQTT_Publish
//// options
//// PubtopicName
//// Qos
//// PayloadData
///////////////////////////////////////////////////////////
void MQTT_Publish(PublishPacket *options, unsigned short publish_id, UTF8 *PubtopicName, unsigned char Qos, char *PayloadData, unsigned int srcdatalen, unsigned long *datalen)
{
    MQTT_Publish_Packet(options, publish_id, PubtopicName, Qos, PayloadData, srcdatalen); // ??????options
    MQTT_Publish_Packet_send(send_100data, srcdatalen, datalen, options); //?????
}

////////////////////////////////////////////////////////////
//// MQTT_Subscribe  ????
//// options????
//// SubtopicName ???
//// SubQos????    ????
///////////////////////////////////////////////////////////
void MQTT_Subscribe(SubscribePacket *options, UTF8 *SubtopicName, unsigned char SubQos, unsigned long *datalen)
{
    MQTT_Subscribe_Packet(options, SubtopicName, SubQos);
    MQTT_Subscribe_Packet_send(send_100data, datalen, options);
}
////////////////////////////////////////////////////////////
//// MQTT_Subscribe  ????
//// Connectdata  ?????
////
///////////////////////////////////////////////////////////
void MQTT_connect(MQTTPacket *Connectdata, unsigned long *datalen)
{
    MQTT_connect_Packet(Connectdata); // ????? Connectdata
    MQTT_connect_Packet_Send(send_100data, datalen, Connectdata); //??????? ?????
}

////////////////////////////////////////////////////////////
//// ?????
////
///////////////////////////////////////////////////////////
void MQTT_heart(MQTTPacket *options, unsigned long *datalen)
{
    UNUSED(options);
    unsigned char *P = &send_100data[0];
    //unsigned int i = 0;
    MQTTHeader header = { 0 };
    header.byte = 0;
    header.bits.type = PINGREQ;
    WriteChar(&P, header.byte);   //????????
    WriteChar(&P, 0);

    *datalen = 2;
}


unsigned char *MQTT_get_databuf(void)
{
    return &send_100data[0];
}

////////////////////////////////////////////////////////////
////
////////////////////////////////////////////////////////////
#if 0
char mqtt(void)
{
    MQTTPacket Connectdata = MQTTPacket_connectData_initializer;
    MQTT_connect_Packet(&Connectdata);
    MQTT_connect_Packet_Send(send_100data, &Connectdata); //??????? ?????
    PublishPacket PowerOnPublishPacket = MQTTPacket_PublishData_initializer;
    UTF8 PowerOnPubtopicName = { 0, NULL };
    PowerOnPubtopicName.data = "/FA/N220/ACCF23967F53/HW1.0/SW1.0/connected";
    PowerOnPubtopicName.len = strlen("/FA/N220/ACCF23967F53/HW1.0/SW1.0/connected");
    char PayloadData[] = "{}";
    MQTT_Publish_Packet(&PowerOnPublishPacket, &PowerOnPubtopicName, 1, PayloadData);
    MQTT_Publish_Packet_send(send_100data, &PowerOnPublishPacket);
    UTF8 SubtopicName = { 0, NULL };
    SubscribePacket ClientSubscribePacket;
    SubtopicName.data = "/client/FA/ACCF23967F53/#";
    SubtopicName.len = strlen("/client/FA/ACCF23967F53/#");
    MQTT_Subscribe_Packet(&ClientSubscribePacket, &SubtopicName, 2);
    MQTT_Subscribe_Packet_send(send_100data, &ClientSubscribePacket);
    return 0;
}

#endif

