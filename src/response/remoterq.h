#ifndef REMOTERQ_H
#define REMOTERQ_H

#include <QFileInfo>
#include <QDateTime>
#include <QHostInfo>
#include <QBuffer>
#include "common/logger.h"
#include <QDataStream>
#include <QRegularExpression>
#include <QTextCodec>

#define FORMAT_DATETIME "dd.MM.yy hh:mm:ss"

// удаленный запрос
const QString remoteClientId = "";                      // вложенные подкдючения не идентифицируем, чтобы не засорять поток идентификацией
                                                        // (если данные идут достаточно быстро вслед за идентификацией, они блокируются в один пакеет
                                                        //  с телом идентификации и не распознаются как отдельные пакетные данные)

enum RemoteRqType
{
    rqEmpty     = 0,
    rqAbout     = 1,                                    // информация о версии сервиса и хосте
    rqMsg       = 2,                                    // отправка сообщений сервером
    rqDirs      = 4,                                    // запрос списка каталогов
    rqFileInfo  = 5,                                    // запрос информации о файле, в том числе версии
    rqFilesInfo = 6,                                    // запрос информации о всех файлах каталога
    rqFilesSize = 7,                                    // запрос размера заданных файлов
    rqDrives    = 8,                                    // запрос списка логических устройств
    rqProcesses = 9,                                    // запрос списка процессов
    rqProcesseInfo = 10,                                // запрос информации о процессе
    rqTempFile     = 11,                                // запрос временной копии файла для копирования
    rqTempFilesZip = 12,                                // запрос архивирования указанных файлов во временом файле
    rqTempDirZip   = 13,                                // запрос архивирования каталога во временом файле
    rqDeleteTemp   = 14,                                // запрос удаления всех временных файлов (время создания и изменения - больше суток назад)
    rqRead         = 15,                                // запрос чтения части файла
    rqEnd          = 16,
    rqScreenshot   = 17,                                // запрос на скриншот экрана
    rqError        = 18,
};


class RemoteRq
{
    friend class HeaderResponce;
    friend class ResponceAbout;
    friend class ResponceDirs;
    friend class ResponceFileInfo;
    friend class ResponceFiles;
    friend class ResponceTempFile;
    friend class ResponceRead;
    friend class ResponceDrives;
    friend class ResponceError;
    friend class ResponcePrntScr;
    friend class ResponceProcesses;
    friend class ResponceProcessesInfo;
    friend class ResponceMsg;
public:

    static QHostAddress localaddress;
    static QHostAddress remoteaddress;

    RemoteRq();
    RemoteRq(RemoteRqType req, QString& fullpath);

    static const quint32 streamHeader;                      // заголовок
    static const quint16 paServerVersion;                   // версия paServer

    static QString getRqName(RemoteRqType);                 // получить имя запроса

    // разбор вложенного пути, например, путь 192.168.0.100:8080/10.52.19.31:8080/192.168.1.11:8080 разбивается на 2 лексемы:
    // - 192.168.0.100:8080
    // - 10.52.19.31:8080/192.168.1.11:8080
    static bool ParseNestedIp(QString& ipportpath, QString& root, QString& path);

    QString getRqName() { return getRqName(rq); }
    QString& getRemotePath() { return remotePath; }

    bool isRemote() { return remotePath.length() > 0; }     // надо более строго выделить корректный удаленный путь
    void setRemote(QString& path)  { remotePath = path; }

    QByteArray Serialize();
    void Deserialize(QDataStream &stream);


    RemoteRqType Rq() { return rq; }

    void setParam (QVariant value) { param  = value; }
    void setParam2(QVariant value) { param2 = value; }
    void setParam3(QVariant value) { param3 = value; }
    void setParam4(QVariant value) { param4 = value; }


    QHostAddress getsrc() { return src; }                   // IP источника запроса
    QHostAddress getdst() { return dst; }                   // IP назначение запроса
    void setsrc(QHostAddress a) { src = a; }                // IP источника запроса
    void setdst(QHostAddress a) { dst = a; }                // IP назначение запроса
    void setFullPath(QString s) { fullPath = s;}
    void setRq(RemoteRqType rq) { this->rq = rq;}
    QString toString();

    // несериализуемая часть
    quint16 version;                                        // версия протокола

public:
    quint32 header;                                         // сигнатура сериализации

    // сериализуемая часть
    RemoteRqType rq;                                        // запрос
    QHostAddress src;                                       // IP источника запроса
    QHostAddress dst;                                       // IP назначение запроса
    QString      fullPath;                                  // полный константный путь запроса, возможно рекурсивный: 10.52.19.71:28080/192.168.1.1:28080/имя_файла
    QString      remotePath;                                // удаленный хост (возможен рекурсивный путь); если пустая строка - локальный хост
    QVariant     param;                                     // параметр
    QVariant     param2;                                    // параметр 2
    QVariant     param3;                                    // параметр 3
    QVariant     param4;                                    // параметр 4
    QVariant     reserv1;                                   // резерв
    QVariant     reserv2;                                   // резерв
    QVariant     reserv3;                                   // резерв
};

#endif // REMOTERQ_H
