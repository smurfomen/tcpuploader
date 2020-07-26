#ifndef RESPONSEABOUT_H
#define RESPONSEABOUT_H

#include "remoterq.h"
#include "headerresponse.h"
#include "common/brieffileinfo.h"
#include "abstractresponse.h"

class ResponseAbout : public AbstractResponse
{
public:
    ResponseAbout();                                        // конструктор по умолчанию для приемной стороны
    ResponseAbout(RemoteRq& req, Logger * logger=nullptr);  // конструктор на базе запроса

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);

private:
    QString toString();

    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    // сериализуемая часть
    QString         fileName;                               // - исполняемый файл сервера
    BriefFileInfo   fileInfo;                               // - информация об исполняемом файле
    QString         hostName;                               // - имя ПК
    QString         cpu;                                    // - тип процессора
    QString         kernel;                                 // - тип ядра ОС
    QString         osversion;                              // - версия ядра ОС
    QString         userName;                               // - учетная запись (системная)
    QVariant        reserv3;
    QVariant        reserv4;

};

#endif // RESPONSEABOUT_H
