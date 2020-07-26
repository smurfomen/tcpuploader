#ifndef INIREADWRITE_H
#define INIREADWRITE_H

#include "logger.h"
#include <QObject>
#include <QHash>
#include <QTextCodec>
#include <utility>
#include <QMap>
enum codecs
{
    UTF8 = 0,
    Windows1251 = 1,
};

class IniReadWrite : public QObject
{
    Q_OBJECT
public:
    IniReadWrite(QString inifile, codecs codec = UTF8, QObject *parent = nullptr);
    IniReadWrite(QString inifile, Logger *logger, codecs codec = UTF8, QObject *parent = nullptr);
    ~IniReadWrite() override;

    bool GetText(QString option, QString& value, QString section = nullptr, int index = 0);// получить значение i-ой опции option в section
    bool GetInt(QString option, int    & value, QString section = nullptr, int index = 0);// получить целое значение i-ой опции option в section
    bool GetBool(QString option, bool    & value, QString section = nullptr, int index = 0);// получить догическое значение i-ой опции option в section
    bool SetField(QString option, const QString& value, const QString& section = nullptr, bool flagCreateNewOption = false);// меняет значение опции в указанной секции, flagCreateNewOption = true, создаст опцию, если её нет
    bool SetField(QString option, bool value, const QString& section = nullptr, bool flagCreateNewOption = false);// меняет значение опции в указанной секции, flagCreateNewOption = true, создаст опцию, если её нет

    // Если section не задан, то значение опции будет получено из hashUnnamedSection или записано в начало ini-файла в случае SetField

    QString getIniFile();

    bool CreateOption(const QString& section, QString option, const QString& value); // Создаёт опцию в файле и возращает успешность оперции

    QHash<QString, QStringList>* getHashUnnamedSection () { return hashUnnamedSection; }
    QHash<QString, QHash<QString,QStringList> *>* getHashOfSections(){ return hashOfSections; }

    bool readIniFile ();  // чтение опций из файла

    //Возвращает список доступных для использованя кодеков
    inline static QMap<codecs, QString> availableCodecs ()
    {
        return initMapOfCodecs();
    }

    // Задаёт кодек, который будет использован
    void setCodec(codecs codecIndex, bool reReadFlag = true); //reReadFlag = true - перечитать файл с использованием выбранного кодека

signals:

public slots:

private:
    QString inifile;                                                    // Путь к файлу конфигурации
    Logger* logger;                                                     // Логгер
    QHash<QString, QStringList>* hashUnnamedSection;                    // Список опций. которые не входят ни в одну секцию
    QHash<QString, QHash<QString,QStringList> *>* hashOfSections;       // опции
    codecs currentCodec;                                                // текущий кодек
    QMap<codecs, QString> mapOfCodecs = initMapOfCodecs();              // список кодеков

    // Так как ключи в хеше регистрозависимые, пришлось организовать регистроНЕзависимый поиск среди ключей хэша
    QHash<QString, QStringList> *getHashValue (const QString& strKey, QHash<QString, QHash<QString, QStringList> *> *hashLookup, QString &hashKey);
    // strKey - Строка, по которой будет произведён поиск, регистр не имеет значения
    // hashLookup - Где ищем
    // hashKey - по ссылке будет записан реальный ключ в хэше
    // Возащает хэш опций для заданного ключа

    static QMap<codecs, QString> initMapOfCodecs();                     //Возвращает список доступных для использованя кодеков
};

#endif // INIREADWRITE_H
