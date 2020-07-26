#include "responsedirs.h"


// конструктор по умолчанию для приемной стороны
ResponseDirs::ResponseDirs()
{
    //_rq = rqEmpty;
    _logger = nullptr;
}

// конструктор на базе запроса
ResponseDirs::ResponseDirs(RemoteRq& req, Logger * logger)
{
    _rq = req;
    _logger = logger;

    _folder = req.param.toString();
    QDir dir(_folder);
    _exist = dir.exists();
    _list = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot|QDir::NoSymLinks);
    Logger::LogStr(toString());

}

QByteArray ResponseDirs::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponse header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _folder;
    out << _exist;
    out << _list;
    return buf.buffer();
}

void ResponseDirs::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _folder;
            stream >> _exist;
            stream >> _list;
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


QString ResponseDirs::toString()
{
    return QString (_folder + ". Список каталогов папки:\r\n" + _list.join("\r\n"));
}
