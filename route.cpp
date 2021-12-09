#include <QProcess>
#include <QThread>
#include <QDebug>
#include "tcp.h"

static QProcess proc;
static QProcess proc1;
static QProcess proc2;
static QStringList sl;
static bool need_wait_usb01 = true;
static QString output;

class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};
// 02c00081ec5b1abb  nanopi
//94007868060c443f03ce  orangepi

void route_rpi()
{
    int route = 0;
    Sleeper::sleep(5);
    QStringList sl;
    do
    {
        Sleeper::sleep(3);
        proc.start("cat /proc/cpuinfo");
        proc.waitForFinished();
        sl = ((QString)proc.readAllStandardOutput()).split('\n');
    }
    while(sl[sl.size() - 3].contains(serial_num) == false);            //  rpi  144  white box

#ifdef DBG
    qDebug() << "usb and serial ok";
#endif
    while(!route)
    {
        Sleeper::sleep(3);
        proc.start("sudo route -n");
        proc.waitForFinished();
        sl = ((QString)proc.readAllStandardOutput()).split('\n');
        for(int i = 0; i < sl.size(); i++)
        {
#ifdef NANOPI_H3
//            if(sl[i].contains("192.168.10.1") && sl[i].endsWith("enx00000a0000df"))
//            {
//                proc.start("sudo route del -net 0.0.0.0 gw 192.168.0.1");
//                proc.waitForFinished();
//                //            proc.start("sudo route del -net 0.0.0.0 gw 192.168.10.1");
//                //            proc.waitForFinished();
//                proc.start("sudo route add -net 10.0.0.0/8 gw 192.168.10.1 enx00000a0000df");
//                proc.waitForFinished();
//            }
            if(sl[i].contains("192.168.10.1") && sl[i].contains("enx00000"))
            {
//                proc.start("sudo route del -net 0.0.0.0 gw 192.168.1.1");
//                proc.waitForFinished();
                //proc.start("sudo route add -net default/32 gw 192.168.0.0 eth0");
                //proc.waitForFinished();
                //            proc.start("sudo route del -net 0.0.0.0 gw 192.168.10.1");
                //            proc.waitForFinished();
                proc.start("sudo route add -net 10.0.0.0/8 gw 192.168.10.1 enx00000a00ff01");
                proc.waitForFinished();
                route = 1;
            }
            else if(sl[i].contains("192.168.10.1") && sl[i].contains("usb0"))
            {
//                proc.start("sudo route del -net 0.0.0.0 gw 192.168.1.1");
//                proc.waitForFinished();
//                proc.start("sudo route del -net 0.0.0.0 gw 192.168.10.1");
//                proc.waitForFinished();
                //            proc.start("sudo route del -net 0.0.0.0 gw 192.168.10.1");
                //            proc.waitForFinished();
                proc.start("sudo route add -net 10.0.0.0/8 gw 192.168.10.1 usb0");
                proc.waitForFinished();
                route = 1;
            }
#else
            if(sl[i].contains("192.168.10.0") && sl[i].endsWith("eth1"))//enx00000c000034"))
            {
                proc.start("sudo route add -net 10.0.0.0/8 gw 192.168.10.1 eth1");
                proc.waitForFinished();
                route = 1;
            }
#endif
        }
    }
#ifdef DBG
    qDebug() << "end route";
#endif
}
