#ifndef RESPONSEMSG_H
#define RESPONSEMSG_H

#include <QString>

#include "remoterq.h"
#include "headerresponse.h"
#include "abstractresponse.h"

class ResponseMsg : public AbstractResponse
{
public:
    ResponseMsg();
    ResponseMsg(RemoteRq &req, Logger *logger, int oldPort, int newPort);

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);

private:
    QString getMsg ();
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;
    QString         _msg;
};

#endif // RESPONSEMSG_H
