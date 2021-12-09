#include "tcp.h"
#include "messages.h"
#include "enums.h"
#include <cstring>

extern void checksum(unsigned char*);
extern void crc(quint8*, int);

//extern QString          ars_addr;
//extern quint8           ars_answer[16];
//extern quint8           ars_check[11];
extern QByteArray       ba;
//extern unsigned char    buf_tx[];
//extern hrnp_t           call_req;
//extern QString          dest_rrs_ip;
extern strMsgHeader     header;
//extern QString          hadrr, host;
extern quint16          hyt_udp_port;
//extern quint16          hyterus_port;
//extern quint8           last_abonent[4];

//extern volatile quint16 msg_cnt;
extern quint8           mu_law;
//extern quint32          ping_counter;
//extern hrnp_t           ptt_release_req;
//extern hrnp_t           ptt_press_req;
//extern hrnp_t           radio_en_dis;
//extern quint32          Radio_Reg_State;
//extern quint8           req_id[4];
//extern hrnp_t           restart;

//extern hrnp_t           remote_monitor;
extern rtp_header       rtp_hdr;
//extern hrnp_t           rtp_pld_req;
//extern strMRRpt         sMRRpt;
//extern strCtrlRpl       sCtrlReply;
extern quint32          Radio_Reg_State;
//extern strRldRpl        sReloadReplay;
//extern strCCRpt         sChanChangeReport;
//extern strCCRpl         sChanChangeReply;
extern strCRpt          sCallReport;
extern strCSRp          sCallStopReply;
extern strCSRp          sStopCallReply;
extern strCRp           sCallReply;
//extern strCallControl   sCallControl;
extern strRMRpt         sRegMsgReport;
//extern strMsgType_Ping  sMsgType_Ping;
//extern QByteArray       tcp_tx;
//extern hrnp_t           zone_ch;
//extern hrnp_t           radio_check;
extern quint8 sendRegMsgReport;
quint32 check_online = 0;
quint32 disable_online = 0;
quint32 enable_online = 0;
quint32 monitor_online = 0;
quint32 start_sound;


/* Hytera Simple Transport Reliability Protocol
 *
 *Header  Version  Type(5-bit Option)     SN      Option    Payload
2 Bytes  1 Byte    1 Byte               2 Bytes   0 Bytes  (2,0x41,0,2,0,0x2B,0,0xC4,3)
 *
 */
quint8 pressPtt[] =   {0x32, 0x42, 0, 0, 0, 0, 2, 0x41, 0, 2, 0, 3, 1, 0xEB, 3};
quint8 releasePtt[] = {0x32, 0x42, 0, 0, 0, 0, 2, 0x41, 0, 2, 0, 3, 0, 0xEC, 3};


//quint8 pressPtt[] =   {0x32, 0x42, 0, 0, 0,0, 2, 0x41, 0, 2, 0, 0x2B, 1, 0xC3, 3};
//quint8 releasePtt[] = {0x32, 0x42, 0, 0, 0,0, 2, 0x41, 0, 2, 0, 0x2B, 0, 0xC4, 3};
static quint16 packet_num;


void TCP::rcv_tcpRCP()
{
    if(ba.at(4) == eCallRequestMsg)//тип(4)+ резерв(5)+id_req(6..9)+calltype(10)+rcvId(11..14)
    {
        if(sRegMsgReport.signal_mode == DIGITAL_MODE)
        {
            if(Radio_Reg_State == READY)
            {
                memcpy(sCallReply.reqid, ba.data() + 2 + 4, 4);
                packet_num++;
                pressPtt[4] = packet_num >> 8;
                pressPtt[5] = packet_num & 0xFF;
                udp_30015.writeDatagram((char*)pressPtt, 15, QHostAddress("192.168.10.1"), 30015);
                //udp_30015.flush();
#ifdef DBG
                qDebug() << "Call_1 request state_1: " << Radio_Reg_State;
#endif
            }
            else
            {
                memcpy(sCallReply.reqid, ba.data() + 2 + sizeof(header), 4);
                sCallReply.requestResult = eFailure;// error
                udp_srv->write(reinterpret_cast<char*>(&sCallReply), sizeof(sCallReply));
#ifdef DBG
                qDebug() << "Call_1 request failure, state_1: " << Radio_Reg_State;
#endif
            }
        }
    }
    else if(ba.at(4) == eStopCallRequestMsg)
    {
        if(sRegMsgReport.signal_mode )
        {
            packet_num++;
            releasePtt[4] = packet_num >> 8;
            releasePtt[5] = packet_num & 0xFF;
            udp_30015.writeDatagram((char*)releasePtt, 15, QHostAddress("192.168.10.1"), 30015);
            memcpy(sCallStopReply.reqid, ba.data() + 2 + sizeof(header), 4);
#ifdef DBG
            qDebug() << QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss ") << "Stop Call_1 request state_1: " << Radio_Reg_State;
#endif
        }
        else
        {
            memcpy(sCallStopReply.reqid, ba.data() + 2 + sizeof(header), 4);
            sCallStopReply.replyResult = eFailure;// error
            udp_srv->write(reinterpret_cast<char*>(&sCallStopReply), sizeof(sCallStopReply));
#ifdef DBG
            qDebug() << QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss ") << "Stop Call_1 request_1 failure state_1: " << Radio_Reg_State;
#endif
        }
    }
    else if ((ba.at(4) == eChanChangeRequestMsg)  && (Radio_Reg_State == READY))
    {

    }
    else if((ba.at(4) == eSuControlRequest) && (Radio_Reg_State == READY))////тип(4)+ резерв(5)+id_req(6..9)+ReqType(10)+rcvId(11..14)
    {

    }
    else if(ba.at(4) == eUdpPortNotification)
    {
        hyt_udp_port = static_cast<quint16>(static_cast<quint32>( ba.at(8 + sizeof(header))) * 256 + static_cast<quint32>(ba.at(9 + sizeof(header))));
        if(udp_srv)
            delete udp_srv;

        udp_srv = new QUdpSocket(this);
        udp_srv->bind(hyt_udp_port );
#ifdef RTP
        mu_law = (ba.at(13 + sizeof(header)));
        rtp_hdr.payload_type = mu_law;
#endif
        connect(udp_srv, SIGNAL(readyRead()),this, SLOT(udp_srv_slot()));

#ifdef DBG
        qDebug() << QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss ") << "port number: " << hyt_udp_port;
#endif
        if(sendRegMsgReport)
        {
            tcp_srv.write(reinterpret_cast<char*>(&sRegMsgReport), sizeof(sRegMsgReport));
            tcp_srv.flush();
        }
    }
    else if(ba.at(4) == eRldRequestMsg)
    {

    }

}
