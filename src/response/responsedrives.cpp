#include "responsedrives.h"

ResponseDrives::ResponseDrives()
{
    //_rq = rqEmpty;
    _logger = nullptr;
}

ResponseDrives::ResponseDrives(RemoteRq& req, Logger * logger)
{
    _rq = req;
    _logger = logger;

    QFileInfoList filesInfo = QDir::drives();
    for (int i=0; i<filesInfo.length(); i++)
        _list.append(filesInfo[i].absoluteFilePath());

    if (logger)
        logger->log(toString());

}

QByteArray ResponseDrives::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponse header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _list;

    return buf.buffer();
}

void ResponseDrives::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
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

QString ResponseDrives::toString()
{
    return QString ("Список логических дисковых устройств:\r\n" + _list.join("\r\n"));
}

