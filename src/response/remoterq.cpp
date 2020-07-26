#include "remoterq.h"

const quint32 RemoteRq::streamHeader = 0x48454144;                 // заголовок HEAD
const quint16 RemoteRq::paServerVersion = 1;                       // версия paServer

QHostAddress RemoteRq::localaddress("127.0.0.1");
QHostAddress RemoteRq::remoteaddress("127.0.0.1");

RemoteRq::RemoteRq()
{
    rq = rqEmpty;
    header  = RemoteRq::streamHeader;
    version = paServerVersion;

    src = localaddress;
    dst = remoteaddress;

    remotePath = "";
}

RemoteRq::RemoteRq(RemoteRqType req, QString& fullpath)
{
    rq = req;
    fullPath = fullpath;

    header  = RemoteRq::streamHeader;
    version = paServerVersion;

    src = localaddress;
    dst = remoteaddress;

    // разделяем удаленный путь
    QString host;
    ParseNestedIp(fullPath, host, remotePath);
}

QByteArray RemoteRq::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream stream (&buf);

    stream << header;
    stream << version;

    stream << (int)rq;
    stream << src;
    stream << dst;
    stream << fullPath;
    stream << remotePath;
    stream << param;
    stream << param2;
    stream << param3;
    stream << param4;
    stream << reserv1;
    stream << reserv2;
    stream << reserv3;

    return buf.buffer();
}


void RemoteRq::Deserialize (QDataStream &stream)
{
    stream >> header;
    stream >> version;
    int rqt;
    stream >> rqt;
    rq = static_cast<RemoteRqType>(rqt);
    stream >> src;
    stream >> dst;
    stream >> fullPath;
    stream >> remotePath;
    stream >> param;
    stream >> param2;
    stream >> param3;
    stream >> param4;
    stream >> reserv1;
    stream >> reserv2;
    stream >> reserv3;
}


// получить имя запроса
QString RemoteRq::getRqName(RemoteRqType type)
{
    QString ret = "Не идентифицирован запрос #" + QString::number(type);
    switch (type)
    {
    case rqAbout       : ret = "rqAbout";  break;
    case rqDirs        : ret = "rqDirs"; break;
    case rqFileInfo    : ret = "rqFileInfo"; break;
    case rqFilesInfo   : ret = "rqFilesInfo"; break;
    case rqDrives      : ret = "rqDrives"; break;
    case rqProcesses   : ret = "rqProcesses"; break;
    case rqProcesseInfo: ret = "rqProcesseInfo"; break;
    case rqTempFile    : ret = "rqTempFile"; break;
    case rqTempFilesZip: ret = "rqTempFilesZip"; break;
    case rqTempDirZip  : ret = "rqTempDirZip"; break;
    case rqDeleteTemp  : ret = "rqDeleteTemp"; break;
    case rqRead        : ret = "rqRead"; break;
    case rqEnd         : ret = "rqEnd"; break;
    case rqScreenshot  : ret = "rqScreenshot"; break;
    default:                            break;
    }
    return ret;
}


QString RemoteRq::toString()
{
    switch(rq)
    {
        case rqEmpty       : return "rqEmpty"                                                                                  ;
        case rqAbout       : return "Запрос информации о версии сервиса и хосте"                                               ;
        case rqDirs        : return "Запрос списка каталогов"                                                                  ;
        case rqFileInfo    : return "Запрос информации о файле"                                                                ;
        case rqFilesInfo   : return "Запрос информации о всех файлах каталога"                                                 ;
        case rqDrives      : return "Запрос списка логических устройств"                                                       ;
        case rqProcesses   : return "Запрос списка процессов"                                                                  ;
        case rqProcesseInfo: return "Запрос информации о процессе"                                                             ;
        case rqTempFile    : return "Запрос временной копии файла для копирования"                                             ;
        case rqTempFilesZip: return "Запрос архивирования указанных файлов во временом файле"                                  ;
        case rqTempDirZip  : return "Запрос архивирования каталога во временом файле"                                          ;
        case rqDeleteTemp  : return "Запрос удаления всех временных файлов"                                                    ;
        case rqRead        : return "Запрос чтения блока данных файла"                                                         ;
        case rqScreenshot  : return "Запрос на скриншот экрана";
        default: return "???";
    }
}

// разбор вложенного пути, например, путь 192.168.0.100:8080/10.52.19.31:8080/192.168.1.11:8080 разбивается на 2 лексемы:
// - 192.168.0.100:8080
// - 10.52.19.31:8080/192.168.1.11:8080
bool RemoteRq::ParseNestedIp(QString& ipportpath, QString& root, QString& path)
{
    // FIX: судя из кейсов использования - возвращать ничего не нужно, либо нужно возвращать хотя бы результат мэтчинга
    bool ret = true;
    root = ipportpath;
    path.clear();
    QRegularExpressionMatch match = QRegularExpression("\\A([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3}):[0-9]{1,5}/\\b").match(ipportpath);

    if (match.hasMatch())
    {
        int rootlength = match.captured().length();
        root = match.captured().left(rootlength-1);
        path = ipportpath.mid(rootlength);
    }
    return ret;
}
