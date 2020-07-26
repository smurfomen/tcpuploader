#ifndef RESPONSEFILES_H
#define RESPONSEFILES_H

#include "remoterq.h"
#include "headerresponse.h"
#include "common/brieffileinfo.h"
#include "abstractresponse.h"

#include <QVector>

class ResponseFiles : public AbstractResponse
{
public:
    ResponseFiles();                                        // конструктор по умолчанию для приемной стороны
    ResponseFiles(RemoteRq& req, Logger * logger);          // конструктор на базе запроса (для передающей стороны)

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);

private:
    QString toString();
    QVector <BriefFileInfo> files() { return _files; }

    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    QString         _folder;                                // папка
    bool            _exist;                                 // папка существует

    QVector <BriefFileInfo> _files;                         // информация о файлах заданного каталога
};

#endif // RESPONSEFILES_H
