#ifndef RESPONSEFILEINFO_H
#define RESPONSEFILEINFO_H

#include "remoterq.h"
#include "headerresponse.h"
#include "common/brieffileinfo.h"
#include "abstractresponse.h"


class ResponseFileInfo : public AbstractResponse
{
public:
    ResponseFileInfo();                                     // конструктор по умолчанию для приемной стороны
    ResponseFileInfo(RemoteRq& req, Logger * logger=nullptr);// конструктор на базе запроса

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);

private:
    bool isDir() { return _dir; }
    QString toString();
    BriefFileInfo fileInfo() { return _fileInfo; }

    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;
    QString         _filepath;                              // искомый файл

    // сериализуемая часть
    bool            _exist;                                 // файл существует
    bool            _dir;                                   // каталог
    BriefFileInfo   _fileInfo;                              // информация о файле
};

#endif // RESPONSEFILEINFO_H
