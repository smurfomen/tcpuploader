#ifndef INI_H
#define INI_H
#include <QString>
#include "common/logger.h"
#include <memory>
#include "server.h"
extern std::shared_ptr<Server> server;
extern std::shared_ptr<Logger> logger;              // текстовый логгер
extern QStringList selectedCients;
extern QString destPath;
namespace ini {

void init(QString inipath = "tcpuploader.ini");

extern quint16 port;

}
#endif // INI_H
