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

private slots:
    // уведомления сервера
    void slotAcceptError      (ClientTcp *);
    void slotSvrNewConnection (ClientTcp *);
    void slotSvrDataready     (ClientTcp *);
    void slotSvrDisconnected  (ClientTcp *);

    // уведомления клиентов рекурсивных подключений
    void nextConnected        (ClientTcp *);
    void nextdisconnected     (ClientTcp *);
    void nexterror            (ClientTcp *);
    void nextdataready        (ClientTcp *);

private:
    const int delayTrashTempFiles = 60 * 5;                 // устаревание временных файлов, сек

    ServerTcp * server {nullptr};
    QList <ClientTcp*> _listOfRemoteConn;

    std::shared_ptr<Logger> logger;
    virtual void timerEvent(QTimerEvent *event);

    QString section = "paServer";
};

#endif // SERVER_H
