#ifndef SERVER_H
#define SERVER_H

#include "common/clienttcp.h"
#include "common/servertcp.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(ushort port, QHostAddress host, std::shared_ptr<Logger> logger, QObject *parent = nullptr);
    ~Server();

    ClientTcp *find(QString ip);

    bool findClientAndSend(QByteArray data, QString ip);

Q_SIGNALS:
    void clientStateChanged(QString ip, bool isConnected);

private slots:
    // уведомления сервера
    void slotAcceptError      (ClientTcp *);
    void slotSvrNewConnection (ClientTcp *);
    void slotSvrDisconnected  (ClientTcp *);

private:
    const int delayTrashTempFiles = 60 * 5;                 // устаревание временных файлов, сек

    ServerTcp * server {nullptr};
    QList <ClientTcp*> _listOfRemoteConn;

    virtual void timerEvent(QTimerEvent *event);

    QString section = "paServer";
};

#endif // SERVER_H
