#include "cstring"
#include "tcp.h"
#include "messages.h"


using namespace std;

//extern QByteArray ba;
//extern QString hadrr, host;
//extern quint16 hyterus_port;
//extern quint16 hyt_udp_port;
//extern QByteArray ba_3005;

#define TS_DELTA 160
#define RTP_HDR_LEN 12
#define htonl(A)    (((static_cast<unsigned long>(A) & 0xff000000) >> 24) | \
    ((static_cast<unsigned long>(A) & 0x00ff0000) >> 8)  | \
    ((static_cast<unsigned long>(A) & 0x0000ff00) << 8)  | \
    ((static_cast<unsigned long>(A) & 0x000000ff) << 24))

//unsigned char temp_8[532];
//quint16 seq_num;
//extern quint32 ts_current;
extern rtp_header rtp_hdr;
//extern quint8 aud_tx[532];
//extern volatile quint16 msg_cnt;
//extern quint32          Radio_Reg_State;
//extern quint32 start_sound;
//QByteArray baUdpHyt;
//quint16 len;
//quint32 timestamp;
quint8 rtp_sound[508] = {0x90,0,0x39,0xb2,0x39,0xb2,0x92,0xe4,0x39,0xb2,0x85,0xc4,0,0x15,0,3,0,0,0,0,0,0,0,0,0,0,0,0};
QByteArray baUdpKup;
static quint16 seq_num = 0x39b2;
quint32 time_stamp = 0x39b285c4;
extern quint8 mu_law;


void TCP::udp_srv_slot() //  sound from server
{
    qint32 len;
//    try{
#ifdef AUDIO_CARD
        sound1_timer.start(2000);
        baUdpKup1.resize(udpKupol1.pendingDatagramSize());
        udpKupol1.readDatagram(braUdpKup1.data(), baUdpKup1.size());
        device->write((char*)(baUdpKup1.data() ), baUdpKup1.size());
        qDebug() << "...";
#else
        baUdpKup.resize(static_cast<int>(udp_srv->pendingDatagramSize()));
        len = baUdpKup.size();
        udp_srv->readDatagram(baUdpKup.data(), len);

#ifdef DBG_BA
        qDebug() << "udp1_sound " << " : " << baUdpKup.size();
#endif

#ifdef MU_LAW

#ifdef RTP
        if(len == (480 + 28))
        {
            if(baUdpKup.at(1) == 0)// muLaw for rpt default
            {
                rtp_hdr.payload_type = 0;
                mu_law = 0;
                rtp_sound[2] = (seq_num >> 8) & 0xFF;
                rtp_sound[3] = seq_num & 0xFF;
                seq_num++;
                rtp_sound[4] = (time_stamp >> 24) & 0xFF;
                rtp_sound[5] = (time_stamp >> 16) & 0xFF;
                rtp_sound[6] = (time_stamp >>  8) & 0xFF;
                rtp_sound[7] =  time_stamp        & 0xFF;
                time_stamp+=480;
                // in analog mode audio must send each 20ms!!! 160-160-160
                memcpy(rtp_sound + 28, baUdpKup.data() + 28, 160);
                udp_30016.writeDatagram((char*)rtp_sound, 188, QHostAddress("192.168.10.1"), 30016);

                rtp_sound[2] = (seq_num >> 8) & 0xFF;
                rtp_sound[3] = seq_num & 0xFF;
                seq_num++;
                rtp_sound[4] = (time_stamp >> 24) & 0xFF;
                rtp_sound[5] = (time_stamp >> 16) & 0xFF;
                rtp_sound[6] = (time_stamp >>  8) & 0xFF;
                rtp_sound[7] =  time_stamp        & 0xFF;
                time_stamp+=480;
                memcpy(rtp_sound + 28, baUdpKup.data() + 28 + 160, 160);
                udp_30016.writeDatagram((char*)rtp_sound, 188, QHostAddress("192.168.10.1"), 30016);

                rtp_sound[2] = (seq_num >> 8) & 0xFF;
                rtp_sound[3] = seq_num & 0xFF;
                seq_num++;
                rtp_sound[4] = (time_stamp >> 24) & 0xFF;
                rtp_sound[5] = (time_stamp >> 16) & 0xFF;
                rtp_sound[6] = (time_stamp >>  8) & 0xFF;
                rtp_sound[7] =  time_stamp        & 0xFF;
                time_stamp+=480;
                memcpy(rtp_sound + 28, baUdpKup.data() + 28 + 320, 160);
                udp_30016.writeDatagram((char*)rtp_sound, 188, QHostAddress("192.168.10.1"), 30016);
            }
//            else
//            {
//                rtp1_hdr.payload_type = 8;
//                mu_law1 = 8;
//                for(i = 0; i < 480; i++)
//                    temp_16[i] =  static_cast<quint16>(ALaw_Decode(static_cast<qint8>(baUdpKup1.at(i + 28))));
//                memcpy(sound1_tx.data() + 1 ,  reinterpret_cast<quint8*>(temp_16), 960 );
//                udp1_opt_board.writeDatagram(sound1_tx.data(), 961, QHostAddress("192.168.10.1"), 4004);
//            }
        }
        else if(len == (960 + 28))
        {
//            mu_law1 = 96;
//            rtp1_hdr.payload_type = 96;
//            sound1_timer.start(2000);
//            for(i = 0; i < 960; i+=2)
//            {
//                temp_8[(i) + 1] = baUdpKup1.at((i) + 1 + 28);
//                temp_8[(i) + 2] = baUdpKup1.at((i) + 28);
//            }
//            udp1_opt_board.writeDatagram(reinterpret_cast<char*>(temp_8), 961, QHostAddress(radrr1), 4004);

        }
#else
        if(len == 480)
        {
            if(mu_law1 == 1)
            {
                for(i = 0; i < 480; i++)
                    temp_16[i] = MuLaw_Decode(baUdpKup1.at(i));
            }
            else
            {
                for(i = 0; i < 480; i++)
                    temp_16[i] = ALaw_Decode(baUdpKup1.at(i));
            }
            sound1_timer.start(2000);
            memcpy(sound1_tx.data() + 1 ,  reinterpret_cast<quint8*>(temp_16), 960 );
            udp1_opt_board.writeDatagram(sound1_tx.data(), 961, QHostAddress(radrr1), 4004);
        }
#endif

#else
        if(len == 960)
        {
            sound1_timer.start(2000);
            memcpy(sound1_tx.data() + 1 , baUdpKup1.data(), 960 );
            udp1_opt_board.writeDatagram(sound1_tx.data(), 961, QHostAddress(radrr1), 4004);
#ifdef DBG_BA
            qDebug() << "send to board1";
#endif
        }
#endif
#endif
//    }
//    catch (std::exception &e) {
//        qCritical("Error in module tcp1r standart exp %s",
//                  e.what());//, typeid(*event).name(), qPrintable(receiver->objectName()),
//        //typeid(*receiver).name());
//    } catch (...) {
//        qCritical("Error in module tcp1r unnown exp");//,
//        //typeid(*event).name(), qPrintable(receiver->objectName()),
//        //typeid(*receiver).name());
//    }
}


//void TCP::udp_srv_slot()
//{
//    baUdpHyt.resize(static_cast<int>(udp_srv->pendingDatagramSize()));
//    udp_srv->readDatagram(baUdpHyt.data(), baUdpHyt.size());
//    //qDebug() << start_sound;
//    if(start_sound)
//    {
//#ifdef DBG
//        qDebug() << "sound>>";
//#endif
//        memmove(aud_tx + 52 , baUdpHyt.data() + 28  , 480);
//        seq_num++;
//        aud_tx[14] = seq_num >> 8;
//        aud_tx[15] = seq_num & 0xFF;
//        timestamp ++;
//        aud_tx[16] = timestamp >> 24;
//        aud_tx[17] = timestamp >> 16 & 0xFF;
//        aud_tx[18] = (timestamp >> 8) & 0xFF;
//        aud_tx[19] = timestamp & 0xFF;

//        udpRCP_3005.writeDatagram((char*)aud_tx, 532, QHostAddress(host), RCP);
//        //udpRCP_3005.flush();
//        tx_tim.start();
//    }
//}

//void TCP::receive_sound()
//{
//    ts_current += TS_DELTA;
//    rtp_hdr.timestamp = htonl(ts_current);
//    seq_num++;
//    rtp_hdr.seq_no[0] = seq_num >> 8;
//    rtp_hdr.seq_no[1] = seq_num & 0xff;
//    memcpy(temp_8 ,reinterpret_cast<quint8*>(&rtp_hdr), RTP_HDR_LEN );
//    memcpy(temp_8 + RTP_HDR_LEN , ba_3005.data() + 52, 160 );
//    if(udp_srv)
//    {
//        udp_srv->writeDatagram( reinterpret_cast<char*>(temp_8), 160 + RTP_HDR_LEN, QHostAddress(hadrr), hyt_udp_port);
//        udp_srv->flush();
//    }
//    sound_tim.start(); // 15msec timer
//}

