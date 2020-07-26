#include "responseprocesses.h"

// конструктор по умолчанию для приемной стороны
ResponseProcesses::ResponseProcesses()
{
    //_rq = rqEmpty;
    _logger = nullptr;
}

ResponseProcesses::ResponseProcesses(RemoteRq& req, Logger * logger)
{

    #ifdef Q_OS_WIN
    GetProcessesList();
    #endif
    #ifdef Q_OS_LINUX
    GetProcessesList();
    #endif
    _rq = req;
    _logger = logger;

}

QByteArray ResponseProcesses::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponse header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _listProcessesNames;
    out << _listPID;
    return buf.buffer();
}

void ResponseProcesses::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _listProcessesNames;
            stream >> _listPID;
        }
        if (_rq.version >= 2)
        {

        }
    }
    else
    {
        QString msg = QString("Клиент версии %1 не поддерживает работу с сервером версии %2. Требуется обновление ПО клиента").arg(RemoteRq::streamHeader).arg(_rq.version);
        Logger::LogStr(msg);
    }
}


QString ResponseProcesses::toString()
{
    return QString ("Получен список всех процессов");
}

void ResponseProcesses::GetProcessesList()
{
    QStringList listOfPID;
    _listProcessesNames.clear();

#ifdef Q_OS_LINUX
    QDir dir(PROC_DIRECTORY);
    listOfPID = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
    QRegExp re("\\d*");
    foreach (QString s,listOfPID) {
        if(re.exactMatch(s)){
            _listPID.append(s);
            QString statusPath = PROC_DIRECTORY + s + "/status";
            QFile fileLine(statusPath);
            if(fileLine.open(QIODevice::ReadOnly)){
                QTextStream textStream(&fileLine);
                QString temp = textStream.readLine().split("\t").last();
                qDebug() << temp;
                _listProcessesNames.append(temp);
                fileLine.close();
            } else {
                _listProcessesNames.append("");
            }
        } else {
            _listPID.append(s);
            _listProcessesNames.append("");
        }
    }
#endif

#ifdef Q_OS_WIN
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    HANDLE hProcessSnap;
    HANDLE hProcess = nullptr;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;
    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(pe32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if(!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);          // clean the snapshot object
        return;
    }

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    do
    {

        dwPriorityClass = 0;

        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

        if(hProcess == nullptr)
        {
            //return nullptr;
        }
        else
        {
            HMODULE hMod;
            DWORD cbNeeded;
            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
                {
                    GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR));
                }
            dwPriorityClass = GetPriorityClass(hProcess);
            if(!dwPriorityClass)
            {
                CloseHandle(hProcess);
            } else
            {
                #if defined(UNICODE)
                    QString s = QString::fromWCharArray(pe32.szExeFile);
                    QStringList s1 = s.split(".");
                    s = s1.first();
                    _listProcessesNames.append(s);
                    listOfPID.append(QString::number(pe32.th32ProcessID));
                #else
                    _listProcessesNames.append(QString::fromLocal8Bit(szProcessName));
                    listOfPID.append(QString::number(pe32.th32ProcessID));
                #endif
            }
        }

    } while(Process32Next(hProcessSnap, &pe32));

    _listPID = listOfPID;
    CloseHandle(hProcessSnap);
#endif
}
