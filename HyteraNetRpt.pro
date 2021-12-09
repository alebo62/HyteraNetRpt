QT -= gui
CONFIG += c++11 console network
CONFIG -= app_bundle
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        checksum.cpp \
        main.cpp \
        msg_init.cpp \
        rcp.cpp \
        tcp.cpp \
        tcp_srv.cpp \
        timers.cpp \
        udp_rad.cpp \
        udp_srv.cpp

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
target.path = /home/pi
HEADERS += \
    enums.h \
    messages.h \
    tcp.h \
    variables.h

DISTFILES += \
    readme
