!isEmpty(SLAVE_TCPUPLOADER):error("tcpuploaderslave.pri already included")
SLAVE_TCPUPLOADER = 1


CONFIG(debug, debug | release) {
DESTDIR = $$PWD/debug
} else {
DESTDIR = $$PWD/release
}

HEADERS += \
    $$PWD/clientuploader.h \
    $$PWD/ini.h

SOURCES += \
    $$PWD/clientuploader.cpp \
    $$PWD/ini.cpp \
    $$PWD/main.cpp
