#include "responseerror.h"

ResponseError::ResponseError()
{
    _logger = nullptr;
    _errorText.clear();
}

ResponseError::ResponseError(RemoteRq& req, ResponseErrorType err, Logger * logger)
{
    _rq = req;
    _logger = logger;
    _error  = err;
    _errorText.clear();
}

QByteArray ResponseError::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponse header(_rq, false);
    header.Serialize(out);

    // 2. Тело ответа
    out << (int)_error;
    out << _errorText;

    return buf.buffer();
}

void ResponseError::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            int err;
            stream >> err;
            _error = (ResponseErrorType)err;
            stream >> _errorText;
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

QString ResponseError::toString()
{
    return QString("Ошибка обработки удаленного запроса: %1. %2").arg(errorName()).arg(_errorText);
}

QString ResponseError::errorName()
{
    switch (_error)
    {
        case Ok         :   return "OK!";
        case Disconnect :   return "Ошибка сетевого соединения";
        case Timeout    :   return "Таймаут";

        default         :   return "Unknown";
    }
}

