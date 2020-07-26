#ifndef RESPONSEDRIVES_H
#define RESPONSEDRIVES_H

#include "remoterq.h"
#include "headerresponse.h"
#include "common/brieffileinfo.h"
#include "abstractresponse.h"

#include <QDir>
#include <QStringList>

class ResponseDrives : public AbstractResponse
{
public:
    ResponseDrives();                                       // конструктор по умолчанию для приемной стороны
    ResponseDrives(RemoteRq& req, Logger * logger=nullptr); // конструктор на базе запроса

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);

private:
    QString toString();
    QStringList     list() { return _list; }

    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    QStringList     _list;                                  // список дисков
};

#endif // RESPONSEDRIVES_H
