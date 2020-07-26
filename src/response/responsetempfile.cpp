#include "responsetempfile.h"
#include <sys/stat.h>

QStringList ResponseTempFile::trashTempFiles;               // корзина имен временных файлов, созданных за время работы

// конструктор по умолчанию для приемной стороны
ResponseTempFile::ResponseTempFile()
{
    _logger = nullptr;
}


// конструктор на базе запроса (для передающей стороны)
// ПРОБЛЕМА: временный файл, создаваемый QTemporaryFile, уничтожается после уничтожения ResponceTempFile
//           поэтому, надо создавать файл самому в папке временных файлов
ResponseTempFile::ResponseTempFile(RemoteRq& req, Logger * logger)
{

    _success = false;
    _rq = req;
    _logger = logger;

    QString s = req.param.toString();
    _filesrc = req.param.toString();

    // создаем временный файл paServer-ГГГГММДД ЧЧММСС.***.tmp, чтобы потом проще удалить
    QString tempfile = QString("%1/paServer-%2.%3.tmp").arg(QDir::tempPath()).arg(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")).arg(rand());
    QFile temp(tempfile);
    if (temp.open(QIODevice::WriteOnly))
    {
        QFileInfo  info(_filesrc);
        QFile file(_filesrc);
        _exist = file.exists() && info.isFile();
        if (_exist && file.open(QIODevice::ReadOnly)){
            QByteArray data = file.readAll();
            temp.write(data);
            _success = true;
        } else{
            _success = false;
        }
        temp.close();

        trashTempFiles.append(_filetemp = temp.fileName());
    }

    Logger::LogStr(toString());

}


QByteArray ResponseTempFile::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponse header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _filesrc;
    out << _exist;
    out << _filetemp;
    out << _success;

    return buf.buffer();
}

void ResponseTempFile::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _filesrc;
            stream >> _exist;
            stream >> _filetemp;
            stream >> _success;
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

QString ResponseTempFile::toString()
{
    return QString("Создан временный файл %1, копия файла %2").arg(_filetemp).arg(_filesrc);
}


