#ifndef TCP_H
#define TCP_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDebug>
#include <QHostAddress>
#include <QTimer>
#include <QDateTime>

//#define DBG
//#define DBG_BA
#define NANOPI_H3

#define TS_DELTA 160
#define MU_LAW
#define RTP
#define RTP_HDR_LEN 12
#define HYTRA

const quint16  RRS = 3002;
const quint16  GPS = 3003;
const quint16  TMS = 3004;
const quint16  RCP = 3005;
const quint16  TLM = 3006;
const quint16  DTR = 3007;
const quint16  SDM = 3009;

//const QString serial_num = "5883";            // white box rpi 144
//const QString serial_num = "9dfb";            // peton
//const QString serial_num = "0000";            // nanopi H5
//const QString serial_num = "1abb";            // nanopi H3
//const QString serial_num = "2c60";            //  rpi 144  2010
//const QString serial_num = "ba37";            //  rpi 150  2004
//const QString serial_num = "0dd7";            //  rpi 151  2012
//const QString serial_num = "f252";            //  rpi 152  2005
//const QString serial_num = "213a";            //  rpi 154  2007
//const QString serial_num = "0044";            //  rpi 156  2006
//const QString serial_num = "46de";            //  rpi 157  2008
//const QString serial_num = "e65b";            //  rpi 158  2003
//const QString serial_num = "212b";            //  rpi 159  2009
//const QString serial_num = "28ea";            //  rpi 162  2011
//const QString serial_num = "7f91";            //  rpi 161  2002
//const QString serial_num = "227f";            //  rpi 160(231)  2001
//const QString serial_num = "695e";              //  rpi 248  2017
const QString serial_num = "91cf";              //  rpi 239  2015


class TCP : public QObject
{
    Q_OBJECT
public:
    explicit TCP(QObject *parent = NULL);
    QTcpSocket tcp_srv;
    QUdpSocket *udp_srv;
//    QUdpSocket udpRRS_3002;
//    QUdpSocket udpGPS_3003;
//    QUdpSocket udpTMS_3004;
//    QUdpSocket udpRCP_3005;
//    QUdpSocket udpTLM_3006;
//    QUdpSocket udpDTR_3007;
//    QUdpSocket udpSDM_3009;

    QUdpSocket udp_30015;
    QUdpSocket udp_30016;

    //QTimer rad_conn_tim;
    QTimer tcp_conn_tim;
    QTimer ping_tim;
    //QTimer reload_tim;
    QTimer radio_check_tim;
    //QTimer monitor_tim;
    //QTimer rx_tim;
    QTimer tx_tim;
    //QTimer sound_tim;

    //void receive_sound();
    //void rcv_tcpRRS();
    //void rcv_tcpTMS();
    //void rcv_tcpGPS();
    void rcv_tcpRCP();
    //float get_latitude(quint32);
    //float get_longitude(quint32);
    //float get_speed(quint32);
    //quint32 get_direction(quint32);
    quint8 pi_num;

signals:

public slots:
    //void rcv_udpRRS();
    //void rcv_udpGPS();
    //void rcv_udpTMS();
    //void rcv_udpRCP();
    //void rcv_udpTLM();
    //void rcv_udpDTR();
    //void rcv_udpSDM();
    void rad_conn_tim_slot();

    void tcp_conn();
    void tcp_disconn();
    void tcp_receive();
    void udp_srv_slot();

    void tcp_conn_tim_slot();
    void ping_tim_slot();
    void reload_tim_slot();
    void radio_check_tim_slot();
    void monitor_tim_slot();
    void rx_tim_slot();
    void tx_tim_slot();
    void sound_tim_slot();

    void receive30015();
    void receive30016();


};

#endif // TCP_H
