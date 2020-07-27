#include "responseread.h"

// конструктор по умолчанию для приемной стороны
ResponseRead::ResponseRead()
{
    _logger = nullptr;
}

// конструктор на базе запроса
ResponseRead::ResponseRead(RemoteRq& req, Logger * logger)
{
    _rq = req;
    _logger = logger;

    _srcfilepath = req.param.toString();
    _offset      = (quint64)req.param2.toULongLong();
    _length      = req.param3.toInt();
    _dstfilepath = req.param4.toString();

    QFile file(_srcfilepath);
    _exist = file.exists();
    _eof = true;
    if (_exist && file.open(QIODevice::ReadOnly))
    {
        QFileInfo info(_srcfilepath);
        _fileInfo.fill(info);
        file.seek(_offset);
        _data = file.read(_length);
        _length = _data.length();
        _eof = file.atEnd();
        file.close();
    }

    Logger::LogStr(toString());
}

QByteArray ResponseRead::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponse header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _dstfilepath;
    out << _srcfilepath;
    out << _exist;
    out << _eof;
    out << _length;
    out << _offset;
    out << _fileInfo;
    out << _data;
    return buf.buffer();
}


void ResponseRead::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _dstfilepath;
            stream >> _srcfilepath;
            stream >> _exist;
            stream >> _eof;
            stream >> _length;
            stream >> _offset;
            stream >> _fileInfo;
            stream >> _data;
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


QString ResponseRead::toString()
{
    return _logger ? QString("Файл %1. Запрос блока длиной %2 начиная с позиции %3").arg(_fileInfo._name).arg(_length).arg(_offset) :
                    //QString("Файл %1. Принят блок данных длиной %2 начиная с позиции %3").arg(_fileInfo._name).arg(_length).arg(_offset);
                     //QString("Принято %1 байт начиная с позиции %2").arg(_length).arg(_offset);
                     QString("%1 / %2 байт").arg(_offset + _length).arg(_fileInfo.length());
}

#include <QProcess>
void ResponseRead::Handle(){
    // создаем каталог куда нужно положить файлик если его нет
    QDir d = QFileInfo(dstfilepath()).absoluteDir();
    if(!d.exists())
        d.mkpath(d.path());


    // создаем файлик и перебрасываем туда все данные
    QFile f (dstfilepath());
    if(f.open(QIODevice::WriteOnly))
    {
        f.write(_data.constData(), length());
        f.close();
    }

#ifdef Q_OS_LINUX
    QString absPath = QFileInfo(f).absoluteFilePath();

    QProcess proc;

    proc.start("chgrp pa "+ d.absolutePath());
    proc.waitForFinished();

    proc.start("chown vladimir "+ d.absolutePath());
    proc.waitForFinished();

    proc.start("chgrp pa "+ absPath);
    proc.waitForFinished();

    proc.start("chown vladimir "+ absPath);
    proc.waitForFinished();

    if(_fileInfo._attrib.contains("E"))
    {
        proc.start("chmod g+x "+ absPath);
        proc.waitForFinished();
        proc.start("chmod u+x "+ absPath);
        proc.waitForFinished();

    }

    if(_fileInfo._attrib.contains("W"))
    {
        proc.start("chmod g+w " + absPath);
        proc.waitForFinished();
        proc.start("chmod u+w "+ absPath);
        proc.waitForFinished();

    }

    if(_fileInfo._attrib.contains("R"))
    {
        proc.start("chmod g+r " + absPath);
        proc.waitForFinished();
        proc.start("chmod u+r "+ absPath);
        proc.waitForFinished();
    }
#endif
}
