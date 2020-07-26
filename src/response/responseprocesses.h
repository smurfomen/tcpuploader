#ifndef RESPONSEPROCESES_H
#define RESPONSEPROCESES_H

#include "remoterq.h"
#include "headerresponse.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <QStringList>
#include <QTextStream>
#include "abstractresponse.h"

#define PROC_DIRECTORY "/proc/"

class ResponseProcesses : public AbstractResponse
{
public:
    ResponseProcesses();                                         // конструктор по умолчанию для приемной стороны
    ResponseProcesses(RemoteRq& req, Logger * logger=nullptr);   // конструктор на базе запроса

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);

    QString toString();
    QStringList     listOfProcesses() { return _listProcessesNames; }
    QStringList     listOfPID() { return _listPID; }

private:
#ifdef Q_OS_WIN
    TCHAR *getProcesName(DWORD processID);
#endif

    void GetProcessesList( );

protected:
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    QStringList     _listProcessesNames;                    // список процессов

    QStringList     _listPID;                               // список идентификаторов процессов
};

#endif // RESPONSEPROCESES_H
