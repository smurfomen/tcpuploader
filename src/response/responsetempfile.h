#ifndef RESPONSETEMPFILE_H
#define RESPONSETEMPFILE_H

#include <QTemporaryFile>
#include "remoterq.h"
#include "headerresponse.h"
#include "common/brieffileinfo.h"
#include "abstractresponse.h"

class ResponseTempFile : public AbstractResponse
{
public:
    ResponseTempFile();                                     // конструктор по умолчанию для приемной стороны
    ResponseTempFile(RemoteRq& req, Logger * logger=nullptr);// конструктор на базе запроса (для передающей стороны)

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);
    QString toString();
    bool exist() { return _exist && _success; }
    QString& name() { return _filetemp; }
    QString& nameSrc() { return _filesrc; }

    static QStringList trashTempFiles;                      // список временных файлов, созданных за время работы

protected:
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    QString         _filesrc;                               // оригинальный файл
    bool            _exist;                                 // папка существует
    bool            _success;
    QString         _filetemp;                              // временная копия
};

#endif // RESPONSETEMPFILE_H
