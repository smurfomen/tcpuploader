#include "server.h"
#include "common/inireadwrite.h"
#include "response/response.h"
#include "response/remoterq.h"
#include "common/logger.h"
Server::Server(ushort port, QHostAddress host, std::shared_ptr<Logger> logger,QObject *parent) : QObject(parent)
{
    Logger::LogStr(tr("Запуск"));
    this->logger = logger;
    server = new ServerTcp(port, host, logger);
    server->setParent(this);
    QObject::connect(server, SIGNAL(acceptError(ClientTcp*))  , this, SLOT(slotAcceptError(ClientTcp*)));
    QObject::connect(server, SIGNAL(newConnection(ClientTcp*)), this, SLOT(slotSvrNewConnection(ClientTcp*)));
    QObject::connect(server, SIGNAL(dataready(ClientTcp*))    , this, SLOT(slotSvrDataready(ClientTcp*)));
    QObject::connect(server, SIGNAL(disconnected(ClientTcp*)) , this, SLOT(slotSvrDisconnected(ClientTcp*)));
    server->start();

    // FIX: каждый час очищаем корзину временных файлов (не похоже на каждый час, скорее 5 минут (60*5*1000) == 300 сек)
    startTimer(delayTrashTempFiles*1000);
}

Server::~Server()
{
    delete server;
}

void Server::slotAcceptError(ClientTcp * conn)
{
    Q_UNUSED(conn)
}

void Server::slotSvrNewConnection(ClientTcp * conn)
{
    QString s(tr("Подключен клиент ") + conn->name());
    Logger::LogStr(s);
}

void Server::slotSvrDataready(ClientTcp * conn)
{
    QString s("Принят запрос от клиента " + conn->name());
    Logger::LogStr(s);

    QBuffer buf;
    buf.setData(conn->data(), conn->length());
    buf.open(QIODevice::ReadOnly);
    QDataStream stream(&buf);

    // WARNING: нельзя создавать на стеке
    std::shared_ptr<RemoteRq> rq = std::shared_ptr<RemoteRq>(new RemoteRq());
    rq->Deserialize(stream);

    rq->setsrc(conn->socket()->peerAddress());
    rq->setdst(conn->socket()->localAddress());

    s = QString("Обработка запроса %1, src=%2, dst=%3").arg(RemoteRq::getRqName(rq->Rq())).arg(rq->getsrc().toString()).arg(rq->getdst().toString());
    if (rq->Rq() != rqRead)
        Logger::LogStr(s);
    else
    {
        QByteArray data = FactoryResponse().fromRequest(rq)->Serialize();
        conn->packsendExt(data);
    }
}

void Server::slotSvrDisconnected(ClientTcp * conn)
{
    QString s(tr("Отключен клиент ") + conn->name());
    Logger::LogStr(s);

    // Если отключён родитель ищем и отключаем потомка
    foreach (ClientTcp* clientTemp, _listOfRemoteConn) {
        if ( static_cast<ClientTcp*>(clientTemp->userPtr(1)) == conn){
            clientTemp->stop();
            _listOfRemoteConn.removeOne(clientTemp);
            delete clientTemp;
            break;
        }
    }
}

void Server::nextConnected(ClientTcp * conn)
{
    RemoteRq * rq = static_cast<RemoteRq *>(conn->userPtr(0));
    if (rq == nullptr) return;
    QByteArray data = rq->Serialize();
    conn->packsendExt(data);
}

void Server::nextdisconnected(ClientTcp * conn)
{
    RemoteRq * rq = static_cast<RemoteRq *>(conn->userPtr(0));
    if (rq == nullptr) return;
    ClientTcp* parent = static_cast<ClientTcp*>(conn->userPtr(1));
    ResponseError responce(*rq, Disconnect, logger.get());
    QByteArray data = responce.Serialize();
    parent->packsendExt(data);

    if (rq)
    {
        delete rq;
        conn->setUserPtr(0,nullptr);
    }
}

void Server::nexterror(ClientTcp * conn)
{
    RemoteRq * rq = static_cast<RemoteRq *>(conn->userPtr(0));
    if (rq == nullptr) return;
    ClientTcp* parent = static_cast<ClientTcp*>(conn->userPtr(1));
    ResponseError responce(*rq, Timeout, logger.get());
    responce.setErrorText( conn->socket()->errorString());
    QByteArray data = responce.Serialize();
    parent->packsendExt(data);
}

void Server::nextdataready(ClientTcp * conn)
{
    ClientTcp* parent = static_cast<ClientTcp*>(conn->userPtr(1));
    parent->send(conn->rawData(), conn->rawLength());
}

void Server::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    while (ResponseTempFile::trashTempFiles.length() > 0)
    {
        QFileInfo info(ResponseTempFile::trashTempFiles[0]);
        if (static_cast<int>(QDateTime::currentDateTime().toTime_t() - info.lastRead().toTime_t()) > delayTrashTempFiles)
        {
            QFile(info.filePath()).remove();
            ResponseTempFile::trashTempFiles.removeAt(0);
        }
        else
            break;
    }
}
