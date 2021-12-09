#include "tcp.h"
#include "messages.h"
//#include <string>
#include "enums.h"
#include <QtEndian>
volatile quint16 msg_cnt = 0;
quint32 state = 0;
unsigned char ack[] =    {0x7e,3,0,0x10,0x20,0x10,0,0,0,0x0C,0x60,0x00};
unsigned char conn[] =   {0x7e,4,0,0xfe,0x20,0x10,0,0,0,0x0c,0x60,0xe1};

extern hrnp_t         remote_monitor;
extern hrnp_t         dig_audio_tx_req;
extern hrnp_t         dig_audio_rx_req;
extern hrnp_t         rtp_pld_req;
extern hrnp_t         br_cast_req;
extern hrnp_t         radio_id;
extern hrnp_t         chan_status;
extern hrnp_t         serialNum_req;
extern hrnp_t         zone_ch;
extern hrnp_t         key_notific_req;
extern hrnp_t         txt_notific_req;
extern hrnp_t         bc_txt_notific_req;
extern hrnp_t         br_cast_analog;
extern hrnp_no_pld_t  pwr_up_chk_req;
extern hrnp_no_pld_t  ack_msg;
extern hrnp_no_pep_t  conn_req;
extern hrnp_no_pep_t  close_req;
extern hrnp_t 		  txt_notif_req;
extern hrnp_t		  txt_msg_tx;
extern hrnp_t		  uart_config_req;
extern hrnp_t         ptt_press_req;
extern strRMRpt 	  sRegMsgReport;
extern strARSmsg      sARSmsg;
extern QByteArray     tcp_tx;
extern strCRp        sCallReply;
extern strCRpt       sCallReport;
extern strCRp        sCallReply;
extern strCSRp       sCallStopReply;
extern strCtrlRpl    sCtrlReply;
extern strCCRpl      sChanChangeReply;
extern strCCRpt      sChanChangeReport;
extern strRldRpl     sReloadReplay;
extern strCtrlRpl    sCtrlReply;
extern char* sn;
extern quint8 req_id[];
extern void checksum(unsigned char*);
extern QString host;
extern unsigned char buf_tx[64];
extern quint32 Radio_Reg_State;
extern quint32 enable_online;
extern quint32 disable_online;
extern quint32 monitor_online;
quint8 CallsDecoder[] = {3,1,5,2,4,6};
extern void receive_sound();
extern quint32 start_sound;
extern quint32 virt_hadrr;
QByteArray ba_3005;

void TCP::rcv_udpRCP()
{
    ba_3005.resize(static_cast<int>(udpRCP_3005.pendingDatagramSize()));
    udpRCP_3005.readDatagram(ba_3005.data(), ba_3005.size());
    //qDebug() << "rcv RCP " << ba_3005.length();;
    if(Radio_Reg_State == INIT_STATE)
    {
        if(ba_3005.at(3) == 0xFD)
        {
#ifdef DBG
            qDebug() << "FD";
#endif
            state = 1;
            rad_conn_tim.stop();
            ack[4] = pi_num = ba_3005.at(5);
            checksum(ack);
            //checksum(sbcr_1);
            msg_cnt++;
            pwr_up_chk_req.packet_num[0] = msg_cnt >> 8;// page 159
            pwr_up_chk_req.packet_num[1] = msg_cnt & 0xFF;
            memcpy(buf_tx , reinterpret_cast<char*>(&pwr_up_chk_req), pwr_up_chk_req.length[1]);
            checksum(buf_tx);
            udpRCP_3005.writeDatagram((char*)buf_tx, pwr_up_chk_req.length[1], QHostAddress(host), RCP);
            udpRCP_3005.flush();
        }
        else if((ba_3005.at(13) == 0xC6) && (ba_3005.at(14) == 0x80) && (state == 1))
        {
            state = 2;
            udpRCP_3005.writeDatagram((char*)ack, 12, QHostAddress(host), RCP);
            udpRCP_3005.flush();
            if(ba_3005.at(17) == 0)
            {
#ifdef DBG
                qDebug() << "Radio_Reg_State : POWER UP check OK";
#endif
                msg_cnt++;
                zone_ch.packet_num[0] = msg_cnt >> 8;  // page 96
                zone_ch.packet_num[1] = msg_cnt & 0xFF;
                memcpy(buf_tx, reinterpret_cast<char*>(&zone_ch), 17 );
                memcpy(buf_tx + 17, reinterpret_cast<char*>(zone_ch.pep.pld), zone_ch.pep.num_of_bytes[0]);
                memcpy(buf_tx + 17 + zone_ch.pep.num_of_bytes[0], reinterpret_cast<char*>(&zone_ch + 21), 2 );
                checksum(buf_tx);
                udpRCP_3005.writeDatagram((char*)buf_tx, zone_ch.length[1], QHostAddress(host), RCP);
                udpRCP_3005.flush();
            }
        }
        else if((ba_3005.at(13) == 0xC4)&& (ba_3005.at(14) == 0x80) && (state == 2))// && (rx[17] == 0))
        {
            state = 3;
            udpRCP_3005.writeDatagram((char*)ack, 12, QHostAddress(host), RCP);
            udpRCP_3005.flush();
            if((ba_3005.at(17) == 0) && (ba_3005.at(18) == 0) && (ba_3005.at(19) == 0) && (ba_3005.at(20) == 0))
            {
#ifdef DBG
                qDebug() << "Zone Channel OK";
#endif
                //Radio_Reg_State = ZONE_CH;
                sRegMsgReport.zone[1] = ba_3005.at(25);
                sRegMsgReport.zone[0] = ba_3005.at(26);
                sRegMsgReport.channel[1] = ba_3005.at(27);
                sRegMsgReport.channel[0] = ba_3005.at(28);
                msg_cnt++;
                serialNum_req.packet_num[0] = msg_cnt >> 8;  // page 98
                serialNum_req.packet_num[1] = msg_cnt & 0xFF;
                memcpy(buf_tx, reinterpret_cast<char*>(&serialNum_req), 17 );
                memcpy(buf_tx + 17, reinterpret_cast<char*>(serialNum_req.pep.pld), serialNum_req.pep.num_of_bytes[0]);
                memcpy(buf_tx + 17 + serialNum_req.pep.num_of_bytes[0], reinterpret_cast<char*>(&serialNum_req + 21), 2 );
                checksum(buf_tx);
                udpRCP_3005.writeDatagram((char*)buf_tx, serialNum_req.length[1], QHostAddress(host), RCP);
                udpRCP_3005.flush();
            }
            else
            {
#ifdef DBG
                qDebug() << "rx Zone Channel fail ";
#endif
            }
        }
        else if((ba_3005.at(13) == 0x01)&& (ba_3005.at(14) == 0x82) && (state == 3))// && (rx[17] == 0))
        {
            state = 4;
            udpRCP_3005.writeDatagram((char*)ack, 12, QHostAddress(host), RCP);
            udpRCP_3005.flush();
            if((ba_3005.at(17) == 0) && (ba_3005.at(18) == 0) && (ba_3005.at(19) == 0) && (ba_3005.at(20) == 0))
            {
#ifdef DBG
                qDebug() << "S/N OK";  //  8digits 45020005+00
#endif
                //        Radio_Reg_State = SER_NUM;
                //                for(int i = 0; i < 10; i++){
                //                    sRegMsgReport.ser_num[i] = *(sn+9-i);// reverse string

                //                    //qDebug() << sRegMsgReport.ser_num[i];
                //                }
                //                memcpy(&sRegMsgReport.ser_num, "511TPH2798", 10 );
                for(int i = 0; i < 8; i++)
                    sRegMsgReport.ser_num[9-i] = static_cast<unsigned char>(ba_3005.at(25 + (i << 1)));//  but len == 64
                sRegMsgReport.ser_num[0] = sRegMsgReport.ser_num[1] = 0x30;
                msg_cnt++;
                chan_status.packet_num[0] = msg_cnt >> 8; // page 103
                chan_status.packet_num[1] = msg_cnt & 0xFF;
                memcpy(buf_tx, reinterpret_cast<char*>(&chan_status), 17 );
                memcpy(buf_tx + 17, reinterpret_cast<char*>(chan_status.pep.pld), chan_status.pep.num_of_bytes[0]);
                memcpy(buf_tx + 17 + chan_status.pep.num_of_bytes[0], reinterpret_cast<char*>(&chan_status + 21), 2 );
                checksum(buf_tx);
                udpRCP_3005.writeDatagram((char*)buf_tx, chan_status.length[1], QHostAddress(host), RCP);
                udpRCP_3005.flush();
            }
            else{
#ifdef DBG
                qDebug() << "rx S/N msg fail " ;
#endif
            }
        }
        else if((ba_3005.at(13) == 0xE7)&& (ba_3005.at(14) == 0x80) && (state == 4))// && (rx[17] == 0))
        {
            state = 5;
            udpRCP_3005.writeDatagram((char*)ack, 12, QHostAddress(host), RCP);
            udpRCP_3005.flush();
            if(ba_3005.at(17) == 0)
            {
#ifdef DBG
                qDebug() << "Chan status OK";
#endif

                //0: conventional digital
                //1: conventional analog
                //2: repeater digital
                //3: repeater analog
                //4: trunking digital
                //5: trunking analog
                if(ba_3005.at(20) == 1)
                    sRegMsgReport.signal_mode = 0;
                else
                    sRegMsgReport.signal_mode = 1;

                radio_id.packet_num[0] = msg_cnt >> 8;  // page 139
                radio_id.packet_num[1] = msg_cnt & 0xFF;
                memcpy(buf_tx, reinterpret_cast<char*>(&radio_id), 17 );
                memcpy(buf_tx + 17, reinterpret_cast<char*>(radio_id.pep.pld), radio_id.pep.num_of_bytes[0]);
                memcpy(buf_tx + 17 + radio_id.pep.num_of_bytes[0], reinterpret_cast<char*>(&radio_id + 21), 2 );
                checksum(buf_tx);
                udpRCP_3005.writeDatagram((char*)buf_tx, radio_id.length[1], QHostAddress(host), RCP);
                udpRCP_3005.flush();
            }
            else{
#ifdef DBG
                qDebug() << "rx Chan Status msg fail ";
#endif
            }
        }
        else if((ba_3005.at(13) == 0x52)&& (ba_3005.at(14) == 0x84) && (state == 5))
        {
            state = 6;
            udpRCP_3005.writeDatagram((char*)ack, 12, QHostAddress(host), RCP);
            udpRCP_3005.flush();
            if(ba_3005.at(17) == 0)
            {
#ifdef DBG
                qDebug() << "Radio Id OK";
#endif
                sRegMsgReport.header.msgType = 1;
                sRegMsgReport.header.payloadLength[0] = 0;
                sRegMsgReport.header.payloadLength[1] = 24;

                sRegMsgReport.type = eRegReportMsg;
                sRegMsgReport.none = 0x00;

                sRegMsgReport.len[0] = 0;
                sRegMsgReport.len[1] = 10;

                if(virt_hadrr)
                {
                    virt_hadrr = qToBigEndian(virt_hadrr);
                    memcpy(sRegMsgReport.radio_id, (char*)&virt_hadrr, 4);

                }
                else
                {
                    sRegMsgReport.radio_id[3] = static_cast<unsigned char>(ba_3005.at(19));
                    sRegMsgReport.radio_id[2] = static_cast<unsigned char>(ba_3005.at(20));
                    sRegMsgReport.radio_id[1] = static_cast<unsigned char>(ba_3005.at(21));
                    sRegMsgReport.radio_id[0] = static_cast<unsigned char>(ba_3005.at(22));
                }
                sRegMsgReport.reg_unreg_state = REGISTRATE;

                tcp_conn_tim.start();

                // 01001800 01 00 000a 30303530303032303534 0001 0001 00 00000001 01
                // QByteArray ba;
                // ba.resize(sizeof(sRegMsgReport));
                // memcpy((char*)ba.data(), (char*)&sRegMsgReport, sizeof(sRegMsgReport));
                // qDebug() << ba.toHex();
                msg_cnt++;
                key_notific_req.packet_num[0] = msg_cnt >> 8;  // audio route rx tx  page232
                key_notific_req.packet_num[1] = msg_cnt & 0xFF;
                memcpy(buf_tx, reinterpret_cast<char*>(&key_notific_req), 17 );
                memcpy(buf_tx + 17, reinterpret_cast<char*>(key_notific_req.pep.pld), key_notific_req.pep.num_of_bytes[0]);
                memcpy(buf_tx + 17 + key_notific_req.pep.num_of_bytes[0], reinterpret_cast<char*>(&key_notific_req + 21), 2 );
                checksum(buf_tx);
                udpRCP_3005.writeDatagram((char*)buf_tx, key_notific_req.length[1], QHostAddress(host), RCP);
                udpRCP_3005.flush();
            }
            else{
#ifdef DBG
                qDebug() << "rx Radio id fail " ;
#endif
            }
        }
        else if((ba_3005.at(13) == 0xE4)&& (ba_3005.at(14) == 0x80) && (state == 6))
        {
            state = 7;
            udpRCP_3005.writeDatagram((char*)ack, 12, QHostAddress(host), RCP);
            udpRCP_3005.flush();
            if(ba_3005.at(17) == 0)
            {
#ifdef DBG
                qDebug() << "Key Notification OK";
#endif
                msg_cnt++;
                //Radio_Reg_State = BRD_TXT_NOTIFIC;
                bc_txt_notific_req.packet_num[0] = msg_cnt >> 8;  // audio route rx tx  page232
                bc_txt_notific_req.packet_num[1] = msg_cnt & 0xFF;
                memcpy(buf_tx, reinterpret_cast<char*>(&bc_txt_notific_req), 17 );
                memcpy(buf_tx + 17, reinterpret_cast<char*>(bc_txt_notific_req.pep.pld), bc_txt_notific_req.pep.num_of_bytes[0]);
                memcpy(buf_tx + 17 + bc_txt_notific_req.pep.num_of_bytes[0], reinterpret_cast<char*>(&bc_txt_notific_req + 21), 2 );
                checksum(buf_tx);
                udpRCP_3005.writeDatagram((char*)buf_tx, bc_txt_notific_req.length[1], QHostAddress(host), RCP);
                udpRCP_3005.flush();
            }
            else{
#ifdef DBG
                qDebug() << "Key Notification Fail";
#endif
            }
        }
        else  if((ba_3005.at(13) == 0x47)&& (ba_3005.at(14) == 0x88) && (state == 7))
        {
            state = 8;
            udpRCP_3005.writeDatagram((char*)ack, 12, QHostAddress(host), RCP);
            udpRCP_3005.flush();
            if(ba_3005.at(17) == 0)
            {
#ifdef DBG
                qDebug() << "BRD_TXT Notification OK";
#endif
                msg_cnt++;
                //Radio_Reg_State = TXT_NOTIFIC;
                rtp_pld_req.packet_num[0] = msg_cnt >> 8;  // audio route rx tx  page232
                rtp_pld_req.packet_num[1] = msg_cnt & 0xFF;
                memcpy(buf_tx, reinterpret_cast<char*>(&rtp_pld_req), 17 );
                memcpy(buf_tx + 17, reinterpret_cast<char*>(rtp_pld_req.pep.pld), rtp_pld_req.pep.num_of_bytes[0]);
                memcpy(buf_tx + 17 + rtp_pld_req.pep.num_of_bytes[0], reinterpret_cast<char*>(&rtp_pld_req + 21), 2 );
                checksum(buf_tx);
                udpRCP_3005.writeDatagram((char*)buf_tx, rtp_pld_req.length[1], QHostAddress(host), RCP);
                udpRCP_3005.flush();
            }
            else
            {
#ifdef DBG
                qDebug() << "Key Notification Fail";
#endif
            }
        }
        else if((ba_3005.at(13) == 0x19)&& (ba_3005.at(14) == 0x84) && (state == 8))
        {
            state = 9;
            udpRCP_3005.writeDatagram((char*)ack, 12, QHostAddress(host), RCP);
            udpRCP_3005.flush();
            if(ba_3005.at(17) == 0x00)
            {
#ifdef DBG
                qDebug() << "RTP Success";
#endif
                msg_cnt++;
                //Radio_Reg_State = ROUTE_TX;
                dig_audio_rx_req.packet_num[0] = msg_cnt >> 8;  // audio route rx tx  page232
                dig_audio_rx_req.packet_num[1] = msg_cnt & 0xFF;
                memcpy(buf_tx, reinterpret_cast<char*>(&dig_audio_rx_req), 17 );
                memcpy(buf_tx + 17, reinterpret_cast<char*>(dig_audio_rx_req.pep.pld), dig_audio_rx_req.pep.num_of_bytes[0]);
                memcpy(buf_tx + 17 + dig_audio_rx_req.pep.num_of_bytes[0], reinterpret_cast<char*>(&dig_audio_rx_req + 21), 2 );
                checksum(buf_tx);
                udpRCP_3005.writeDatagram((char*)buf_tx, dig_audio_rx_req.length[1], QHostAddress(host), RCP);
                udpRCP_3005.flush();
            }
            else
            {
#ifdef DBG
                qDebug() << " RTP Fail";
#endif
            }
        }
        else if((ba_3005.at(13) == 0xDF)&& (ba_3005.at(14) == 0x80) && (state == 9))
        {
            state = 10;
            udpRCP_3005.writeDatagram((char*)ack, 12, QHostAddress(host), RCP);
            udpRCP_3005.flush();
            if(ba_3005.at(17) == 0x00)
            {
#ifdef DBG
                qDebug() << "Rx route Success";
#endif
                msg_cnt++;
                dig_audio_tx_req.packet_num[0] = msg_cnt >> 8;  // audio route rx tx  page232
                dig_audio_tx_req.packet_num[1] = msg_cnt & 0xFF;
                memcpy(buf_tx, reinterpret_cast<char*>(&dig_audio_tx_req), 17 );
                memcpy(buf_tx + 17, reinterpret_cast<char*>(dig_audio_tx_req.pep.pld), dig_audio_tx_req.pep.num_of_bytes[0]);
                memcpy(buf_tx + 17 + dig_audio_tx_req.pep.num_of_bytes[0], reinterpret_cast<char*>(&dig_audio_tx_req + 21), 2 );
                checksum(buf_tx);
                udpRCP_3005.writeDatagram((char*)buf_tx, dig_audio_tx_req.length[1], QHostAddress(host), RCP);
                udpRCP_3005.flush();
            }
            else
            {
#ifdef DBG
                qDebug() << "Rx route fail";
#endif
            }
        }
        else if((ba_3005.at(13) == 0xDF)&& (ba_3005.at(14) == 0x80) && (state == 10))
        {
            state = 11;
            udpRCP_3005.writeDatagram((char*)ack, 12, QHostAddress(host), RCP);
            udpRCP_3005.flush();
            if(ba_3005.at(17) == 0x00)
            {
#ifdef DBG
                qDebug() << "Tx route Success";
#endif
                msg_cnt++;
                br_cast_req.packet_num[0] = msg_cnt >> 8; // broadcast rx and tx  page80
                br_cast_req.packet_num[1] = msg_cnt & 0xFF;
                memcpy(buf_tx, reinterpret_cast<char*>(&br_cast_req), 17 );
                memcpy(buf_tx + 17, reinterpret_cast<char*>(br_cast_req.pep.pld), br_cast_req.pep.num_of_bytes[0]);
                memcpy(buf_tx + 17 + br_cast_req.pep.num_of_bytes[0], reinterpret_cast<char*>(&br_cast_req + 21), 2 );
                checksum(buf_tx);
                udpRCP_3005.writeDatagram((char*)buf_tx, br_cast_req.length[1], QHostAddress(host), RCP);
                udpRCP_3005.flush();
            }
            else{
#ifdef DBG
                qDebug() << "Tx route Fail";
#endif
            }
        }
        else if((ba_3005.at(13) == 0xC9)&& (ba_3005.at(14) == 0x80) && (state == 11))
        {
            Radio_Reg_State = READY; // End INitialization !!!!!!!!!!!
            udpRCP_3005.writeDatagram((char*)ack, 12, QHostAddress(host), RCP);
            udpRCP_3005.flush();
            if(ba_3005.at(17) == 0x00)
            {
#ifdef DBG
                qDebug() << "BroadCast Success";
#endif
            }
            else {
#ifdef DBG
                qDebug() << "BroadCast Fail";
#endif
            }
        }
    }
    else
    {
        //qDebug() << "rcv RCP " << ba_3005.toHex();
        if((ba_3005.at(13) == 0x49) && (ba_3005.at(14) == 0x88))// radio disable answer
        {
            disable_online = 0;
            sCtrlReply.radio_id[0] = ba_3005.at(21);
            sCtrlReply.radio_id[1] = ba_3005.at(20);
            sCtrlReply.radio_id[2] = ba_3005.at(19);
            sCtrlReply.radio_id[3] = ba_3005.at(18);
            sCtrlReply.result =  ba_3005.at(17);
            sCtrlReply.req_type = 0x01;
            if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
            {
                tcp_srv.write(reinterpret_cast<char*>(&sCtrlReply), sizeof(sCtrlReply));
                tcp_srv.flush();
            }
#ifdef DBG
            qDebug() << "radio disable:" << sCtrlReply.result;
#endif
        }
        else if ((ba_3005.at(13) == 0x4A) && (ba_3005.at(14) == 0x88)) // radio enable answer
        {
            enable_online = 0;
            sCtrlReply.radio_id[0] = ba_3005.at(21);
            sCtrlReply.radio_id[1] = ba_3005.at(20);
            sCtrlReply.radio_id[2] = ba_3005.at(19);
            sCtrlReply.radio_id[3] = ba_3005.at(18);
            sCtrlReply.result =  ba_3005.at(17);
            sCtrlReply.req_type = 0x02;
            if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
            {
                tcp_srv.write(reinterpret_cast<char*>(&sCtrlReply), sizeof(sCtrlReply));
                tcp_srv.flush();
            }
#ifdef DBG
            qDebug() << "radio enable:" << sCtrlReply.result;
#endif
        }
        else if ((ba_3005.at(13) == 0x34) && (ba_3005.at(14) == 0x88)) // Remote Monitor Ack
            // 02 3488 0500 0a                 6f000000 f803  rm ack fail
        {   // 02 3488 0500 00                 6f000000 0203  rm ack success
            // 02 44b8 0c00 0100 0400 00000000 6f000000 b603  call broadcast
            // 02 44b8 0c00 0300 0400 00000000 6f000000 b403  call broadcast
            monitor_online = 0;
            sCtrlReply.radio_id[0] = ba_3005.at(21);
            sCtrlReply.radio_id[1] = ba_3005.at(20);
            sCtrlReply.radio_id[2] = ba_3005.at(19);
            sCtrlReply.radio_id[3] = ba_3005.at(18);

            monitor_tim.stop();
            if(ba_3005.at(17))
            {
                sCtrlReply.result = 1;
#ifdef DBG
                qDebug() << "monitor online fail";
#endif
            }
            else{
                sCtrlReply.result = 0;
#ifdef DBG
                qDebug() << "monitor online ok";
#endif
            }
            sCtrlReply.req_type = 0x03;
            if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
            {
                tcp_srv.write(reinterpret_cast<char*>(&sCtrlReply), sizeof(sCtrlReply));
                tcp_srv.flush();
            }
        }
        switch (Radio_Reg_State)
        {
        case WAIT_CALL_REPLY:
            if((ba_3005.at(13) == 0x41) && (ba_3005.at(14) == 0x88))
            {
                if(ba_3005.at(17) == eSucces)
                {
                    sCallReply.requestResult = eSucces;
                    Radio_Reg_State = WAIT_CALL_REPORT;
                    tcp_srv.write(reinterpret_cast<char*>(&sCallReply), sizeof(sCallReply));
                    msg_cnt++;
                    ptt_press_req.packet_num[0] = msg_cnt >> 8;// page 159
                    ptt_press_req.packet_num[1] = msg_cnt & 0xFF;
                    memcpy(buf_tx, reinterpret_cast<char*>(&ptt_press_req), 17 );
                    memcpy(buf_tx + 17, reinterpret_cast<char*>(ptt_press_req.pep.pld), ptt_press_req.pep.num_of_bytes[0]);
                    memcpy(buf_tx + 17 + ptt_press_req.pep.num_of_bytes[0], reinterpret_cast<char*>(&ptt_press_req + 21), 2 );
                    checksum(buf_tx);
                    udpRCP_3005.writeDatagram((char*)buf_tx, ptt_press_req.length[1], QHostAddress(host), RCP);
                    udpRCP_3005.flush();
                }
                else
                {
                    sCallReply.requestResult = eFailure;
                    Radio_Reg_State = READY;// a если был прием???
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReply), sizeof(sCallReply));
                        tcp_srv.flush();
                    }
                }
            }
            break;
        case WAIT_CALL_REPORT:
            if((ba_3005.at(13) == 0x41) && (ba_3005.at(14) == 0x80)) // page 94
            {

                if(ba_3005.at(17) == 0)
                {
#ifdef DBG
                    qDebug() << "41B8 0";
#endif
                    //                    memcpy( sCallReport.calledId, sRegMsgReport.radio_id , 4);// its my id
                    //                    sCallReport.callState = eCallInit;
                    //                    tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                }
                else
                {
#ifdef DBG
                    qDebug() << "41B8 1";
#endif
                    Radio_Reg_State = READY;
                    sCallReport.callState = eCallEnded;
                    memcpy( sCallReport.calledId, sRegMsgReport.radio_id , 4);// its my id
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                        tcp_srv.flush();
                    }
                }
            }
            else if((ba_3005.at(13) == 0x43) && (ba_3005.at(14) == 0xB8))
            {
#ifdef DBG
                qDebug() << "43B8";
#endif
                if(ba_3005.at(17) == 1)// voice call
                {
#ifdef DBG
                    qDebug() << "tx 3005";
#endif
                    start_sound = 1;
                    Radio_Reg_State = WAIT_STOP_CALL_REPLY;
                    memcpy( sCallReport.calledId, sRegMsgReport.radio_id , 4);// its my id
                    sCallReport.callState = eCallInit;
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                        tcp_srv.flush();
                    }
                }
                else
                {
#ifdef DBG
                    qDebug() << "to ready";
#endif
                    Radio_Reg_State = READY;
                    sCallReport.callState = eCallEnded;
                    memcpy( sCallReport.calledId, sRegMsgReport.radio_id , 4);// its my id
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                        tcp_srv.flush();
                    }
                }
            }
            break;
            // ********************************** tx ****************************************
        case WAIT_STOP_CALL_REPLY:
            if((ba_3005.at(13) == 0x41) && (ba_3005.at(14) == 0x80)) // page 94
            {
#ifdef DBG
                qDebug() << "4180";
#endif
                if(ba_3005.at(17) == 0)
                {
                    Radio_Reg_State = WAIT_STOP_CALL_HANGIN;
                    sCallStopReply.replyResult = eSucces;
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallStopReply), sizeof(sCallStopReply));
                        tcp_srv.flush();
                    }
#ifdef DBG
                    qDebug() << "WAIT_STOP_CALL_REPLY";
#endif

                }
                else
                {
                    Radio_Reg_State = WAIT_STOP_CALL_HANGIN;
                    sCallStopReply.replyResult = eFailure;
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallStopReply), sizeof(sCallStopReply));
                        tcp_srv.flush();
                    }
#ifdef DBG
                    qDebug() << "WAIT_STOP_CALL_REPLY Fail --> Ready";
#endif
                }
            }
            break;
        case WAIT_STOP_CALL_HANGIN:
            if((ba_3005.at(13) == 0x43) && (ba_3005.at(14) == 0xB8)) // page 82
            {
#ifdef DBG
                qDebug() << "43B8hang";
#endif

                if(ba_3005.at(17) == 2)
                {
                    sCallReport.callType = CallsDecoder[ba_3005.at(21)];
                    sCallReport.callState = eCallHandTime;
                    memcpy( &sCallReport.calledId, &sRegMsgReport.radio_id , 4);
                    sCallReport.receivedId[3] = ba_3005.at(23);
                    sCallReport.receivedId[2] = ba_3005.at(24);
                    sCallReport.receivedId[1] = ba_3005.at(25);
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                        tcp_srv.flush();
                    }
                    Radio_Reg_State = WAIT_STOP_CALL_ENDED;
#ifdef DBG
                    qDebug() << "WAIT_STOP_CALL_HANGIN";
#endif
                }
            }
            break;
        case WAIT_STOP_CALL_ENDED:
            if((ba_3005.at(13) == 0x43) && (ba_3005.at(14) == 0xB8)) // page 82
            {
#ifdef DBG
                qDebug() << "43B8 end";
#endif
                if(ba_3005.at(17) == 3)
                {
                    sCallReport.callType = CallsDecoder[ba_3005.at(21)];
                    sCallReport.callState = eCallEnded;
                    memcpy( sCallReport.calledId, sRegMsgReport.radio_id , 4);
                    sCallReport.receivedId[3] = ba_3005.at(23);
                    sCallReport.receivedId[2] = ba_3005.at(24);
                    sCallReport.receivedId[1] = ba_3005.at(25);
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                        tcp_srv.flush();
                    }
                    Radio_Reg_State = READY;
#ifdef DBG
                    qDebug() << "WAIT_STOP_CALL_ENDED";
#endif
                    tx_tim.stop();
                }
            }
            else if((ba_3005.at(13) == 0x44) && (ba_3005.at(14) == 0xB8)) // input call
            {
                if(ba_3005.at(17) == 1)
                {
                    sCallReport.callType = CallsDecoder[ba_3005.at(19)];
                    sCallReport.callState = eCallInit;
                    sCallReport.receivedId[1] = ba_3005.at(23);
                    sCallReport.receivedId[2] = ba_3005.at(22);
                    sCallReport.receivedId[3] = ba_3005.at(21);

                    sCallReport.calledId[1] = ba_3005.at(27);
                    sCallReport.calledId[2] = ba_3005.at(26);
                    sCallReport.calledId[3] = ba_3005.at(25);
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                        tcp_srv.flush();
                    }
#ifdef DBG
                    qDebug() << "voice rx";
#endif
                    Radio_Reg_State = RX;
                    tx_tim.stop();
                    rx_tim.start();
                }
            }
            break;
        case PTT_PRESS_AN:
#ifdef DBG
            qDebug() << "PTT Press ";
#endif
            memcpy( sCallReport.calledId, sRegMsgReport.radio_id , 4);// its my id
            memset(sCallReport.receivedId, 0, 4);
            sCallReport.callType = 1;
            sCallReport.callState = eCallInit;
            if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
            {
                tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
            }
            break;
        case PTT_RELEASE_AN:
            sCallReport.callType = 1;
            sCallReport.callState = eCallEnded;
            memcpy( sCallReport.calledId, sRegMsgReport.radio_id , 4);
            memset(sCallReport.receivedId, 0, 4);
            if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
            {
                tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
            }
            Radio_Reg_State = READY;
#ifdef DBG
            qDebug() << "WAIT_STOP_CALL_ENDED";
#endif
            break;
        case WAIT_CTRL_REPLY:
            if((ba_3005.at(13) == 0x34) && (ba_3005.at(14) == 0x88)) // page 82 RemoteMonitor Ack
            {
                //sCtrlReply.radio_id[0] = ba_3005.at(21);
                //sCtrlReply.radio_id[1] = ba_3005.at(20);
                //sCtrlReply.radio_id[2] = ba_3005.at(19);
                //sCtrlReply.radio_id[3] = ba_3005.at(18);
                if(ba_3005.at(13) == 0x00)
                {
                    sCtrlReply.result = eSucces;
                }
                else
                {
                    sCtrlReply.result = eFailure;
                }
                if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                {
                    tcp_srv.write(reinterpret_cast<char*>(&sCtrlReply), sizeof(sCtrlReply));
                    tcp_srv.flush();
                }
            }
            break;
        case WAIT_RELOAD_REPLY:
            if((ba_3005.at(13) == 0xC1) && (ba_3005.at(14) == 0x81)) // page 160
            {
                sReloadReplay.result = static_cast<unsigned char>(ba_3005.at(17));
                if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                {
                    tcp_srv.write(reinterpret_cast<char*>(&sReloadReplay), sizeof(sReloadReplay));
                    tcp_srv.flush();
                }
                if(sReloadReplay.result == eSucces)
                {
                    Radio_Reg_State = INIT_STATE;
                    sRegMsgReport.reg_unreg_state = UNREGISTRATE;
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sRegMsgReport), sizeof(sRegMsgReport));
                    }
                    reload_tim.start();
                    tcp_srv.disconnectFromHost();
                }
                else
                {
#ifdef DBG
                    Radio_Reg_State = READY;
#endif
                }
            }
            break;
        case READY:

            if((ba_3005.at(13) == 0x44) && (ba_3005.at(14) == 0xB8)) // input call
            {  //  44b8 0c00 0100 0000 00ff0000 72000000 b803
                //            17   19   21       25
                rx_tim.start();
                if(ba_3005.at(17) == 1) //  VoiceRX
                {
                    sCallReport.callType = CallsDecoder[ba_3005.at(19)];// Call Type
                    sCallReport.callState = eCallInit;

                    sCallReport.receivedId[1] = ba_3005.at(23);
                    sCallReport.receivedId[2] = ba_3005.at(22);
                    sCallReport.receivedId[3] = ba_3005.at(21);

                    if(ba_3005.at(19) == 4)// !!!!!!!!!!!!!!!!!!!
                    {
                        sCallReport.receivedId[2] = 0xFF;
#ifdef DBG
                        qDebug() << "monitor call in";
#endif
                    }
                    //else
                    //    sCallReport.receivedId[3] = ba_3005.at(21);
                    sCallReport.calledId[1] = ba_3005.at(27);
                    sCallReport.calledId[2] = ba_3005.at(26);
                    sCallReport.calledId[3] = ba_3005.at(25);

                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                        tcp_srv.flush();
                    }
#ifdef DBG
                    qDebug() << "voice rx";
#endif
                    Radio_Reg_State = RX;
                }
            }
            else if((ba_3005.at(13) == 0x33) && (ba_3005.at(14) == 0x88))// CC_RadioCheckACK
            {
                //qDebug() << ba_3005.toHex();
            }
        case RX:
            if(ba_3005.at(3) == VOICE)// page85
            {
#ifdef DBG
                qDebug() << "voice ";
#endif
                //qDebug() << ba_3005.toHex();
                receive_sound();
                rx_tim.start();
            }
            else if((ba_3005.at(3) == DATA) && (ba_3005.at(13) == 0x44)&& (ba_3005.at(14) == 0xB8)) // input call HANG_TIME
            {
                if(ba_3005.at(17) == 2)
                {
                    //sCallReport.callType = CallsDecoder[buf_rx[19]];
                    sCallReport.callState = eCallHandTime;
                    //                sCallReport.receivedId[1] = buf_rx[23];
                    //                sCallReport.receivedId[2] = buf_rx[22];
                    //                sCallReport.receivedId[3] = buf_rx[21];

                    //                sCallReport.calledId[1] = buf_rx[27];
                    //                sCallReport.calledId[2] = buf_rx[26];
                    //                sCallReport.calledId[3] = buf_rx[25];
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                        tcp_srv.flush();
                    }
#ifdef DBG
                    qDebug() << "hang time";
#endif
                    Radio_Reg_State = RX_HT;
                }
                else if (ba_3005.at(17) == 3)
                {
                    sCallReport.callState = eCallHandTime ;
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                        tcp_srv.flush();
                    }
                    sCallReport.callState = eCallEnded;
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                        tcp_srv.flush();
                    }
#ifdef DBG
                    qDebug() << "monitor call end";
#endif
                    rx_tim.stop();
                    Radio_Reg_State = READY;
                }
            }

            break;
        case RX_HT:
            if(ba_3005.at(3) == VOICE)// page85
            {
#ifdef DBG
                qDebug() << "hang time sound";
#endif
                sCallReport.callState = eCallInit;
                if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                {
                    tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                    tcp_srv.flush();
                }
                Radio_Reg_State = RX;
                rx_tim.start();
                //receive_Sound();
            }
            else if((ba_3005.at(13) == 0x44)&& (ba_3005.at(14) == 0xB8)) // input call
            {
                if(ba_3005.at(17) == 3)
                {
                    //sCallReport.callType = CallsDecoder[buf_rx[19]];

                    sCallReport.callState = eCallEnded;
                    //                sCallReport.receivedId[1] = buf_rx[23];
                    //                sCallReport.receivedId[2] = buf_rx[22];
                    //                sCallReport.receivedId[3] = buf_rx[21];

                    //                sCallReport.calledId[1] = buf_rx[27];
                    //                sCallReport.calledId[2] = buf_rx[26];
                    //                sCallReport.calledId[3] = buf_rx[25];
                    if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
                    {
                        tcp_srv.write(reinterpret_cast<char*>(&sCallReport), sizeof(sCallReport));
                        tcp_srv.flush();
                    }
#ifdef DBG
                    qDebug() << "call end";
#endif
                    rx_tim.stop();
                    Radio_Reg_State = READY;
                }
            }
            break;
        }
    }
}
