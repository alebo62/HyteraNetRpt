#include "tcp.h"
#include "messages.h"
#include "enums.h"


extern QByteArray   ba;
extern QString      dest_rrs_ip;
extern hrnp_t       txt_msg_tx;
extern strRMRpt     sRegMsgReport;
extern strTxtMsg_SimpMsg sTxtMsg_SimpMsg;

/*  MsgHdr(0x09)
    Opcode    0x80 0xB1(SendGroupMsg)    0x80 0xA1(SendPrivateMsg) (1,2)
    #of bytes  (3,4)
    Payload   RequestID[4]+DestID[4]+SrcID[4]+TMData[n] (5..8  9..12 13..16 17...)
    Checksum        4 + #of bytes
    MsgEnd (0x03)   5 + #of bytes

*/

extern void crc(unsigned char*, int);
QByteArray test;
QString     tms_addr;
QByteArray  ba_3004;
unsigned char txt_msg_arr[255];
quint32 msg_len;
void TCP::rcv_udpTMS()
{
    ba_3004.resize(static_cast<int>(udpTMS_3004.pendingDatagramSize()));
    udpTMS_3004.readDatagram(ba_3004.data(), ba_3004.size());
#ifdef DBG_BA
    qDebug() << "txt " << ba_3004.toHex();
#endif

    msg_len = ba_3004.at(4) - 12;

    sTxtMsg_SimpMsg.SequenceNumber = ba_3004.at(8);

    sTxtMsg_SimpMsg.ReceiverId[0] = 0;
    sTxtMsg_SimpMsg.ReceiverId[1] = ba_3004.at(10);
    sTxtMsg_SimpMsg.ReceiverId[2] = ba_3004.at(11);
    sTxtMsg_SimpMsg.ReceiverId[3] = ba_3004.at(12);

    sTxtMsg_SimpMsg.SenderId[0] = 0;// номер сети не нужен
    sTxtMsg_SimpMsg.SenderId[1] = ba_3004.at(14);
    sTxtMsg_SimpMsg.SenderId[2] = ba_3004.at(15);
    sTxtMsg_SimpMsg.SenderId[3] = ba_3004.at(16);

    sTxtMsg_SimpMsg.ReceiverType = 3;// simple txt msg
    sTxtMsg_SimpMsg.IsAckRequired = false;
    if(ba_3004.at(2) == 0xA1)
    {
        sTxtMsg_SimpMsg.ReceiverType = eDevice;
        //memcpy(sTxtMsg_SimpMsg.ReceiverId, sRegMsgReport.radio_id , 4);
    }
    else if(ba_3004.at(2) == 0xB1)
    {
        sTxtMsg_SimpMsg.ReceiverType = eTalkGroup;
        //memcpy(sTxtMsg_SimpMsg.ReceiverId, ba_3004.data() + 10 , 3);
    }

    sTxtMsg_SimpMsg.header.payloadLength[1] = 9 + 3 + msg_len;           // ?
    memcpy(sTxtMsg_SimpMsg.Text, ba_3004.data() + 17, msg_len);

    //    for(int i = 0; i < msg_len + 16; i++)
    //        test.append( *((char*)&sTxtMsg_SimpMsg + i));
    //    qDebug() << test.toHex();

    tcp_srv.write(reinterpret_cast<char*>(&sTxtMsg_SimpMsg), 4+9+3+ msg_len);// ?
    tcp_srv.flush();

}

/*
 * IronAgentMsgType  1 байт (0x02)
 * PayloadLength     2 байта
 * Reserved          1 byte

 *  TxtMsgType      1 байт (4)
 *  int ReceiverId  4 байта	(5..8)
 *  int SenderId    4 байта	(9..12)
 *
 Payload:
    byte ReceiverType – Тип получателя,(13)
    bool IsAckRequired – Требуется ли подтверждение,(14)
    byte SequenceNumber – Последовательный номер,(15)
    byte[] Text – Текстовое сообщение (16...)

 */

void TCP::rcv_tcpTMS()
{
#ifdef DBG_BA
    qDebug() << "txt msg " << ba.toHex();
#endif
    switch (ba.at(4)) {
    case TmsServAvail:
        // "02 00-09 00 00 00-00-00-33 00000001"
        break;
    case TmsAck:

        break;
    case SimpleTextMessage:  // 02001600 03 00-00-00-33 00 00 00 01 01 01 01(15)  "6800-6500-6c00-6c00-6f00" hello
        msg_len = ba.at(2) - 12;
        if(ba.at(13) == eDevice)
        {
            txt_msg_tx.pep.opcode[1] = 0xA1;
            if(ba.at(14) == false)//ack requred
            {
                txt_msg_tx.pep.opcode[0] &= 0x03;
            }
            else
            {
                txt_msg_tx.pep.opcode[0] |= 0x80;
            }
        }
        else if(ba.at(13) == eTalkGroup)
        {
            txt_msg_tx.pep.opcode[1] = 0xB1;
        }
        memcpy(txt_msg_tx.pep.pld + 12, ba.data() + 16, msg_len);

        txt_msg_tx.pep.pld[0] = 0;  // req id
        txt_msg_tx.pep.pld[1] = 0;
        txt_msg_tx.pep.pld[2] = 0;
        txt_msg_tx.pep.pld[3] = ba.at(15)+ 1;


        txt_msg_tx.pep.pld[5] = ba.at(6);// dest id
        txt_msg_tx.pep.pld[6] = ba.at(7);
        txt_msg_tx.pep.pld[7] = ba.at(8);

        tms_addr =  dest_rrs_ip + QString::number(sRegMsgReport.radio_id[1]) + ".";
        tms_addr += QString::number(sRegMsgReport.radio_id[2]) + ".";
        tms_addr += QString::number(sRegMsgReport.radio_id[3]);

        txt_msg_tx.pep.pld[9] =  sRegMsgReport.radio_id[1];
        txt_msg_tx.pep.pld[10] = sRegMsgReport.radio_id[2];
        txt_msg_tx.pep.pld[11] = sRegMsgReport.radio_id[3];

        txt_msg_tx.pep.num_of_bytes[1] = msg_len + 12;

        memcpy(txt_msg_arr, reinterpret_cast<char*>(&txt_msg_tx) + 12, 5);
        memcpy(txt_msg_arr + 5, reinterpret_cast<char*>(txt_msg_tx.pep.pld) , msg_len+12);
        txt_msg_arr[msg_len + 12 + 5 ] = 0;
        txt_msg_arr[msg_len + 13 + 5] = 0x03;
        crc(txt_msg_arr , 5 + msg_len + 2 + 12);

        //        for(int i = 0; i < (msg_len+5+2+12); i++)
        //            test.append(txt_msg_arr[i]);
        //        qDebug() << test.toHex();

        udpTMS_3004.writeDatagram((char*)txt_msg_arr, msg_len + 5 + 2 +12, QHostAddress(tms_addr), TMS);
        udpTMS_3004.flush();
        break;
    default:
        break;
    }
    //  7e0400002010000e 0029 696d 09 80a1 0016 00000001 0a000033 0a000001 6800-6500-6c00-6c00-6f00 9e 00
    //  7e0400002010000e 0029 0000 09 80a1 0016 68b10600 0003 007e040000201000020018000002c4000500

}
