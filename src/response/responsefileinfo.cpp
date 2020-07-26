#include "responsefileinfo.h"

// конструктор по умолчанию для приемной стороны
ResponseFileInfo::ResponseFileInfo()
{
    //_rq = rqEmpty;
    _logger = nullptr;
}

// конструктор на базе запроса
ResponseFileInfo::ResponseFileInfo(RemoteRq& req, Logger * logger)
{
    _rq = req;
    _logger = logger;
    _filepath = req.param.toString();

    QFileInfo info(_filepath);
    _fileInfo.fill(info);
    _dir   = info.isDir();
    _exist = info.exists();

    if (logger)
        logger->log(toString());
}


QByteArray ResponseFileInfo::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponse header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _exist;
    out << _dir;
    out << _fileInfo;

    return buf.buffer();
}

void ResponseFileInfo::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _exist;
            stream >> _dir;
            stream >> _fileInfo;
        }
    }
}

QString ResponseFileInfo::toString()
{
    QString name = _fileInfo._name;
    return QString("Файл: %1\r\nРазмер: %2\r\nСоздан: %3\r\nИзменен: %4\r\nАтрибуты: %5\r\n").arg(_fileInfo._name).arg(_fileInfo._length).arg(_fileInfo._created.toString(FORMAT_DATETIME)).arg(_fileInfo._lastChanged.toString(FORMAT_DATETIME)).arg(_fileInfo._attrib);
}
