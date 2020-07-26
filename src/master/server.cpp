#include "server.h"
#include "common/inireadwrite.h"
#include "response/response.h"
#include "response/remoterq.h"
#include "common/logger.h"
#include "ini.h"
Server::Server(ushort port, QHostAddress host, std::shared_ptr<Logger> logger, QObject *parent) : QObject(parent)
{
    Logger::LogStr(tr("Запуск"));

    server = new ServerTcp(port, host, logger);
    server->setParent(this);
    QObject::connect(server, SIGNAL(acceptError(ClientTcp*))  , this, SLOT(slotAcceptError(ClientTcp*)));
    QObject::connect(server, SIGNAL(newConnection(ClientTcp*)), this, SLOT(slotSvrNewConnection(ClientTcp*)));
    QObject::connect(server, SIGNAL(disconnected(ClientTcp*)) , this, SLOT(slotSvrDisconnected(ClientTcp*)));
    server->start();

    // FIX: каждый час очищаем корзину временных файлов (не похоже на каждый час, скорее 5 минут (60*5*1000) == 300 сек)
    startTimer(delayTrashTempFiles*1000);
}

Server::~Server()
{
    server->stop();
    delete server;
}

ClientTcp *Server::find(QString ip)
{
    // находим клиента
    ClientTcp * conn = nullptr;
    for(auto client : server->clients())
    {
        if(client->remoteIP() == ip)
        {
            conn = client;
            break;
        }
    }

    return conn;
}

bool Server::findClientAndSend(QByteArray data, QString ip)
{
    ClientTcp * finded = find(ip);
    if(!finded)
        return false;

    finded->packsend(data);
    return true;
}



void Server::slotAcceptError(ClientTcp * conn)
{
    Q_UNUSED(conn)
}

void Server::slotSvrNewConnection(ClientTcp * conn)
{
    Q_EMIT clientStateChanged(conn->remoteIP(), conn->isConnected());

    QString s(tr("Подключен клиент ") + conn->remoteIP());
    Logger::LogStr(s);
}

void Server::slotSvrDisconnected(ClientTcp * conn)
{
    Q_EMIT clientStateChanged(conn->remoteIP(), conn->isConnected());

    QString s(tr("Отключен клиент ") + conn->remoteIP());
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
