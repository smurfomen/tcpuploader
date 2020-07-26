!isEmpty(MASTER_TCPUPLOADER):error("tcpuploadermaster.pri already included")
MASTER_TCPUPLOADER = 1

CONFIG(debug, debug | release) {
DESTDIR = $$PWD/debug
} else {
DESTDIR = $$PWD/release
}

FORMS += \
    $$PWD/connectionswidget.ui \
    $$PWD/exchangewidget.ui \
    $$PWD/mainwindow.ui

HEADERS += \
    $$PWD/connectionswidget.h \
    $$PWD/exchangewidget.h \
    $$PWD/ini.h \
    $$PWD/mainwindow.h \
    $$PWD/server.h

SOURCES += \
    $$PWD/connectionswidget.cpp \
    $$PWD/exchangewidget.cpp \
    $$PWD/ini.cpp \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/server.cpp
