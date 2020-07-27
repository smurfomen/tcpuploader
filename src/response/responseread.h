#ifndef RESPONSEREAD_H
#define RESPONSEREAD_H

#include "remoterq.h"
#include "headerresponse.h"
#include "common/brieffileinfo.h"
#include "abstractresponse.h"

class ResponseRead : public AbstractResponse
{
public:
    ResponseRead();                                         // конструктор по умолчанию для приемной стороны
    ResponseRead(RemoteRq& req, Logger * logger=nullptr);   // конструктор на базе запроса
    QByteArray Serialize();
    void Deserialize(QDataStream& stream);
    QString toString();

    quint64 offset() { return _offset; }
    QByteArray& data() { return _data; }
    QString dstfilepath() { return _dstfilepath; }
    QString srcfilepath() { return _srcfilepath; }
    bool isEof() { return _eof; }
    int length() { return _length; }

    void Handle();

private:
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    // сериализуемая часть
    QString         _srcfilepath;                           // копируемый файл
    QString         _dstfilepath;                           // целевой файл
    bool            _exist;                                 // файл существует
    bool            _eof;                                   // достигнут конец файла
    int             _length;                                // длина считанных данных
    quint64         _offset;
    BriefFileInfo   _fileInfo;                              // информация о файле
    QByteArray      _data;                                  // считанные данные

};

#endif // RESPONSEREAD_H
