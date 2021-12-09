#ifndef VARIABLES_H
#define VARIABLES_H

#include "tcp.h"


QString MY_RI_ADDR = "192.168.10.2";
QString hadrr ;// =      "192.168.0.122";
QString host = "10.";//0.0.222";
quint16 hyterus_port = 40042;
quint16 hyt_udp_port;
quint8 i = 111;
QString dest_rrs_ip = host.mid(0, 3);// 10. + "0." + "0." + QString::number(i);

//char* sn = "511TPH2812";
char* sn = "511TPH2814";

quint8 aud_tx[532];

quint8 sound_msg[52 + 480] = {0x7e,4,0,1,0x20,0x10,0,0,0x02,0x14,0x00,0x00,
                       0x90,0x00,
                       0,0x44, // 14-15  seq number
                       0,0,0,0x44,//  16-19 timestamp
                       0,0,0,0, // 20-23 Synchronization source (SSRC) identifier
                                // Contributing source (CSRC) identifiers (variable 0-15 items 32bits each)

                       0xe0,0,   // 24-25 16 bit CODE_TYPE Standard DMR Header Extension
                       0,6, // 26-27 Num 32bit words
                       0,// 28  Super frame number
                       0,0,10, // 29-31 DST_DMR_ID
                       0, // 32reserved
                       0,0,0x33, // 33-35 SRC_DMR_ID
                       0,0,0,0, // 36-39 BaseStation_ID
                       0, // 40 Proto Major + ProtoMinor
                       0x10, // 41 ColorCode + FadingControl
                       0x01, // 42 DatoType(0) + CallType (0100)DMR+GrCall+NotCallAlert+Reserved
                       0,// 43  2-2-4 Source+StartReason+EndReason
                       0, // 44  Service Options
                       0, // 45 Signal Quality
                       0, // 46 4-4 Reserved + Encryption
                       0, // 47 Key Id
                       0,0,0,0 // 48-50 Init Vector
                             };
/*
 *
 *QByteArray gps_ba {0x08,0xa0,02,0x00,0x32,0x00,0x00,0x04,0xb0,0x0a,0x00,0x00,0x66,0x00,0x00,0x41,0x30,0x39,0x31,
 * 0x32,0x35,0x38,0x31,0x35,0x31,0x30,0x32,0x31,0x4e,0x35,0x39,0x35,0x35,0x2e,0x38,0x32
 * ,0x39,0x34,0x45,0x30,0x33,0x30,0x32,0x33,0x2e,0x32,0x34,0x30,0x31,0x30,0x2e,0x30,0x33,0x35,0x38,0x0b,0x03};
 *
 * */

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

#endif // VARIABLES_H
