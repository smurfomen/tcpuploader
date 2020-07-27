#include "clientuploader.h"
#include <QBuffer>
#include <QDataStream>
#include "response/remoterq.h"
#include "response/response.h"
#include "response/headerresponse.h"

ClientUploader::ClientUploader(QString settings, std::shared_ptr<Logger> logger)
{
    this->logger = logger;
    this->settings = settings;

    client = std::shared_ptr<ClientTcp>(new ClientTcp(settings, logger));

    QObject::connect(client.get(), SIGNAL(dataready   (ClientTcp*)), this, SLOT(cltDataready    (ClientTcp*)));
    client->start();
}


void ClientUploader::cltDataready(ClientTcp * conn)
{
    QString s("Принят запрос от клиента " + conn->name());
    Logger::LogStr(s);

    QBuffer buf;
    buf.setData(conn->data(), conn->length());
    buf.open(QIODevice::ReadOnly);
    QDataStream stream(&buf);

    HeaderResponse header;
    header.Deserialize(stream);
    header.setsrc(conn->socket()->peerAddress());           // IP адреса src/dst берем из соединения
    header.setdst(conn->socket()->localAddress());

    // если ошибка - разбор ошибки и выход
    if (header.error())
    {
        return;
    }

    if(header.Rq() == rqRead)
    {
        ResponseRead r;
        r.Deserialize(stream);
        r.Handle();
    }

}
