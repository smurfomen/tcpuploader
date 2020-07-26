#ifndef HEADERRESPONSE_H
#define HEADERRESPONSE_H

#include "remoterq.h"

class HeaderResponse
{
public:
    HeaderResponse() { }                                    // конструктор по умолчанию
    HeaderResponse(RemoteRq& req, bool success=true);       // конструктор на базе запроса

    void Serialize  (QDataStream &stream);

    RemoteRqType Rq() { return rq; }

    void setsrc(QHostAddress a) { src = a; }                // IP источника запроса
    void setdst(QHostAddress a) { dst = a; }                // IP назначение запроса

    bool success() { return _success; }                     // все ОК
    bool error  () { return !_success; }                    // ошибка

    void setError(bool s=true) { _success = s; }

protected:
    RemoteRqType rq;                                        // тип запроса
    QHostAddress src;                                       // IP источника ответа
    QHostAddress dst;                                       // IP назначение ответа
    QString      fullPath;                                  // полный константный путь запроса, возможно рекурсивный: tcp://10.52.19.71/tcp://192.168.1.1
    QVariant     reserv1;                                   // резерв
    QVariant     reserv2;                                   // резерв
    QVariant     reserv3;                                   // резерв

    bool         _success;                                  // успех/ошибка
};

#endif // HEADERRESPONSE_H
