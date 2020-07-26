#ifndef RESPONSEDIRS_H
#define RESPONSEDIRS_H
#include "remoterq.h"
#include "headerresponse.h"
#include "common/brieffileinfo.h"
#include "abstractresponse.h"

#include <QDir>
#include <QStringList>

class ResponseDirs : public AbstractResponse
{
public:
    ResponseDirs();                                         // конструктор по умолчанию для приемной стороны
    ResponseDirs(RemoteRq& req, Logger * logger=nullptr);   // конструктор на базе запроса

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);

private:
    QString toString();
    RemoteRq        rq() { return _rq; }
    bool            exist() { return _exist; }
    QStringList     list() { return _list; }
    QString         folder() { return _folder; }

    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    QString         _folder;
    bool            _exist;                                 // папка существует
    QStringList     _list;                                  // список файлов заданной папки
};

#endif // RESPONSEDIRS_H
