#ifndef RESPONSEERROR_H
#define RESPONSEERROR_H

#include "remoterq.h"
#include "headerresponse.h"
#include "common/brieffileinfo.h"
#include "abstractresponse.h"

enum ResponseErrorType
{
    Ok              = 0,
    Disconnect,                                             // ошибка соединения
    Timeout,                                                // таймаут
};

class ResponseError : public AbstractResponse
{
public:
    ResponseError();                                        // конструктор по умолчанию для приемной стороны
    ResponseError(RemoteRq& req, ResponseErrorType error, Logger * logger=nullptr);  // конструктор на базе запроса

    static QString getErrorName(RemoteRqType);              // получить имя ошибки

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);
    void setErrorText(QString s) { _errorText = s; }

private:
    QString errorName();
    QString toString();

    ResponseErrorType error() { return _error; }
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    // сериализуемая часть
    ResponseErrorType _error;
    QString         _errorText;

};

#endif // RESPONSEERROR_H
