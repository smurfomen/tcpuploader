#ifndef RESPONSEPROCESSESINFO_H
#define RESPONSEPROCESSESINFO_H

#include "remoterq.h"
#include "headerresponse.h"
#include "abstractresponse.h"

#include <unistd.h>
#include <stdio.h>
#include <QStringList>
#include <QVector>
#include <QString>
#include <QTextStream>

class ResponseProcessesInfo : public AbstractResponse
{
public:

    ResponseProcessesInfo();
    ResponseProcessesInfo(RemoteRq& req, Logger * logger=nullptr);
    ~ResponseProcessesInfo();

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);

    QString toString();

    QStringList     listOfProcessesNameFiles() { return _listOfProcessesNameFiles; }
    QStringList     listOfProcessesLaunchDate() { return _listOfProcessesLaunchDate; }
    QStringList     listOfProcessesMemory() { return _listOfProcessesMemory; }
    QStringList     listOfProcessesCurrentState() { return _listOfProcessesCurrentState; }
    QStringList     listOfPIDsToPopulate() { return _list; }

private:
    void GetProcessInfo();


protected:
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    QStringList _listOfProcessesNameFiles;
    QStringList _listOfProcessesLaunchDate;
    QStringList _listOfProcessesProcUsage;
    QStringList _listOfProcessesMemory;
    QStringList _listOfProcessesCurrentState;

    QStringList     _list;                                  // список процессов(PID) для которых нужно получить информацию
};

#endif // RESPONSEPROCESSESINFO_H
