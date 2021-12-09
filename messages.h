#ifndef MESSAGES_H
#define MESSAGES_H
#include <tcp.h>

struct strMsgHeader
{
    quint8 msgType;
    quint8 payloadLength[2];
    quint8 rezerv;
} ;

struct strARSmsg  // len = 4 + 6 + 4= 14
{
    struct strMsgHeader header;//0-3
    quint8 arsMsgType;// 4
    quint8 userId[4];// 5-8
    quint8 csbk; // 9
    quint8 req_id[4];
};

typedef enum {
    INIT_STATE = 0,
    READY, // ==1
    RX,
    RX_HT,
    WAIT_CALL_REPLY,
    WAIT_CALL_REPORT,
    WAIT_STOP_CALL_REPLY,
    WAIT_STOP_CALL_HANGIN,
    WAIT_STOP_CALL_ENDED,
    TXT_NOTIFIC,
    BRD_TXT_NOTIFIC,
    TXT_TRANSMIT,
    WAIT_RELOAD_REPLY,
    WAIT_CTRL_REPLY,
    WAIT_CH_CHANGE_REPLY,
    WAIT_CH_CHANGE_STATUS,
    PTT_PRESS_AN,
    PTT_RELEASE_AN
} Device_State;



typedef enum {
    DATA = 0x00,
    CONNECT = 0xFE,//Slave uses this Opcode to connect to HRNP radio network.
    ACCEPT = 0xFD,// Master uses this Opcode to accept the connection request from slave.
    REJECT = 0xFC, //Master uses this Opcode to reject the connection request sent from slave if the data verification failed.
    CLOSE = 0xFB, // Slave/Master uses this Opcode to Close connection request
    CLOSE_ACK = 0xFA, // Answer to CLOSE request. NULL DATA 0x00 Send a HRNP data packet. Data[N]
    DATA_ACK = 0x10, // Answer to a HRNP data packet. NULL
    VOICE = 0x01
} HRNP_opcode_types;

#pragma pack(1)
typedef struct __attribute__((__packed__))
{
    quint8 MsgHdr;
    quint8 opcode[2];
    quint8 num_of_bytes[2];
    quint8 *pld;
    quint8 Checksum;
    quint8 MsgEnd;
} hdpep_t;

typedef struct
{
    quint8 MsgHdr;
    quint8 opcode[2];
    quint8 num_of_bytes[2];
    quint8 Checksum;
    quint8 MsgEnd;
} __attribute__((__packed__)) hdpep_no_pld_t;

typedef struct
{
    quint8 header;// = 0x7E;
    quint8 version;// = 0x04;
    quint8 block;
    quint8 opcode;
    quint8 source;
    quint8 dest;
    quint8 packet_num[2];
    quint8 length[2];
    quint8 check_sum[2];
    hdpep_t pep;// 19+...
}__attribute__((__packed__)) hrnp_t;


typedef struct
{
    quint8 header;// = 0x7E;
    quint8 version;// = 0x04;
    quint8 block;
    quint8 opcode;
    quint8 source;
    quint8 dest;
    quint8 packet_num[2];
    quint8 length[2];
    quint8 check_sum[2];
    hdpep_no_pld_t pep_npld; // 19
} __attribute__((__packed__)) hrnp_no_pld_t;

typedef struct
{
    quint8 header;// = 0x7E;
    quint8 version;// = 0x04;
    quint8 block;
    quint8 opcode;
    quint8 source;
    quint8 dest;
    quint8 packet_num[2];
    quint8 length[2];
    quint8 check_sum[2];
} __attribute__((__packed__)) hrnp_no_pep_t; // 12

typedef struct __attribute__((__packed__))
{
    quint16 call_req;//0x0841
    quint8  type;
    quint32 radio_id;

} call_req_t;

typedef struct __attribute__((__packed__))
{
    quint16 call_rep;//0x0841;//
    quint8  result;
} call_rep_t;

//uint16_t call_req_remove = 0x0842;

typedef struct __attribute__((__packed__))
{
    quint16 call_rep;//0x8842;//
    quint8  result;
} call_remove_rep_t;


typedef struct{
        /* первый байт */
        quint8 csrc_len: 	4;
        quint8 extension:	1;
        quint8 padding:	1;
        quint8 version:	2;
        /* второй байт */
        quint8 payload_type:  	7;
        quint8 marker:		1;
        /* третий-четвертый байты */
        quint8 seq_no[2];
        /* пятый-восьмой байты */
        quint32 timestamp;
        /* девятый-двенадцатый байт */
        quint32 ssrc;
//        quint32 csrc;
        // now extention 8 byte
//        quint16 hdr_identifier;
//        quint16 length;
//        quint8 slot_last;
    }__attribute__ ((packed)) rtp_header;

struct strRMRpt{ // registr msg report=26

    struct strMsgHeader header;
    quint8 type;
    quint8 none;
    quint8 len[2] ;
    quint8 ser_num[10];
    quint8 zone[2];
    quint8 channel[2];
    quint8 reg_unreg_state;
    quint8 radio_id[4];
    quint8 signal_mode;
};

struct strRldRpl{ // reload replay msg =7
    struct strMsgHeader header;//  0-3  27.7.17
    quint8 type; // 4
    quint8 none; // 5
    quint8 reqid[4]; // 6-9
    quint8 result; // 10
};

struct strCCRpt{ // chan change report= 7
    struct strMsgHeader header;//  27.7.17
    quint8 type;
    quint8 none;
    quint8 newZone[2];
    quint8 newChan[2] ;
    quint8 signal_mode;
};

struct strCCRpl{ // chan change replay = 7
    struct strMsgHeader header;//  27.7.17
    quint8 type;
    quint8 none;
    quint8 reqid[4];
    quint8 result;
};

struct strCtrlRpl
{ // ctrl replay = 8 + 4
    struct strMsgHeader header;
    quint8 type;
    quint8 none;
    quint8 reqid[4];
    quint8 result;
    quint8 req_type;
    quint8 radio_id[4];
};

//struct strCtrlRequest
//{
//    quint8 typeMsg[2];// 041C
//    quint8 feature;
//    quint8 function;
//    quint8 rmtAdressType;//1
//    quint8 rmtAdressSize;//3
//    quint8 rmtAdress[3];
//    quint8 rmtPort[2];
//};


struct strCRpt{ // call report = 12
    struct strMsgHeader header;//  27.7.17
    quint8 type = 5;//=5
    quint8 none;//=0
    quint8 callType;
    quint8 calledId[4] ;
    quint8 receivedId[4];
    quint8 callState;
};

struct strCSRp{ // call stop reply = 7
    struct strMsgHeader header;//  27.7.17
    quint8 type;
    quint8 none;
    quint8 reqid[4];
    quint8 replyResult;
};

struct strCRp{ // call reply = 7
    struct strMsgHeader header;// 27.7.17
    quint8 type;
    quint8 none;
    quint8 reqid[4];
    quint8 requestResult;
};



//struct strCSRp{ // call stop reply = 7
//    struct strMsgHeader header;//  27.7.17
//    quint8 type;
//    quint8 none;
//    quint8 reqid[4];
//    quint8 replyResult;
//};

struct strMRRpt{ // modem type report = 4+5
    struct strMsgHeader header;//  27.7.17
    quint8 Msgtype;
    quint8 none;
    quint8 ver[2];
    quint8 ModemType;
};



//struct strERpl{ // = 7
//    struct strMsgHeader header;//  27.7.17
//    quint8 type;
//    quint8 none;
//    quint8 reqid[4];
//    quint8 replyResult;
//};

//struct strPUINPUT
//{
//    quint8 Opcode[2];
//    quint8 PUI_Source;
//    quint8 PUI_Type;
//    quint8 PUI_ID[2];
//    quint8 PUI_State;
//    quint8 PUI_State_Min;
//    quint8 PUI_State_Max;
//};

//// ************** Text Messages **************
//struct strTxtMsg_SrvAv
//{
//    struct strMsgHeader header;
//    quint8 TxtMsgType = 0;
//    quint8 ReceiverId[4];
//    quint8 SenderId[4];
//};

//struct strTxtMsg_SrvAvAck
//{
//    struct strMsgHeader header;
//    quint8 TxtMsgType = 1;
//    quint8 ReceiverId[4];
//    quint8 SenderId[4];
//};

//struct strTxtMsg_Ack
//{
//    struct strMsgHeader header;
//    quint8 TxtMsgType = 2;
//    quint8 ReceiverId[4];
//    quint8 SenderId[4];
//    bool result;
//    quint8 SeqNumber;
//};

struct strTxtMsg_SimpMsg
{
    struct strMsgHeader header;//0-3
    quint8 TxtMsgType = 3; //4
    quint8 ReceiverId[4]; // 5-8
    quint8 SenderId[4];  // 9-12
    quint8 ReceiverType;// – 13 Тип получателя,
    bool IsAckRequired;// – 14 Требуется ли подтверждение,
    quint8 SequenceNumber;// 15 – Последовательный номер,
    quint8 Text[280];// –  16-... Текстовое сообщение
};

////  *************** Location messages ***********************
//struct strLocMsgType_ImdReq
//{
//    struct strMsgHeader header;//0-3
//    quint8 LocMsgType = 0; //4
//    quint8 RadioId[4];  // 5-8
//    quint8 RequestId[4]; // 9-12
//    bool useCSBK;        // 13
//};

//struct strLocMsgType_ImdRep
//{
//    struct strMsgHeader header; // 0-3
//    quint8 LocMsgType = 2;//4
//    quint8 RadioId[4];  // 5-8
//    quint8 RequestId[4];// 9-12
//    quint8 Result[4];   // 13-16

//    quint8 Latitude[4];// – 17..20 Широта,
//    quint8 Longitude[4]; // 21-24 - Долгота,
//    quint8 Altitude[4]; // 25-28  - Высота,
//    quint8 Heading[4]; //  29-32 - Направление,
//    quint8 Radius[4]; //- 33-36  Радиус,
//    quint8 Speed[4]; //=  37-40  Скорость // 14...
//};

//struct strLocMsgType_TrgReq
//{
//    struct strMsgHeader header;//0-3
//    quint8 LocMsgType = 2; // 4
//    quint8 RadioId[4];    // 5-8
//    quint8 RequestId[4];  // 9-12
//    quint8 Interval[2];  // 13-14
//    bool useCSBK;         //15
//};

//struct strLocMsgType_TrgAnsw
//{
//    struct strMsgHeader header;// 0-3
//    quint8 LocMsgType = 4;  // 4
//    quint8 RadioId[4];  // 5-8
//    quint8 RequestId[4]; // 9-12
//    quint8 Result[4];   // 13-16
//};

struct strLocMsgType_TrgRep
{
    struct strMsgHeader header;// 0-3
    quint8 LocMsgType = 5;  // 4
    quint8 RadioId[4];  // 5-8
    quint8 RequestId[4]; // 9-12
    quint8 Result[4];   // 13-16

    quint8 Latitude[4];// – 17..20 Широта,
    quint8 Longitude[4]; // 18-21 - Долгота,
    quint8 Altitude[4]; // 22-25  - Высота,
    quint8 Heading[4]; //  26-29 - Направление,
    quint8 Radius[4]; //- 30-33  Радиус,
    quint8 Speed[4]; //=  34-47  Скорость // 14...  // 14...
}__attribute__((__packed__));

//struct strLocMsgType_TrgStopReq
//{
//    struct strMsgHeader header; // 0-3
//    quint8 LocMsgType = 5; // 4
//    quint8 RadioId[4];  // 5-8
//    quint8 RequestId[4]; // 9-12
//};

//struct strLocMsgType_TrgStopAnsw
//{
//    struct strMsgHeader header; // 0-3
//    quint8 LocMsgType = 7; // 4
//    quint8 RadioId[4];  // 5-8
//    quint8 RequestId[4]; // 9-12
//    quint8 Result[4];// 13-16  see enum TrgLocationResult;
//};

struct strMsgType_Ping
{
    struct strMsgHeader header; // 0-3
    quint8 Ping = 1; // 1
};



#endif // MESSAGES_H
