#ifndef CLIENTUPLOADER_H
#define CLIENTUPLOADER_H

#include "common/logger.h"
#include <QObject>
#include <memory>
#include "common/clienttcp.h"
#include "response/response.h"
class ClientUploader : public QObject
{

    Q_OBJECT
public:
    ClientUploader(QString settings, std::shared_ptr<Logger> logger);

public Q_SLOTS:
    void cltDataready        (ClientTcp *);

private:
    QString distFolderForFiles = "/home/vladimir/upload";

    QString settings;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<ClientTcp> client;
};

#endif // CLIENTUPLOADER_H
