#-------------------------------------------------
#
# Project created by QtCreator 2020-07-25T10:37:39
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
CONFIG += master

master {
TARGET = tcpuploadermaster
include($$PWD/master/tcpuploadermaster.pri)
}

slave {
TARGET = tcpuploaderslave
include($$PWD/slave/tcpuploaderslave.pri)
}



SOURCES += \
        common/brieffileinfo.cpp \
        common/clienttcp.cpp \
        common/inireadwrite.cpp \
        common/logger.cpp \
        common/servertcp.cpp \
        common/tcpheader.cpp \
        response/remoterq.cpp \
        response/headerresponse.cpp \
        response/responseabout.cpp \
        response/responsedirs.cpp \
        response/responsedrives.cpp \
        response/responseerror.cpp \
        response/responsefileinfo.cpp \
        response/responsefiles.cpp \
        response/responsemsg.cpp \
        response/responseprntscr.cpp \
        response/responseprocesses.cpp \
        response/responseprocessesinfo.cpp \
        response/responseread.cpp \
        response/responsetempfile.cpp \

HEADERS += \
        common/brieffileinfo.h \
        common/clienttcp.h \
        common/inireadwrite.h \
        common/logger.h \
        common/servertcp.h \
        common/tcpheader.h \
        response/remoterq.h \
        response/abstractresponse.h \
        response/headerresponse.h \
        response/response.h \
        response/responseabout.h \
        response/responsedirs.h \
        response/responsedrives.h \
        response/responseerror.h \
        response/responsefileinfo.h \
        response/responsefiles.h \
        response/responsemsg.h \
        response/responseprntscr.h \
        response/responseprocesses.h \
        response/responseprocessesinfo.h \
        response/responseread.h \
        response/responsetempfile.h \



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
