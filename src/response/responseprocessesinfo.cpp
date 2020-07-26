#include "responseprocessesinfo.h"

#ifdef Q_OS_LINUX
#include "responsetempfile.h"
#endif


#ifndef PROC_DIRECTORY
#define PROC_DIRECTORY "/proc/"
#endif

// конструктор по умолчанию для приемной стороны
ResponseProcessesInfo::ResponseProcessesInfo()
{
    _logger = nullptr;
}

ResponseProcessesInfo::ResponseProcessesInfo(RemoteRq& req, Logger * logger)
{
    _list = req.param.toString().split("/");
    GetProcessInfo();
    _rq = req;
    _logger = logger;
}

QByteArray ResponseProcessesInfo::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponse header(_rq);
    header.Serialize(out);
    // 2. Тело ответа
    out << _listOfProcessesNameFiles;
    out << _listOfProcessesLaunchDate;
    out << _listOfProcessesMemory;
    out << _listOfProcessesCurrentState;
    out << _list;

    return buf.buffer();
}

void ResponseProcessesInfo::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _listOfProcessesNameFiles;
            stream >> _listOfProcessesLaunchDate;
            stream >> _listOfProcessesMemory;
            stream >> _listOfProcessesCurrentState;
            stream >> _list;
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


QString ResponseProcessesInfo::toString()
{
    return QString ("Получена информация о процессах.");
}

ResponseProcessesInfo::~ResponseProcessesInfo()
{

}

void ResponseProcessesInfo::GetProcessInfo()
{

#ifdef Q_OS_LINUX

    QString pid, comm, state, ppid, pgrp, session, tty_nr;
    QString tpgid, flags, minflt, cminflt, majflt, cmajflt;
    QString utime, stime, cutime, cstime, priority, nice;
    QString num_threads, itrealvalue, starttime, vsize, rss;
    double rssValue = 0.0, vm = 0.0;

    for (int index = 0; index < _list.size(); index++){
        if(_list[index] != " "){
            QString fileStatusPath = PROC_DIRECTORY + _list[index] + "/stat";
            QFile procStatFile (fileStatusPath);
            if(procStatFile.open(QIODevice::ReadOnly)){

                QString tempfile = QString("%1/paServer-%2.%3.tmp").arg(QDir::tempPath()).arg(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")).arg(rand());
                QFile tempStatFile(tempfile);
                ResponseTempFile::trashTempFiles.append(tempfile);

                tempStatFile.remove();
                QString string = "ps -p " + _list[index] + " -eo lstart >> " + tempfile;
                system(string.toStdString().c_str());

                tempStatFile.open(QIODevice::ReadOnly);
                QTextStream tempStream(&tempStatFile);
                tempStream.readLine();

                _listOfProcessesLaunchDate << tempStream.readLine();

                QTextStream textStream(&procStatFile);

                textStream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
                               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
                               >> utime >> stime >> cutime >> cstime >> priority >> nice
                               >> num_threads >> itrealvalue >> starttime >> vsize >> rss;
                procStatFile.close();
                switch (state[0].toLatin1()){
                    case 'R':{
                        _listOfProcessesCurrentState << "Running";
                        break;
                    }
                    case 'S':{
                        _listOfProcessesCurrentState << "Sleeping";
                        break;
                    }
                    case 'D':{
                        _listOfProcessesCurrentState << "Waiting";
                        break;
                    }
                    case 'Z':{
                        _listOfProcessesCurrentState << "Zombie";
                        break;
                    }
                    case 'T':{
                        _listOfProcessesCurrentState << "Stopped";
                        break;
                    }
                    case 't':{
                        _listOfProcessesCurrentState << "Tracing stop";
                        break;
                    }
                    case 'X':{
                        _listOfProcessesCurrentState << "Dead";
                        break;
                    }
                    default:{
                        _listOfProcessesCurrentState << "Unknown";
                    }
                }

                long upTime = utime.toULong() * sysconf(_SC_CLK_TCK);
                long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024.0;
                rssValue = rss.toLong() * page_size_kb;
                _listOfProcessesMemory << QString::number(rssValue);

            } else {
                _listOfProcessesNameFiles << " ";
                _listOfProcessesCurrentState << " ";
                _listOfProcessesMemory << " ";
                _listOfProcessesLaunchDate << " ";
            }

            QString fileCmdlinePath = PROC_DIRECTORY + _list[index] + "/cmdline";
            QFile procCmdlineFile (fileCmdlinePath);
            if(procCmdlineFile .open(QIODevice::ReadOnly)){
                QTextStream textStream(&procCmdlineFile );
                QString cmdLine= textStream.readLine();
                _listOfProcessesNameFiles << cmdLine.split(" ").last();
                procCmdlineFile.close();

            } else{
                _listOfProcessesNameFiles << " ";
            }
        } else{
            _listOfProcessesNameFiles << " ";
            _listOfProcessesCurrentState << " ";
            _listOfProcessesMemory << " ";
            _listOfProcessesLaunchDate << " ";
        }
    }
#endif

#ifdef Q_OS_WIN
    TCHAR szProcessPathName[MAX_PATH+1] = TEXT("<unknown>");
    HANDLE hProcess = nullptr;
    PROCESS_MEMORY_COUNTERS_EX ppsmemCounters;

    for (int index = 0; index < _list.size(); index++){
        QString s = _list[index];
        DWORD PID = s.toULong();
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
        if (hProcess != nullptr){
            if (GetModuleFileNameEx(hProcess, nullptr, szProcessPathName, sizeof(szProcessPathName)) !=0){

                #ifdef UNICODE
                    _listOfProcessesNameFiles.append(QString::fromWCharArray(szProcessPathName));
                #else
                    _listOfProcessesNameFiles.append(QString::fromLocal8Bit(szProcessPathName));
                #endif

            }else {
                _listOfProcessesNameFiles.append("");
            }
            if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&ppsmemCounters, sizeof(ppsmemCounters)) !=0){
                #ifdef UNICODE
                    SIZE_T memory = ppsmemCounters.WorkingSetSize/1024;
                    _listOfProcessesMemory.append(QString::number(memory));
                #else
                    _listOfProcessesNameFiles.append(QString::fromLocal8Bit(szProcessPathName));
                #endif

            }else {
                _listOfProcessesMemory.append("");
            }

            FILETIME lpCreationTime;
            FILETIME lpExitTime;
            FILETIME lpKernelTime;
            FILETIME lpUserTime;

            SYSTEMTIME lpSystemTime;
            if (GetProcessTimes(hProcess, &lpCreationTime, &lpExitTime, &lpKernelTime, &lpUserTime) != 0){
                FileTimeToSystemTime(&lpCreationTime, &lpSystemTime);
                    QString s = QString::number(lpSystemTime.wDay) +
                            + "." + QString::number(lpSystemTime.wMonth) + "." + QString::number(lpSystemTime.wYear) +
                            + "." + QString::number(lpSystemTime.wHour) + "." + QString::number(lpSystemTime.wMinute) +
                            + "." + QString::number(lpSystemTime.wSecond);
                    _listOfProcessesLaunchDate.append(s);

            }else {
                _listOfProcessesLaunchDate.append("");
            }

            DWORD lpExitCode;

            if (GetExitCodeProcess(hProcess, &lpExitCode) != 0){
                if (lpExitCode == STILL_ACTIVE){
                    _listOfProcessesCurrentState.append("Выполняется");
                } else {
                    _listOfProcessesCurrentState.append("Завершён");
                }
            }else {
                _listOfProcessesCurrentState.append("");
            }

            CloseHandle(hProcess);
        } else {
            return;
        }
    }
    return;
    #endif
}
