#include "responseprntscr.h"
#include "responsetempfile.h"

// конструктор по умолчанию для приемной стороны
ResponsePrntScr::ResponsePrntScr()
{
    //_rq = rqEmpty;
    _logger = nullptr;
}


// конструктор на базе запроса (для передающей стороны)
// ПРОБЛЕМА: временный файл, создаваемый QTemporaryFile, уничтожается после уничтожения ResponceTempFile
//           поэтому, надо создавать файл самому в папке временных файлов
ResponsePrntScr::ResponsePrntScr(RemoteRq& req, Logger * logger)
{
    _rq = req;
    _logger = logger;

    // создаем временный файл screenshot-ГГГГММДД ЧЧММСС.***.jpeg, чтобы потом проще удалить
    QString tempfile = QString("%1/screenshot-%2.%3.png").arg(QDir::tempPath()).arg(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")).arg(rand());
    QFile temp(tempfile);
    if (temp.open(QIODevice::WriteOnly))
    {
        QPixmap pic =  grabScreens();
       _exist = pic.save(tempfile, "PNG");
        temp.close();
    }

    ResponseTempFile::trashTempFiles.append(_fileScreenshot = tempfile);

    Logger::LogStr(toString());
}

QPixmap ResponsePrntScr::grabScreens()
{
    auto screens = QGuiApplication::screens();
    QList<QPixmap> scrs;
    int w = 0, h = 0, p = 0;
    foreach (auto scr, screens) {
      QPixmap pix = scr->grabWindow(0);
      w += pix.width();
      if (h < pix.height()) h = pix.height();
      scrs << pix;
    }
    QPixmap final(w, h);
    QPainter painter(&final);
    final.fill(Qt::black);
    foreach (auto scr, scrs) {
      painter.drawPixmap(QPoint(p, 0), scr);
      p += scr.width();
    }
    return final;
}

QByteArray ResponsePrntScr::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponse header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _exist;
    out << _fileScreenshot;

    return buf.buffer();
}

void ResponsePrntScr::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _exist;
            stream >> _fileScreenshot;
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

QString ResponsePrntScr::toString()
{
    return QString("Скриншот создан. Передаём Имя созданного файла " + _fileScreenshot);
}


