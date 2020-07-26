#ifndef BRIEFFILEINFO_H
#define BRIEFFILEINFO_H

#include <QFileInfo>
#include <QDateTime>
#include <QHostInfo>
#include <QBuffer>
#include <QDataStream>

class BriefFileInfo
{

public:
    BriefFileInfo() { _length = 0; }
    BriefFileInfo (QFileInfo& fi);                          // конструктор
    void fill(QFileInfo& fi);
    qint64 length() { return _length; }

    friend QDataStream &operator <<(QDataStream &stream, BriefFileInfo& info)
    {
        stream << info._name;
        stream << info._lastChanged;
        stream << info._created;
        stream << info._length;
        stream << info._attrib;
        stream << info._version;
        stream << info._reserv;
        return stream;
    }

    friend QDataStream &operator >> (QDataStream &stream, BriefFileInfo& info)
    {
        stream >> info._name;
        stream >> info._lastChanged;
        stream >> info._created;
        stream >> info._length;
        stream >> info._attrib;
        stream >> info._version;
        stream >> info._reserv;
        return stream;
    }

    QString     _name;                                      // имя файла локальное
    QDateTime   _lastChanged;                               // дата изменения
    QDateTime   _created;                                   // дата создания
    quint64     _length;                                    // длина
    QString     _attrib;                                    // атрибуты
    QVariant    _reserv;
    QString     _version;                                   // Версия файла

private:
    QString getVersionString(QString fName);
};

#endif // BRIEFFILEINFO_H
