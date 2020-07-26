#include "responseabout.h"
#include <QCoreApplication>

// конструктор по умолчанию для приемной стороны
ResponseAbout::ResponseAbout()
{
    //_rq = rqEmpty;
    _logger = nullptr;
}

// конструктор фрмирует отклик
ResponseAbout::ResponseAbout(RemoteRq& req, Logger * logger)
{
    _rq = req;
    _logger = logger;

    QFileInfo info( QCoreApplication::applicationFilePath() );

    fileName = info.filePath();
    fileInfo.fill(info);
    hostName = QHostInfo::localHostName();
    cpu =       "";//    QSysInfo::currentCpuArchitecture();
    kernel =    "";//    QSysInfo::kernelType();
    osversion = "";//    QSysInfo::kernelVersion();

    QByteArray user = qgetenv("USER");
    if (user.isEmpty())
        user = qgetenv("USERNAME");
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    userName = codec->toUnicode(user);

    if (logger)
        logger->log(toString());
}

// сериализация ответа
QByteArray ResponseAbout::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponse header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << fileName;
    out << fileInfo;
    out << hostName;
    out << cpu;
    out << kernel;
    out << osversion;
    out << userName;
    out << reserv3;
    out << reserv4;
    return buf.buffer();
}


void ResponseAbout::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> fileName;
            stream >> fileInfo;
            stream >> hostName;
            stream >> cpu;
            stream >> kernel;
            stream >> osversion;
            stream >> userName;
            stream >> reserv3;
            stream >> reserv4;
        }
        if (_rq.version >= 2)
        {

        }
    }
    else
    {
        QString msg = QString("Клиент версии %1 не поддерживает работу с сервером версии %2. Требуется обновление ПО клиента").arg(RemoteRq::streamHeader).arg(_rq.version);
        Logger::LogStr(msg);
    }
}

QString ResponseAbout::toString()
{
    return QString("CPU: %1\r\nОС: %2\r\nVersion: %3\r\nХост: %4\r\nUSER: %5\r\nFile: %6\r\n").arg(cpu).arg(kernel).arg(osversion).arg(hostName).arg(userName).arg(fileName);
}
