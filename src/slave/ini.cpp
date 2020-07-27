#include "ini.h"

std::shared_ptr<Logger> logger = nullptr;              // текстовый логгер
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

            QString tempLogdir;
            if(rdr.GetText("LOGDIR",tempLogdir))
                LOG_DIR = tempLogdir;
        }

        logger = std::shared_ptr<Logger>(new Logger(LOG_DIR, true, true));
        Logger::SetLoger(logger.get());
    }

    quint16 port = 28080;
}
