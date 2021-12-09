#include "tcp.h"
#include "messages.h"
#include "enums.h"

extern quint8       ars_answer[16];
extern quint8       ars_check[11];
extern QString      dest_rrs_ip;
extern strMsgHeader header;
extern QString      host;
extern quint8       req_id[];
extern strRMRpt 	sRegMsgReport;
extern strARSmsg    sARSmsg;
extern QByteArray   tcp_tx, ba;
extern quint32      check_online;
extern strCtrlRpl   sCtrlReply;

extern void checksum(unsigned char*);
extern void crc(unsigned char*, int);


QString         ars_addr;
QByteArray      ba_3002;
unsigned char   buf_tx[64];


void TCP::rcv_udpRRS()
{
    ba_3002.resize(static_cast<int>(udpRRS_3002.pendingDatagramSize()));
    udpRRS_3002.readDatagram(ba_3002.data(), ba_3002.size());
#ifdef DBG_BA
    qDebug() << "rrs " << ba_3002.toHex();
#endif

    switch (ba_3002.at(0)) {
    case 0x11:// RRS
        switch (ba_3002.at(2)) {
        case RegistrationRrs : // RadioIP[4]
            sARSmsg.userId[0] = 0;
            sARSmsg.userId[1] = ba_3002.at(6);
            sARSmsg.userId[2] = ba_3002.at(7);
            sARSmsg.userId[3] = ba_3002.at(8);
            sARSmsg.arsMsgType = RegMessage;
            tcp_tx.resize(sizeof(sARSmsg));
            memmove(tcp_tx.data(), &sARSmsg, sizeof(sARSmsg));
            if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                tcp_srv.write(tcp_tx, sizeof(sARSmsg));
            break;
        case DeRegistrationRrs:// RadioIP[4]
            sARSmsg.userId[0] = 0;
            sARSmsg.userId[1] = ba_3002.at(6);
            sARSmsg.userId[2] = ba_3002.at(7);
            sARSmsg.userId[3] = ba_3002.at(8);
            sARSmsg.arsMsgType = DeregMessage;
            tcp_tx.resize(sizeof(sARSmsg));
            memmove(tcp_tx.data(), &sARSmsg, sizeof(sARSmsg));
            if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                tcp_srv.write(tcp_tx, sizeof(sARSmsg));

            break;
        case OnLineCheckAckRrs:  // RadioIP[4]+Result[1]
            if(check_online)
            {
                radio_check_tim.stop();
                check_online = 0;
                sCtrlReply.result = 0;  // success
                sCtrlReply.req_type = 0;// radio check
                radio_check_tim.stop();
                if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                {
                    tcp_srv.write((char*)&sCtrlReply, sizeof(sCtrlReply));
                }
#ifdef DBG
                qDebug() << "radio check ok!";
#endif
            }
            else
            {
                sARSmsg.userId[0] = 0;// номер сети не нужен
                sARSmsg.userId[1] = ba_3002.at(6);
                sARSmsg.userId[2] = ba_3002.at(7);
                sARSmsg.userId[3] = ba_3002.at(8);
                sARSmsg.arsMsgType = RegAck;
                sARSmsg.csbk = 0;
                memcpy(sARSmsg.req_id, req_id, 4);
                if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                {
                    tcp_srv.write((char*)&sARSmsg, sizeof(sARSmsg));
                }
            }
            break;
        default:
            break;
        }
        break;
    case 0x14:// DDS
        switch (ba_3002.at(2)) {
        case 0x11 :// RRS
                        if(check_online)
                        {
                            radio_check_tim.start();
#ifdef DBG
                qDebug() << "start check timer!";
#endif
            //                radio_check_tim.stop();
            //                check_online = 0;
            //                sCtrlReply.result = 0; // success
            //                sCtrlReply.req_type = 0;// radio check
            //                radio_check_tim.stop();
            //                tcp_srv.write((char*)&sCtrlReply, sizeof(sCtrlReply));
            //                tcp_srv.flush();
            //                qDebug() << "radio check ok";
            //            // address 6..8
            //            // opcode 9,10
            //            // result 11
                        }
            break;
        case 0x07 :// LP

            break;
        case 0x12 :// TP

            break;
        case 0x13 :// DTP

            break;
        default:
            break;
        }

        break;
    default:
        break;
    }

}

/*
 * SuRegMsgType 1 байт (4)- RadioId 4 байта (5..8)-bool UseCsbk 1 байт (9)-RequestId 4 bytes (10..13)
*/
void TCP::rcv_tcpRRS()
{
    memcpy(req_id, ba.data() + 5, 4); // only for online check answer
    ars_addr = dest_rrs_ip + QString::number(ba.at(6))+ "."+QString::number(ba.at(7))+"."+ QString::number(ba.at(8));
    switch (ba.at(4)) {
    case SuccessReg:
        ars_answer[9] = 0;
        ars_answer[6] = ba.at(6);
        ars_answer[7] = ba.at(7);
        ars_answer[8] = ba.at(8);
        crc(ars_answer, 16);
        udpRRS_3002.writeDatagram((char*)ars_answer, 16, QHostAddress(ars_addr), RRS);
        udpRRS_3002.flush();
        break;
    case FailureReg:
        ars_answer[9] = 1;
        ars_answer[6] = ba.at(6);
        ars_answer[7] = ba.at(7);
        ars_answer[8] = ba.at(8);
        crc(ars_answer, 16);
        udpRRS_3002.writeDatagram((char*)ars_answer, 16, QHostAddress(ars_addr), RRS);
        udpRRS_3002.flush();
        break;
    case QueryMessage:
        ars_check[6] = ba.at(6);
        ars_check[7] = ba.at(7);
        ars_check[8] = ba.at(8);
        crc(ars_check, 11);
        udpRRS_3002.writeDatagram((char*)ars_check, 11, QHostAddress(ars_addr), RRS);
        udpRRS_3002.flush();
        break;
    default:
#ifdef DBG
        qDebug() << "Unknown rrs ";
#endif
        break;
    }
}
