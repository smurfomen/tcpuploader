#ifndef INI_H
#define INI_H
#include <QString>
#include "common/logger.h"
#include <memory>
extern std::shared_ptr<Logger> logger;              // текстовый логгер
namespace ini {

void init(QString inipath = "tcpuploader.ini");

extern quint16 port;

}
#endif // INI_H
