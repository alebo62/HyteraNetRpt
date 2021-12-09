#include "tcp.h"
#include "messages.h"
#include "enums.h"
#include <QProcess>

#define TS_DELTA 160
#define RTP_HDR_LEN 12
#define htonl(A)    (((static_cast<unsigned long>(A) & 0xff000000) >> 24) | \
    ((static_cast<unsigned long>(A) & 0x00ff0000) >> 8)  | \
    ((static_cast<unsigned long>(A) & 0x0000ff00) << 8)  | \
    ((static_cast<unsigned long>(A) & 0x000000ff) << 24))

extern quint16 hyt_udp_port;
//extern QByteArray ba_3005;
extern QString host, hadrr;
extern quint16 hyterus_port;
//extern unsigned char conn[];
extern strRMRpt  sRegMsgReport;
extern strCtrlRpl sCtrlReply;
extern quint32   ping_counter;
extern quint32 Radio_Reg_State;
extern quint16 msg_cnt;
extern quint32 state;
//extern quint32 check_online;
//extern quint32 enable_online;
//extern quint32 disable_online;
//extern quint32 monitor_online;
extern struct strCRpt sCallReport1;
extern quint8 groupNum;
extern quint32 start_sound;
extern unsigned char temp_8[532];
extern quint16 seq_num;
extern quint32 ts_current;
extern rtp_header rtp_hdr;
extern hrnp_t     ptt_release_req;
extern unsigned char    buf_tx[];
extern void checksum(unsigned char* buf_tx);

static QProcess proc;
static quint32 sound_tim_counter;

void TCP::rad_conn_tim_slot()
{
    static quint32 conn_counter = 0;
#ifdef DBG
    qDebug() << "connect";
#endif
    //udpRCP_3005.writeDatagram((char*)conn, 12, QHostAddress(host), RCP);
    //udpRCP_3005.flush();
//    if(++conn_counter == 10)
//        proc.start("sudo reboot");
}

void TCP::tcp_conn_tim_slot()
{
    if(tcp_srv.state() ==  QAbstractSocket::UnconnectedState )
        tcp_srv.connectToHost(hadrr, hyterus_port);
    else if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
    {
        //if(hyt_udp_port )
        {
            tcp_conn_tim.stop();
//            tcp_srv.write((char*)&sRegMsgReport, sizeof(sRegMsgReport));
//            tcp_srv.flush();
//#ifdef DBG
//            qDebug() << "RegMsgReport send";
//#endif
        }
    }
}

void TCP::ping_tim_slot()
{
    ping_counter++;
#ifdef DBG
    qDebug() << ping_counter;
#endif
    if(ping_counter == 3)
    {
        ping_tim.stop();
        tcp_srv.close();
    }
}

void TCP::reload_tim_slot()
{
    //reload_tim.stop();
    //    state = 0;
    //    udpRCP_3005.writeDatagram((char*)conn, 12, QHostAddress(host), RCP);
    //    Radio_Reg_State = INIT_STATE;
    //    msg_cnt = 0;
#ifdef DBG
    qDebug() << "reload tim slot";
#endif
    proc.start("sudo reboot");
}
extern quint8 serialNum[];

void TCP::radio_check_tim_slot()
{
//    serialNum[4] = sn1 >> 8;
//    serialNum[5] = sn1 & 0xFF;
//            sn1++;
    udp_30015.writeDatagram((char*)serialNum, 14, QHostAddress("192.168.10.1"), 30015);
    udp_30015.flush();
    radio_check_tim.stop();
//    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
//    {
//        check_online = 0;
//        sCtrlReply.result = 1; // fail
//        sCtrlReply.req_type = 0;// radio check
//        if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
//        {
//            tcp_srv.write((char*)&sCtrlReply, sizeof(sCtrlReply));
//            tcp_srv.flush();
//        }
#ifdef DBG
        qDebug() << "radio check timer";
#endif
//    }
}

void TCP::monitor_tim_slot()
{
//    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
//    {
//        monitor_online = 0;
//        sCtrlReply.result = 1; // fail
//        //sCtrlReply.req_type = 3;// monitor radio
//        monitor_tim.stop();
//        if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
//        {
//            tcp_srv.write((char*)&sCtrlReply, sizeof(sCtrlReply));
//            tcp_srv.flush();
//        }
//#ifdef DBG
//        qDebug() << "radio monitor timer";
//#endif
//    }

}

void TCP::rx_tim_slot()
{
    Radio_Reg_State = READY;
    //rx_tim.stop();
#ifdef DBG
    qDebug() << "rx timer slot";
#endif
}

void TCP::tx_tim_slot()
{
    tx_tim.stop();
    sCallReport1.callType = 1;// group call
    memset(sCallReport1.receivedId, 0, 4);
    memset(sCallReport1.calledId, 0, 4);
    sCallReport1.calledId[3] = groupNum;
    sCallReport1.receivedId[3] = groupNum;
    sCallReport1.callState = eCallEnded;
    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
    {
        tcp_srv.write(reinterpret_cast<char*>(&sCallReport1), sizeof(sCallReport1));
        //tcp_srv.flush();
        qDebug() << "call end";
    }
    Radio_Reg_State = READY;
//    start_sound = 0;
//    tx_tim.stop();

//    if((Radio_Reg_State == WAIT_STOP_CALL_REPLY) || (Radio_Reg_State == WAIT_STOP_CALL_HANGIN))
//    {
//        Radio_Reg_State = WAIT_STOP_CALL_REPLY;
//        //memcpy(sCallStopReply.reqid, ba.data() + 2 + sizeof(header), 4);
//        msg_cnt++;
//        ptt_release_req.packet_num[0] = msg_cnt >> 8;// page 159
//        ptt_release_req.packet_num[1] = msg_cnt & 0xFF;
//        memcpy(buf_tx, reinterpret_cast<char*>(&ptt_release_req), 17 );
//        memcpy(buf_tx + 17, reinterpret_cast<char*>(ptt_release_req.pep.pld), ptt_release_req.pep.num_of_bytes[0]);
//        memcpy(buf_tx + 17 + ptt_release_req.pep.num_of_bytes[0], reinterpret_cast<char*>(&ptt_release_req + 21), 2 );
//        checksum(buf_tx);
//        udpRCP_3005.writeDatagram((char*)buf_tx, ptt_release_req.length[1], QHostAddress(host), RCP);
//        udpRCP_3005.flush();
//#ifdef DBG
//        qDebug() << "tx_timer slot ptt release";
//#endif
//    }
//    else
//    {
//        Radio_Reg_State = READY;
//#ifdef DBG
//        qDebug() << "tx_timer slot --> READY";
//#endif
//    }
}

void TCP::sound_tim_slot()
{
    sound_tim_counter++;
    ts_current += TS_DELTA;
    rtp_hdr.timestamp = htonl(ts_current);
    //seq_num++;
    //rtp_hdr.seq_no[0] = seq_num >> 8;
    //rtp_hdr.seq_no[1] = seq_num & 0xff;
//    memcpy(temp_8 ,reinterpret_cast<quint8*>(&rtp_hdr), RTP_HDR_LEN );
//    memcpy(temp_8 + RTP_HDR_LEN , ba_3005.data() + 52 + 160 * sound_tim_counter, 160 );
//    if(udp_srv)
//    {
//        udp_srv->writeDatagram( reinterpret_cast<char*>(temp_8), 160 + RTP_HDR_LEN, QHostAddress(hadrr), hyt_udp_port);
//        udp_srv->flush();
//    }
//    if(sound_tim_counter == 2)
//    {
//        sound_tim.stop();
//        sound_tim_counter = 0;
//    }
}
