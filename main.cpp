#include <QCoreApplication>
#include "tcp.h"
//#include <string>
#include <QSettings>
extern QString hadrr;
extern QString host;
extern quint8 chanNum;

QString str;
quint32 virt_hadrr;
static quint32 radio_id;



int main(int argc, char *argv[])
{
    if(argc >= 2)
    {
        hadrr = QString(argv[1]);
//        radio_id =  QString(argv[2]).toUInt();

//        str.setNum (radio_id >> 16);
//        host.append(str).append(".");
//        str.setNum ((radio_id >> 8) & 0xFF);
//        host.append(str).append(".");
//        str.setNum (radio_id & 0xFF);
//        host.append(str);
    }
    if(argc >= 3)
    {
        chanNum =  QString(argv[2]).toUInt();
//        virt_hadrr =  QString(argv[3]).toUInt();

//        str.setNum (radio_id >> 16);
//        host.append(str).append(".");
//        str.setNum ((radio_id >> 8) & 0xFF);
//        host.append(str).append(".");
//        str.setNum (radio_id & 0xFF);
//        host.append(str);
    }
    QCoreApplication a(argc, argv);
    TCP tcp;
    return a.exec();
}
