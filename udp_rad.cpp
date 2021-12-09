#include "tcp.h"
#include "messages.h"
#include "enums.h"

//unsigned char buf_tx1[128] = { 0x32, 0x42, 0, 0x25,0x00, 0x00, 0, 0 , 0, 12, 0x60, 0xE1};
//unsigned char heartbeat[6] = {0x32,0x42,0,2,0,0};
//unsigned char txt_msg_ack[26] = {0x32, 0x42, 0, 0, 0,0, 9, 0x80,0xA2, 0, 13, 0,0,0,0 ,0,0,0,0 ,192,168,0,26, 0, 0, 0x03};
quint8 ack_connect15[15] = {0x32 ,0x42 ,0 ,0x25 ,0 ,0 ,0x83 ,4 ,0 ,0 ,3 ,0xed ,4 ,1 ,0};
quint8 ack_connect16[17] = {0x32 ,0x42 ,0 ,0x25 ,0 ,0 ,0x83 ,4 ,0 ,0 ,3 ,0xed ,0x84 ,1 ,0, 1, 0};
quint8 ack_heatbeat[6] = {0x32 ,0x42 ,0 ,0x02 ,0 ,0 };

#define htonl(A)    (((static_cast<unsigned long>(A) & 0xff000000) >> 24) | \
    ((static_cast<unsigned long>(A) & 0x00ff0000) >> 8)  | \
    ((static_cast<unsigned long>(A) & 0x0000ff00) << 8)  | \
    ((static_cast<unsigned long>(A) & 0x000000ff) << 24))

extern strRMRpt 	  sRegMsgReport;
extern struct strCRpt sCallReport1;
extern struct strCRp  sCallReply;
extern struct strCSRp sCallStopReply;
extern quint8 req_id[];
extern void checksum(unsigned char*);
extern QString host;
extern quint8 Radio_Reg_State;
extern quint32 ts_current;
extern rtp_header rtp_hdr;
extern quint16 hyt_udp_port;


QByteArray ba_3006;
quint8 ack_callctrl[15];


//quint8 groupNum = 20;
//quint8 chanNum = 2;
//char * serialNum = "2222222222";

quint8 groupNum = 10;
quint8 chanNum = 1;
//char * serialNum1 = "1111111111";
quint8 serialNum[] =  {0x32, 0x42, 0, 0, 0, 0, 2, 0x01, 2, 1, 0, 2,    0x2A, 3};
QString kadrr;// = "192.168.0.180";

static quint16 temp_16[480];
static quint8 temp_8[960 + 40];
static quint16 seq_num;

quint8 sendRegMsgReport = 0;// было ли послано рег сообщение
void fillRegMsg();
bool get_serial = false;
QByteArray ba30015;

//3242002000 0283040000 03ed040100 0245b81000000001000100000000000000000000002303
// ******** Analog Call Control ************
void TCP::receive30015()
{
    qint32 len;
    ba30015.resize(static_cast<int>(udp_30015.pendingDatagramSize()));
    len = ba30015.size();
    udp_30015.readDatagram(ba30015.data(), len);

#ifdef DBG_BA
    qDebug() << "Repeiter 30015 " << ba30015.toHex();
#endif
    if(ba30015.at(3) == 0x20)// call control
    {
        memcpy(ack_callctrl, ba30015.data(), 15);
        ack_callctrl[3] += 1;
        udp_30015.writeDatagram((char*)ack_callctrl, 15, QHostAddress("192.168.10.1"), 30015);

        if(ba30015.at(15) == 2 )
        {// RCP
            if((ba30015.at(16) == 0x45) && (ba30015.at(17) == 0xB8))//Rep BroadCast Receive(Transmit) Status
            {
                switch (ba30015.at(22)) {
                case 0://call init
                    sCallReport1.callType = 1;// group call
                    memset(sCallReport1.receivedId, 0, 4);
                    memset(sCallReport1.calledId, 0, 4);
                    sCallReport1.calledId[3] = groupNum;
                    sCallReport1.receivedId[3] = groupNum;

                    sCallReport1.callState = eCallInit;
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport1), sizeof(sCallReport1));
                        qDebug() << "send rx";
                    }
                    break;
                case 1:// call hng
                    sCallReport1.callType = 1;// group call
                    memset(sCallReport1.receivedId, 0, 4);
                    memset(sCallReport1.calledId, 0, 4);
                    sCallReport1.calledId[3] = groupNum;
                    sCallReport1.receivedId[3] = groupNum;
                    sCallReport1.callState = eCallHandTime;
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport1), sizeof(sCallReport1));
                    }
                    break;

                case 5:// call end
                    tx_tim.start(50);

                    break;
                default:
                    break;
                }
            }
            else if((ba30015.at(16) == 0x43) && (ba30015.at(17) == 0xB8))//Rep BroadCast Transmit Status
            {
                // BroadCast Tx Status
                // Process(2) + Source(2) + CallType(2) + RadioId(4)
                if(ba30015.at(20) == 3) // StopCall HangTime
                {
                    qDebug() << "Hang Call";
                    //Radio_Reg_State = WAIT_STOP_CALL_REPLY;
                    sCallReport1.callType = 1;// group call
                    memset(sCallReport1.receivedId, 0, 4);
                    memset(sCallReport1.calledId, 0, 4);
                    sCallReport1.calledId[3] = groupNum;
                    sCallReport1.receivedId[3] = groupNum;
                    sCallReport1.callState = eCallHandTime;
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport1), sizeof(sCallReport1));
                        tcp_srv.flush();
                    }
                }
            }
            else if((ba30015.at(16) == 0x41) && (ba30015.at(17) == 0x80))//Reply Press Ptt
            {
                if(ba30015.at(20) == 0 && ba30015.at(28) == 1) // ptt press success!
                {
                    sCallReply.requestResult = eSucces;
                    tcp_srv.write(reinterpret_cast<char*>(&sCallReply), sizeof(sCallReply));
                    qDebug() << "ptt press success";
                }
                else if(ba30015.at(20) == 0 && ba30015.at(28) == 0) // ptt release success!
                {
                    sCallStopReply.replyResult = eSucces;
                    tcp_srv.write(reinterpret_cast<char*>(&sCallStopReply), sizeof(sCallStopReply));
                    qDebug() << "ptt release success";
                }
                else if(ba30015.at(20) == 1 && ba30015.at(28) == 1)// ptt press fault!
                {
                    sCallReply.requestResult = eFailure;
                    tcp_srv.write(reinterpret_cast<char*>(&sCallReply), sizeof(sCallReply));
                    qDebug() << "press ptt fault";
                }
                else if(ba30015.at(20) == 1 && ba30015.at(28) == 0)// ptt release fault!
                {
                    sCallReply.requestResult = eFailure;
                    tcp_srv.write(reinterpret_cast<char*>(&sCallReply), sizeof(sCallReply));
                    qDebug() << "press ptt fault";
                }
            }
            else if((ba30015.at(16) == 0x01) && (ba30015.at(17) == 0x82))// serial num report
            {

                if((sendRegMsgReport == 1) && (tcp_srv.state() ==  QAbstractSocket::ConnectedState))
                {
                    Radio_Reg_State = READY;
                    fillRegMsg();
                    for(int i = 0; i < 10; i++)
                        qDebug() << (sRegMsgReport.ser_num[i] = ba30015.at(28 + (i << 1)));
                    //qDebug() << sRegMsgReport.ser_num[8] << sRegMsgReport.ser_num[9];
                    get_serial = false;
                    tcp_srv.write(reinterpret_cast<char*>(&sRegMsgReport), sizeof(sRegMsgReport));
                    tcp_srv.flush();
#ifdef DBG
                    qDebug() << QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss ") << "Registration1 to kupol";
#endif
                }
            }
        }
    }
    else if(ba30015.at(3) == 0x24)// connect
    {
        udp_30015.writeDatagram((char*)ack_connect15, 15, QHostAddress("192.168.10.1"), 30015);
        udp_30015.flush();

        if((tcp_srv.state() ==  QAbstractSocket::ConnectedState) && !sendRegMsgReport)
        {
            sendRegMsgReport = 1;
            radio_check_tim.start();// send serial num request
        }
    }
    else if(ba30015.at(3) == 0x02)// heatbeat
    {
        if(sendRegMsgReport) // for reset
        {
            udp_30015.writeDatagram((char*)ack_heatbeat, 6, QHostAddress("192.168.10.1"), 30015);
            udp_30015.flush();
        }
    }
}

QByteArray ba30016;
// *************** Analog Voice Service ***********************
// 32 42 0024 0000 83 04 000003ed 84 01 00 01 00
//32 42 00 20 0001 83 04000003ed 040100 02 45b8 10000000 00 0001 0000000000000000000000 2403
//32 42 00 20 0008 83 04000003ed 040100 02 45b8 10000000 01 0001 0000000000000000000000 2303
//32 42 00 20 0009 83 04000003ed 040100 02 45b8 10000000 05 0001 0000000000000000000000 1f03
void TCP::receive30016()
{
    qint32 len;
    quint8 sound[480];
    ba30016.resize(static_cast<int>(udp_30016.pendingDatagramSize()));
    len = ba30016.size();
    udp_30016.readDatagram(ba30016.data(), len);
#ifdef DBG_BA
    qDebug() << "Repiter 30016 " << len;//ba30016.toHex();
#endif
    if(ba30016.at(3) == 0x24){// connect
        udp_30016.writeDatagram((char*)ack_connect16, 17, QHostAddress("192.168.10.1"), 30016);
    }
    else if(ba30016.at(3) == 0x02){// heatbeat
        udp_30016.writeDatagram((char*)ack_heatbeat, 6, QHostAddress("192.168.10.1"), 30016);
    }
    else if(ba30016.at(0) == 0x90){// sound
        memcpy(sound, ba30016.data() + 28, 480);
        ts_current += TS_DELTA;
        rtp_hdr.timestamp = htonl(ts_current);
        seq_num++;
        rtp_hdr.seq_no[0] = seq_num >> 8;
        rtp_hdr.seq_no[1] = seq_num & 0xff;
        memcpy(reinterpret_cast<quint8*>(temp_8), reinterpret_cast<quint8*>(&rtp_hdr), RTP_HDR_LEN );
        for(int i = 0; i < 480; i++)
            temp_8[i + RTP_HDR_LEN] = sound[i];
        udp_srv->writeDatagram(reinterpret_cast<char*>(temp_8), 480 + RTP_HDR_LEN, QHostAddress(kadrr), hyt_udp_port);

        //        ts1_current += TS_DELTA;
        //        rtp1_hdr.timestamp = htonl(ts1_current);
        //        seq_num1++;
        //        rtp1_hdr.seq_no[0] = seq_num1 >> 8;
        //        rtp1_hdr.seq_no[1] = seq_num1 & 0xff;
        //        memcpy(reinterpret_cast<quint8*>(temp_8) ,reinterpret_cast<quint8*>(&rtp1_hdr), RTP_HDR_LEN );
        //        for(i = 0; i < 160; i++)
        //            temp_8[i + RTP_HDR_LEN] = sound[i + 160];
        //        udpKupol1.writeDatagram(reinterpret_cast<char*>(temp_8), 160 + RTP_HDR_LEN, QHostAddress(kadrr), portKupol1);

        //        ts1_current += TS_DELTA;
        //        rtp1_hdr.timestamp = htonl(ts1_current);
        //        seq_num1++;
        //        rtp1_hdr.seq_no[0] = seq_num1 >> 8;
        //        rtp1_hdr.seq_no[1] = seq_num1 & 0xff;
        //        memcpy(reinterpret_cast<quint8*>(temp_8) ,reinterpret_cast<quint8*>(&rtp1_hdr), RTP_HDR_LEN );
        //        for(i = 0; i < 160; i++)
        //            temp_8[i + RTP_HDR_LEN] = sound[i + 320];
        //        udpKupol1.writeDatagram(reinterpret_cast<char*>(temp_8), 160 + RTP_HDR_LEN, QHostAddress(kadrr), portKupol1);
    }
}

void fillRegMsg()
{
    sRegMsgReport.zone[0] = static_cast<unsigned char>(0);
    sRegMsgReport.zone[1] = static_cast<unsigned char>(1);
    sRegMsgReport.channel[0] = static_cast<unsigned char>(0);
    sRegMsgReport.channel[1] = static_cast<unsigned char>(chanNum);
    sRegMsgReport.len[0] = 0;
    sRegMsgReport.len[1] = 10;
    //for(int i = 0; i < 10; i++)
    //    sRegMsgReport.ser_num[i] = *(serialNum1 + i);
    sRegMsgReport.radio_id[0] = sRegMsgReport.radio_id[1] = sRegMsgReport.radio_id[2] = 0;
    sRegMsgReport.radio_id[3] = 100;
    sRegMsgReport.signal_mode = DIGITAL_MODE;
    sRegMsgReport.reg_unreg_state = REGISTRATE;
    sRegMsgReport.type = eRegReportMsg;
    sRegMsgReport.header.msgType = 1;
    sRegMsgReport.header.payloadLength[0] = 0;
    sRegMsgReport.header.payloadLength[1] = 24;
}
