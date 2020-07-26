#ifndef RESPONSEPRNTSCR_H
#define RESPONSEPRNTSCR_H

#include "remoterq.h"
#include "headerresponse.h"
#include "common/brieffileinfo.h"
#include "abstractresponse.h"

#include <QTemporaryFile>
#include <QScreen>
#include <QGuiApplication>
#include <QPainter>

class ResponsePrntScr : public AbstractResponse
{
private:
    QPixmap grabScreens();

public:
    ResponsePrntScr();                                      // конструктор по умолчанию для приемной стороны
    ResponsePrntScr(RemoteRq& req, Logger * logger=nullptr);// конструктор на базе запроса (для передающей стороны)

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);
private:
    QString toString();
    bool exist() { return _exist; }
    QString& name() { return _fileScreenshot; }

    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    bool            _exist;                                 // скриншот существует
    QString         _fileScreenshot;                        // временная копия
};

#endif // RESPONSEPRNTSCR_H
