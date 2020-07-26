!isEmpty(SLAVE_TCPUPLOADER):error("tcpuploaderslave.pri already included")
SLAVE_TCPUPLOADER = 1


CONFIG(debug, debug | release) {
DESTDIR = $$PWD/debug
} else {
DESTDIR = $$PWD/release
}

HEADERS += \
    $$PWD/ini.h \
    $$PWD/server.h

SOURCES += \
    $$PWD/ini.cpp \
    $$PWD/server.cpp
