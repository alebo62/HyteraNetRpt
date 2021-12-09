#include "tcp.h"
#include "variables.h"
#include <QSettings>
#include <QProcess>


//extern void route_rpi();
extern void msg_init();
//extern unsigned char conn[];
//extern QString host;
//extern void crc(unsigned char*, int);
extern QString kadrr;

TCP::TCP(QObject *parent) : QObject(parent)
{
 //   QSettings settings("NWCSC", "FerrAgentpi");
//    settings.setValue("/Settings/fa_login", "admin" );
//    settings.setValue("/Settings/fa_psw", "1234");
//    settings.setValue("/Settings/server_adress", "192.168.0.180");
//    settings.setValue("/Settings/onega_adress", "");
//    settings.setValue("/Settings/gw_adress", "");
//    settings.setValue("/Settings/radio_id", "");
//    settings.setValue("/Settings/channel_id", "");

    //hadrr  = settings.value("/Settings/server_adress").toString();
    //channel = settings.value("/Settings/channel_id").toString();
    //group = settings.value("/Settings/group_id").toString();

    //Settings/server_adress
    ///Settings/gw_adress
    ///Settings/radio_id
    ///Settings/channel_id

    msg_init();
    udp_srv = NULL;
    //connect(&udp_srv, SIGNAL(readyRead()),this, SLOT(udp_srv_slot()));
    //connect(&rad_conn_tim, SIGNAL(timeout()), this, SLOT(rad_conn_tim_slot()));
    connect(&tcp_conn_tim, SIGNAL(timeout()), this, SLOT(tcp_conn_tim_slot()));
    connect(&ping_tim,     SIGNAL(timeout()), this, SLOT(ping_tim_slot()));
    //connect(&reload_tim,   SIGNAL(timeout()), this, SLOT(reload_tim_slot()));
    connect(&radio_check_tim,   SIGNAL(timeout()), this, SLOT(radio_check_tim_slot()));
    //connect(&monitor_tim,   SIGNAL(timeout()), this, SLOT(monitor_tim_slot()));
    //connect(&rx_tim,   SIGNAL(timeout()), this, SLOT(rx_tim_slot()));
    connect(&tx_tim,   SIGNAL(timeout()), this, SLOT(tx_tim_slot()));
    //connect(&sound_tim,   SIGNAL(timeout()), this, SLOT(sound_tim_slot()));

    connect(&tcp_srv,SIGNAL(readyRead()),this,SLOT(tcp_receive()));
    connect(&tcp_srv,SIGNAL(disconnected()),this,SLOT(tcp_disconn()));
    connect(&tcp_srv,SIGNAL(connected()),this,SLOT(tcp_conn()));

    connect(&udp_30015, SIGNAL(readyRead()), this, SLOT(receive30015()));
    connect(&udp_30016, SIGNAL(readyRead()), this, SLOT(receive30016()));

    udp_30015.bind(QHostAddress("192.168.10.2"), 30015);
    udp_30016.bind(QHostAddress("192.168.10.2"), 30016);

    memcpy(aud_tx ,  sound_msg, 52);
    //rad_conn_tim.setInterval(3000);
    //monitor_tim.setInterval(18000);
    //rad_conn_tim.start();
    //udpRCP_3005.writeDatagram((char*)conn, 12, QHostAddress(host), RCP);
    tcp_conn_tim.setInterval(3000);
    ping_tim.setInterval(11000);
    //reload_tim.setInterval(1000);
    radio_check_tim.setInterval(100);
    //rx_tim.setInterval(5000);
    tx_tim.setInterval(50);
    //sound_tim.setInterval(15);

    tcp_conn_tim.start(3000);
    tcp_srv.connectToHost(hadrr, 40042);

}




