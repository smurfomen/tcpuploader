#include "responsefiles.h"

// конструктор по умолчанию для приемной стороны
ResponseFiles::ResponseFiles()
{
    //_rq = rqEmpty;
    _logger = nullptr;
}

// конструктор на базе запроса (для передающей стороны)
ResponseFiles::ResponseFiles(RemoteRq& req, Logger * logger)
{
    _rq = req;
    _logger = logger;
    _folder = req.param.toString();

    QDir dir(_folder);
    _exist = dir.exists();
    QFileInfoList list = dir.entryInfoList(QDir::Files);
    for (int i=0; i<list.size(); i++)
    {
        _files.append(BriefFileInfo(list[i]));
    }

    if (logger)
        logger->log(toString());
}


QByteArray ResponseFiles::Serialize()
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
    out << (int)_files.size();
    for (int i=0; i<_files.size(); i++)
        out << _files[i];

    return buf.buffer();
}

void ResponseFiles::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _folder;
            stream >> _exist;
            int n;
            stream >> n;
            for (int i=0; i<n; i++)
            {
                _files.append(BriefFileInfo());
                stream >> _files[i];
            }
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

QString ResponseFiles::toString()
{
    QString msg(_folder + ". Список файлов папки:\r\n");
    for (int i=0; i<_files.count(); i++)
         msg += _files[i]._name + "\r\n";
    return msg;
}
