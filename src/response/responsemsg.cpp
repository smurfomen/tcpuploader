#include "responsemsg.h"
ResponseMsg::ResponseMsg(RemoteRq &req, Logger *logger, int oldPort, int newPort)
{
    _rq = req;
    _logger = logger;
    _msg = "На сервере изменилась конфигурация, поменялся порт c " + QString::number(oldPort) + " на " + QString::number(newPort);
}

ResponseMsg::ResponseMsg()
{
    _logger = nullptr;
}

QByteArray ResponseMsg::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponse header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _msg;

    return buf.buffer();
}

void ResponseMsg::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _msg;
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

QString ResponseMsg::getMsg (){
    return this->_msg;
}
