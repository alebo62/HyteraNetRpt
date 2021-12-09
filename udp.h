#ifndef UDP_H
#define UDP_H
#include <QUdpSocket>
#include <QObject>
#include <QScopedPointer>

class UDP : public QObject
{
    Q_OBJECT
public:
    explicit UDP(QObject *parent = nullptr);
//    QUdpSocket udp1_3018;
//    QUdpSocket udp1_3017;
//    QUdpSocket udp1_30001;
//    QUdpSocket udp1_30002;
//    QUdpSocket udp1_30003;
//    QUdpSocket udp1_30004;
//    QUdpSocket udp1_30005;
//    QUdpSocket udp1_30006;
//    QUdpSocket udp1_30007;
//    QUdpSocket udp1_30008;
//    QUdpSocket udp1_30009;
//    QUdpSocket udp1_30010;
//    QUdpSocket udp1_30012;
//    QUdpSocket udp1_30014;
    QUdpSocket udp1_30015;
    QUdpSocket udp1_30016;

//signals:

private slots:
    void receive3017();
    void receive3018();
    void receive30001();
    void receive30002();
    void receive30003();
    void receive30004();
    void receive30005();
    void receive30006();
    void receive30007();
    void receive30008();
    void receive30009();
    void receive30010();
    void receive30012();
    void receive30014();
    void receive30015();
    void receive30016();

    void timSlot();
//public slots:
};

#endif // UDP_H

