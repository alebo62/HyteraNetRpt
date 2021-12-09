#include "tcp.h"
#include "enums.h"
#include "messages.h"

extern strLocMsgType_TrgRep sLocMsgType_TrgRep;
extern quint8 std_loc_imm_req[15];
extern QByteArray ba;
extern QString dest_rrs_ip;
extern void crc(unsigned char*, int);
QString gps_addr;

float result;
/*
 *Отправляем на станцию --->
 *
    MsgHdr(0x08)
    Opcode
        0xA0 Standard Location Immediate Report
            0x01 (Request)          RequestID[4]+RadioIP[4]  --->
        0xB0 --- Emergency Location Reporting Service
            0x01 (ReportStopReq)    RequestID[4]+RadioIP[4]  --->
        0xC0 --- Triggered Location Reporting Service
            0x01 (ReportReq)        RequestID[4]+RadioIP[4] + StartTime[14] + StopTime[14] + Interval[8] --->
            0x04 (ReportStopReq)    RequestID[4]+RadioIP[4]   --->
        0xD0 --- Condition Triggered Reporting Service
            0x01 (ReportReq)        RequestID[4]+RadioIP[4] + TriggerType[1]+Distance[4]+StartTime[14] + StopTime[14] + Interval[8]+MaxInterval[8]
            0x11 (QuickGPSReq)      RequestID[4]+RadioIP[4]+CmdType[1]+Field[N]
        0xE0 --- RSSI Report Configuring Service
            0x01(RSSIReportConfReq) RequestID[4]+RadioIP[4]+ReportType[2]
    #of bytes
            0xA0 - Standard Location Immediate Service  8 - 50 - 52 - 49(31-bluetooth)
            0xB0 - Emergency Location Reporting Service 8 - 10 - 45 - 46(28-bluetooth)
            0xC0 - Triggered Location Reporting Service 44 - 10 - 56 - 8 - 10
            0xD0 - Condition Triggered Reporting Service  49 - 9+???
            0xE0 - RSSI Report Configuring Service   10
    Payload
        ...
    Checksum
    MsgEnd （0x03）


    crc(uchar* p, len)
    len = p[4] + 7  !!!!!!!!!!!!!!!!

*/
void TCP::rcv_tcpGPS()
{
    //    IronAgentMsgType  1 байт (0x02)
    //    PayloadLength     2 байта
    //    Reserved          1 byte
    gps_addr = dest_rrs_ip + QString::number(ba.at(6))+ "."+QString::number(ba.at(7))+"."+ QString::number(ba.at(8));
    switch (ba.at(4)) {   // RadioId 4 байта(5..8) +  RequestId 4 байта (9..12)
    case ImmedLocRequest: // bool UseCsbk – использовать CSBK при передаче
        memcpy(std_loc_imm_req+5, ba.data() + 9, 4); // reqid
        memcpy(std_loc_imm_req+10, ba.data() + 6, 3); // reqid
        crc(std_loc_imm_req,15);
        udpGPS_3003.writeDatagram((char*)std_loc_imm_req, 15, QHostAddress(gps_addr), GPS);
        //udpGPS_3003.flush();
        //qDebug();
        break;
    case TrigLocaRequest: // u16 interval – инт между отчетами + bool UseCsbk – исп. CSBK при передаче

        break;
    case TrigLocStopRequest: // Отсутствует

        break;
    default:
        break;
    }
    //    addr = "12." + QString::number(static_cast<quint8>(ba1.at(6))) + "." + QString::number(static_cast<quint8>(ba1.at(7))) + "." + QString::number(static_cast<quint8>(ba1.at(8))) ;

    //    if(ba1.at(4) == ImmedLocRequest )
    //    {
    //        memcpy(imm_loc_req + 5, ba1.data() + 10, 3);// copy request id
    //        udp1_4001.writeDatagram(reinterpret_cast<char*>(imm_loc_req), 11, QHostAddress(addr), 4001);
    //#ifdef DBG
    //        qDebug() << QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss ") << "ImmedLocRequest_1 for " << addr;
    //#endif
    //    }
    //    else if(ba1.at(4) == TrigLocaRequest)
    //    {
    //        memcpy(trig_loc_req + 5, ba1.data() + 10, 3);// copy request id
    //        if(ba1.at(13) == 0)
    //        {float
    //            if(static_cast<quint8>(ba1.at(14)) > 127)
    //            {
    //                trig_loc_req[1] = 13;
    //                trig_loc_req[13] = static_cast<quint8>(ba1.at(14));
    //                trig_loc_req[14] = 0x01;// copy interval
    //                udp1_4001.writeDatagram(reinterpret_cast<char*>(trig_loc_req), 15, QHostAddress(addr), 4001);
    //            }
    //            else
    //            {
    //                trig_loc_req[1] = 12;
    //                trig_loc_req[13] = static_cast<quint8>(ba1.at(14));
    //                udp1_4001.writeDatagram(reinterpret_cast<char*>(trig_loc_req), 14, QHostAddress(addr), 4001);
    //            }
    //        }
    //        else
    //        {
    //            trig_loc_req[1] = 13;
    //            trig_loc_req[13] = static_cast<quint8>(ba1.at(14));// interval  8min
    //            trig_loc_req[14] = 0x03;// copy interval
    //            udp1_4001.writeDatagram(reinterpret_cast<char*>(trig_loc_req), 15, QHostAddress(addr), 4001);
    //        }
    //#ifdef DBG
    //        qDebug() << QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss ") << "TrigLocRequest_1 for " << addr;
    //#endif
    //    }
    //    else if(ba1.at(4) == TrigLocStopRequest)
    //    {
    //        memcpy(trig_loc_stop_req + 5, ba1.data() + 10, 3);// copy requset id
    //        udp1_4001.writeDatagram(reinterpret_cast<char*>(trig_loc_stop_req), 8, QHostAddress(addr), 4001);
    //#ifdef DBG
    //        qDebug() << QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss ") << "TrigLocStopRequest_1 for " << addr;
    //#endif
    //    }

}


QByteArray ba_3003;
void TCP::rcv_udpGPS()
{
    ba_3003.resize(static_cast<int>(udpGPS_3003.pendingDatagramSize()));
    udpGPS_3003.readDatagram(ba_3003.data(), ba_3003.size());
#ifdef DBG_BA
    qDebug() << ba_3003.toHex();
#endif
    switch (ba_3003.at(1)) {
    case StdLocImmReport:
        switch (ba_3003.at(2)) {
        case 0x02:  // RequestID[4]+RadioIP[4] + Result[2]+ GPSData[40]
            sLocMsgType_TrgRep.LocMsgType = 0x02;// immediatly report
            memcpy(sLocMsgType_TrgRep.RequestId, ba_3003.data() + 5, 4);
            memcpy(&sLocMsgType_TrgRep.RadioId[1], ba_3003.data() + 10, 3);
            sLocMsgType_TrgRep.Result[3] = ba_3003.at(14);
            result = get_latitude(ba_3003.at(4) - 21);
            memcpy((char*)sLocMsgType_TrgRep.Latitude, (char*)&result , 4);
            result = get_longitude(ba_3003.at(4) - 21);
            memcpy(sLocMsgType_TrgRep.Longitude, (char*)&result, 4);
            result = get_speed(ba_3003.at(4) - 21);
            memcpy(sLocMsgType_TrgRep.Speed, (char*)&result, 4);
            result = get_direction(ba_3003.at(4) - 21);
            memcpy(sLocMsgType_TrgRep.Heading, (char*)&result, 4);
            if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
            {
                tcp_srv.write((char*)&sLocMsgType_TrgRep, sizeof(sLocMsgType_TrgRep));
                tcp_srv.flush();
            }


            //qDebug() << "gps immed 02";
            break;
        case 0x03: // RequestID[4]+RadioIP[4] + Result[2]+GPSData[40]+RSSIValue[2]
#ifdef DBG
            qDebug() << "gps immed 03";
#endif
            break;
        case 0x04:// RadioIP[4]+EmegencyType[1]+DataType[1]+Data[n]
#ifdef DBG
            qDebug() << "gps immed 04";
#endif
            break;
        }

        break;
    case EmgLocReportSrv:
        switch (ba_3003.at(2)) {
        case 0x02: //  RequestID[4]+RadioIP[4] + Result[2]

            break;
        case 0x03: // RadioIP[4] + EmegencyType[1]+GPSData[40]

            break;
        case 0x04: // RadioIP[4]+EmegencyType[1]+DataType[1]+Data[n]

            break;
        default:

            break;
        }
        break;
    case TrigLocReportSrv:
        switch (ba_3003.at(2)) {
        case 0x02: // RequestID[4](5..8)+RadioIP[4](9..12) + Result[2]

            break;
        case 0x03:  // RequestID[4](5..8)+RadioIP[4](9..12) + TimeRemaining[8](13...20) + GPSData[40](21... 60 )
            if(ba_3003.at(21) == 'A')// 0x41
                sLocMsgType_TrgRep.Result[3] = 0;
            else
                sLocMsgType_TrgRep.Result[3] = 1;
            sLocMsgType_TrgRep.LocMsgType = 0x05;// triggered report
            memcpy(sLocMsgType_TrgRep.RequestId, ba_3003.data() + 5, 4);
            memcpy(&sLocMsgType_TrgRep.RadioId[1], ba_3003.data() + 10, 3);
            result = get_latitude(ba_3003.at(4) - 21);
            memcpy((char*)sLocMsgType_TrgRep.Latitude, (char*)&result, 4);
            result = get_longitude(ba_3003.at(4) - 21);
            memcpy((char*)sLocMsgType_TrgRep.Longitude, (char*)&result , 4);
            //memcpy(sLocMsgType_TrgRep.Longitude, (char*)&result, 4);
            result = get_speed(ba_3003.at(4) - 21);
            memcpy(sLocMsgType_TrgRep.Speed, (char*)&result, 4);
            result = get_direction(ba_3003.at(4) - 21);
            memcpy(sLocMsgType_TrgRep.Heading, (char*)&result, 4);
            if(tcp_srv.state() ==  QAbstractSocket::ConnectedState)
            {
                tcp_srv.write((char*)&sLocMsgType_TrgRep, sizeof(sLocMsgType_TrgRep));
                tcp_srv.flush();
            }
            break;
        case 0x05://  RequestID[4]+RadioIP[4] + Result[2]

            break;
        default:

            break;
        }
        break;
    case CondTrigReportSrv:
        switch (ba_3003.at(1)) {
        case 0x02: //RequestID[4]+RadioIP[4] + TriggerType[1]+Result[2]

            break;
        case 0x03: // RequestID[4]+RadioIP[4] + GPSData[40]

            break;
        case 0x04:// RequestID[4]+RadioIP[4] + GPSData[40]+RSSIValue[2]

            break;
        case 0x05: // RequestID[4]+RadioIP[4]+DataType[1]+Data[n]

            break;
        case 0x12: // RequestID[4]+RadioIP[4]+ CmdType[1]+Result[2]

            break;
        default:

            break;
        }
        break;
    case RssiReportConfigSrv:
        switch (ba_3003.at(1)) {
        case 0x02: // RequestID[4]+RadioIP[4]+ReportType[2]+Result[2]

            break;
        default:

            break;
        }
        break;
    default:
        break;
    }
}

float TCP::get_latitude(quint32 x)
{
    double result = 0.0;
    quint32 lat=(ba_3003.at(x)- 0x30)*10 + (ba_3003.at(x+1) - 0x30);//  celaya chast gradusov
    quint32 y = (ba_3003.at(x+2)- 0x30)*10 + (ba_3003.at(x+3) - 0x30);//  celaya chast minut
    quint32 z = (ba_3003.at(x+5)- 0x30)*1000 + (ba_3003.at(x+6)- 0x30)*100 + (ba_3003.at(x+7)- 0x30)*10 + (ba_3003.at(x+8) - 0x30);
    double min = ((double)y + ((double)z / 10000.0)) / 60.0;
    return result = ((double)lat + min);
}

float TCP::get_longitude(quint32 x)
{
    double result = 0.0;
    quint32 lon = (ba_3003.at(x+10)- 0x30)*100 + (ba_3003.at(x+11) - 0x30)*10 + (ba_3003.at(x+12) - 0x30);//  celaya chast gradusov
    quint32 y = (ba_3003.at(x+13)- 0x30)*10 +  (ba_3003.at(x+14) - 0x30);//  celaya chast minut
    quint32 z = (ba_3003.at(x+16)- 0x30)*1000 +(ba_3003.at(x+17) - 0x30)*100 + (ba_3003.at(x+18)- 0x30)*10 + (ba_3003.at(x+19) - 0x30);
    double min = ((double)y + ((double)z / 10000.0)) / 60.0;
    return result = ((double)lon + min);
}

float TCP::get_speed(quint32 x)
{
    float result = 0.0;
    return result = ((ba_3003.at(x+20)- 0x30)*10 + (ba_3003.at(x+22) - 0x30)) / 10.0;
}

quint32 TCP::get_direction(quint32 x)
{
    quint32 result = 0;
    return result = ((ba_3003.at(x+23)- 0x30)*100 + (ba_3003.at(x+24) - 0x30)*10 + (ba_3003.at(x+25) - 0x30));
}


/*
 Это для передачи на сервер --->

    IronAgentMsgType  1 байт (0x02)
    PayloadLength     2 байта
    Reserved          1 byte

    LocationMsgType 1 байт(4)
        ImmediateLocationReport     (0x02)  --->
        TriggeredLocationAnswer     (0x04)  --->
        TriggeredLocationReport     (0x05)  --->
        TriggeredLocationStopAnswer (0x07)  --->

    RadioId 4 байта	  (5..8)
    RequestId 4 байта (9..12)
    Payload  (13..)
        int Result – результат, + LocationData – структура, описанная ниже (44)
        int Result – результат установки триггера (4)
        int Result – результат, + LocationData – структура, описанная ниже (44)
        int Result – результат установки триггера (4)


*/


