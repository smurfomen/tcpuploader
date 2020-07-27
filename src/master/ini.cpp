#include "ini.h"

std::shared_ptr<Logger> logger = nullptr;              // текстовый логгер
std::shared_ptr<Server> server = nullptr;
QStringList selectedCients;
QString destPath = "/home/pcuser";
#include "common/inireadwrite.h"
QString LOG_DIR = "log/paserver/paserver.log";
namespace ini {
    void init(QString inipath)
    {
        if(QFile(inipath).exists())
        {
            // читаем настройки из ini-файла
            IniReadWrite rdr(inipath);
            int tempPort = 0;
            if(rdr.GetInt("PORT", tempPort))
                port = static_cast<quint16>(tempPort);

            QString temp;
            if(rdr.GetText("LOGDIR",temp))
                LOG_DIR = temp;

            if(rdr.GetText("DESTPATH", temp))
                destPath = temp;
        }

        logger = std::shared_ptr<Logger>(new Logger(LOG_DIR, true, true));
        Logger::SetLoger(logger.get());
        server = std::shared_ptr<Server>(new Server(ini::port, QHostAddress::Any, logger));
    }

    quint16 port = 28080;
}
