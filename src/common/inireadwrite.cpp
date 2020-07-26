#include "inireadwrite.h"
#include <QTextStream>
#include <QRegularExpression>

IniReadWrite::IniReadWrite(QString inifile, codecs codec, QObject *parent) : QObject(parent)
{
    currentCodec = codec;                                               // По умолчанию codec = UTF8
    hashOfSections = new QHash<QString,QHash<QString,QStringList>*>;
    hashUnnamedSection = new QHash<QString, QStringList>;
    logger = nullptr;
    this->inifile = std::move(inifile);
    setParent(parent);
    readIniFile();
}

IniReadWrite::IniReadWrite(QString inifile, Logger* logger, codecs codec, QObject *parent) : QObject(parent)
{
    currentCodec = codec;                                               // По умолчанию codec = UTF8
    hashOfSections = new QHash<QString,QHash<QString,QStringList>*>;
    hashUnnamedSection = new QHash<QString, QStringList>;
    this->logger = logger;
    this->inifile = std::move(inifile);
    setParent(parent);
    readIniFile();
}

//Читаем ini-файл, заданный в inifile. В процессе работы будут заполнены хэши hashUnnamedSection или hashOfSections
//в зависимости от содержимого ini-файла
bool IniReadWrite::readIniFile ()
{
    hashUnnamedSection->clear();
    hashOfSections->clear();

    bool ret = true;

    QString spath = this->inifile;
    QFileInfo fi(this->inifile);

    if (fi.isRelative())
    {
        spath = QString("%1/%2").arg(QDir::current().absolutePath(), this->inifile);   // формируем полное имя файла
    }
    inifile = spath;
    bool flagUnnamedSection = true;

    QFile file (inifile);
    if (file.open(QFile::ReadWrite))
    {
        QTextStream in (&file);
        in.codec();
        in.setCodec(mapOfCodecs.value(currentCodec).toUtf8());
        QString str;
        QString keyHash = "";
        QHash<QString,QStringList>* hash = nullptr;
        while (!(str = in.readLine().trimmed()).isNull())   // читаем построчно до упора
        {
            if(str != "")
            {
                if (str[0]==';' || str[0]=='/')                // отсекаем комментарии
                {
                    continue;
                }
            }
            if (QRegularExpression(R"((^\[.+\]\s*;.*$|^\[.+\]$))").match(str).hasMatch()) // Ищем секцию [Секция]
            {
                flagUnnamedSection = false;
                int indexTemp = str.indexOf(";");

                if(indexTemp != -1)
                {
                    str = str.mid(0, indexTemp);
                    str = str.trimmed();
                }

                if(getHashValue(str, hashOfSections, keyHash) != nullptr)
                {
                    hash = hashOfSections->value(keyHash);
                } else
                {
                    keyHash = str;
                    hash = new QHash<QString,QStringList>;
                }
            } else if (QRegularExpression(R"(^[\wа-яА-Я_\+\-\d]+(?==.*))").match(str).hasMatch())  // Ищем возможную опцию ОПЦИЯ=
            {
                QString value = QRegularExpression("(?<==).+$").match(str).captured();
                int indexTemp = value.indexOf(";");
                if(indexTemp != -1)
                {
                    if(QRegularExpression(R"(\s)").match(value.mid(indexTemp - 1, 1)).hasMatch())
                    {
                        value = value.mid(0, indexTemp);
                        value = value.trimmed();
                    }
                }
                QString name  = QRegularExpression(R"(^[\wа-яА-Я_+\-\d]+(?==.*))").match(str).captured(); // Нашли опцию
                QStringList list;
                if(hash != nullptr)
                {
                    if(hash->contains(name))
                    {
                        list = hash->value(name);
                    }
                }
                if (!flagUnnamedSection)
                {
                    QStringList list = hash->value(name);
                }

                if(value == nullptr)
                {
                    list.append("");
                } else
                {
                    list.append(value);
                }

                if (flagUnnamedSection){
                    hashUnnamedSection->insert(name, list);
                } else
                {
                    hash->insert(name,list);
                    hashOfSections->insert(keyHash, hash);
                }
            }
        }

    } else
    {
        ret = false;
        if(logger != nullptr)
            logger->log(tr("Не получилось открыть файл"));
    }
    file.close();
    return ret;
}

void IniReadWrite::setCodec(codecs codecIndex, bool reReadFlag)
{
    currentCodec = codecIndex;
    if(reReadFlag)
    {
        readIniFile();
    }
}

bool IniReadWrite::SetField(QString option, bool value, const QString& section, bool flagCreateNewOption)
{
    QString boolText = value ? "ON" : "OFF";
    return SetField(std::move(option), boolText, section,flagCreateNewOption);
}

bool IniReadWrite::SetField(QString option, const QString& value, const QString& section, bool flagCreateNewOption)
{
    bool ret = true;

    option = option.toUpper();

    QString string;
    QStringList listFile;
    QFile file(inifile);
    QString comment = "";
    int indexOfComment = -1;
    if(file.open(QFile::ReadWrite | QFile::Text))
    {
        QTextStream stream(&file);
        stream.setCodec(mapOfCodecs.value(currentCodec).toUtf8());
        string = stream.readAll();
        //собираем строку на подстроки
        listFile = string.split("\n");

        if((listFile.size() == 1) && (listFile[0] == ""))
        {
            if(logger != nullptr){
                logger->log(tr("Файл настроек пустой!!!"));
            }
            if(!flagCreateNewOption)
            {
                file.close();
                return false;
            }
        }

        int indexOfSection = 0;
        int flagNoOption = true;
        int flagNoSection = true;
        while (indexOfSection != -1)
        {
            int indexOfOption = -1;
            indexOfSection = listFile.indexOf(QRegularExpression(R"((^\[)" + section + R"(\]\s*;.*$|^\[)" + section + R"(\]$))", QRegularExpression::CaseInsensitiveOption), indexOfSection); // индекс строки нужной секции
            //найдена ли была секция?
            if (indexOfSection != -1)
            {
                flagNoSection = false;
                for (int i = indexOfSection + 1; i < listFile.size(); i++)
                {
                    QString string = listFile[i];
                    if (QRegularExpression(R"((^\[)" + section + R"(\]\s*;.*$|^\[)" + section + R"(\]$))", QRegularExpression::CaseInsensitiveOption).match(string).hasMatch())
                    {
                        continue;
                    }
                    if (QRegularExpression(R"((^\[.+\]\s*;.*$|^\[.+\]$))").match(string).hasMatch())
                    {
                        while ((!QRegularExpression(R"((^\[)" + section + R"(\]\s*;.*$|^\[)" + section + R"(\]$))", QRegularExpression::CaseInsensitiveOption).match(string).hasMatch()) && (i < listFile.size()))
                        {
                            string = listFile[i];
                            i++;
                        }
                    }
                    if (QRegularExpression("^" + QRegularExpression::escape(option) + "(?==.*)").match(string).hasMatch())
                    {
                        flagNoOption = false;
                        indexOfOption = i;
                        break;
                    }
                }
            } else if (indexOfSection == -1)
            {
                for (int i = indexOfSection + 1; i < listFile.size(); i++)
                {
                    QString string = listFile[i];
                    if (QRegularExpression(R"((^\[.+\]\s*;.*$|^\[.+\]$))").match(string).hasMatch())
                    {
                        break;
                    }
                    if (QRegularExpression("^" + QRegularExpression::escape(option) + "(?==.*)").match(string).hasMatch())
                    {
                        flagNoOption = false;
                        indexOfOption = i;
                        break;
                    }
                }
            }

            if (indexOfOption == -1)
            {
                if(indexOfSection == -1)
                {
                    indexOfSection++;
                }
                ret = false;
                break;
            }

            indexOfComment = listFile[indexOfOption].indexOf(QRegularExpression(R"(\s+;)"));
            if(indexOfComment != -1)
            {
                comment = listFile[indexOfOption].right(listFile[indexOfOption].count() - indexOfComment);
            }
            if(comment != "")
            {
                listFile[indexOfOption] = option + "=" + value + comment;
            } else
            {
                listFile[indexOfOption] = option + "=" + value;
            }
            ret = true;
            break;
        }
        if(flagNoOption)
        {
            if(flagCreateNewOption)
            {
                file.close();
                bool ret = CreateOption(section, option, value);
                if(logger != nullptr)
                {
                    logger->log(tr("В секции %1 не была найдена опция %2 в файле настроек %3. Создаём её. %4").arg(section, option, file.fileName(), (ret ? "TRUE" : "FALSE")));
                }
                return static_cast<bool>(true);
            }
            if(logger != nullptr)
            {
                logger->log(tr("В секции %1 не была найдена опция %2 в файле настроек %3").arg(section, option, file.fileName()));
            }
            ret = false;
        }
        if(flagNoSection && (section != nullptr))
        {
            if(logger != nullptr)
            {
                logger->log(tr("Секция %1 не найдена в файле настроек %2").arg(section, file.fileName()));
            }
            ret = false;
        }
        if (ret)
        {
            string = "";
            //собираем строки обратно в одну строку
            string = listFile.join("\n");
            stream.seek(0);
            stream << string;
            file.resize(file.pos());
        }
    }
    else
    {
        ret = false;
        if(logger != nullptr)
        {
            logger->log(tr("Не удалось прочитать файл %1 !").arg(inifile));
        }
    }
    file.close();
    return ret;
}

QString IniReadWrite::getIniFile()
{
    return this->inifile;
}

bool IniReadWrite::CreateOption(const QString& section, QString option, const QString& value)
{
    QString string;
    QStringList listFile;
    QFile file(inifile);
    option = option.toUpper();
    if(file.open(QFile::ReadWrite | QFile::Text))
    {
        QTextStream stream(&file);
        stream.setCodec(mapOfCodecs.value(currentCodec).toUtf8());
        string = stream.readAll();
        //собираем строку на подстроки
        listFile = string.split("\n");
    } else
    {
        if(logger != nullptr)
        {
            logger->log(tr("Не удалось прочитать файл %1 !").arg(inifile));
        }
    }

    int indexOfSection = 0;
    if (section != nullptr)
    {
        indexOfSection = listFile.indexOf(QRegularExpression(R"((^\[)" + section + R"(\]\s*;.*$|^\[)" + section + R"(\]$))", QRegularExpression::CaseInsensitiveOption)); // индекс строки нужной секции

        if(indexOfSection == -1)
        {
            listFile.append("[" + section + "]");
            listFile.append(option + "=" + value);
        } else if(section != nullptr)
        {
            listFile.insert(indexOfSection + 1, option + "=" + value);
        }
    } else
    {
        listFile.prepend(option + "=" + value);
    }
    string = "";
    //собираем строки обратно в одну строку
    string = listFile.join("\n");
    file.seek(0);
    QTextStream out(&file);
    out.setCodec(mapOfCodecs.value(currentCodec).toUtf8());
    out << string;
    file.resize(file.pos());
    file.close();
    return true;
}

bool IniReadWrite::GetText(QString option, QString& value, QString section, int index)
{
    option = option.toUpper();
    // Не указана секция, ищем в неименнуемом массиве
    if(section == nullptr)
    {
        if(hashUnnamedSection->contains(option))
        {
            if(index < hashUnnamedSection->value(option).size())
            {
                value = hashUnnamedSection->value(option).at(index);
                return true;
            }
            return false;
        }
        return false;
    } else
    {
        section.prepend("[");
        section.append("]");
        QString key = "";
        QHash<QString,QStringList>* hashOptions = getHashValue(section, hashOfSections, key);
        if(hashOptions && (key != ""))
        {
            if(hashOptions->contains(option))
            {
                if(index < hashOptions->value(option).size())
                {
                    value = hashOptions->value(option).at(index);
                    return true;
                }
                return false;
            } else
            {
                return false;
            }
        } else
        {
            return false;
        }
    }
}

// получить целое значение i-ой опции option
bool IniReadWrite::GetInt (QString option, int & value, QString section, int index)
{
    bool ret = false;
    QString s;
    if (GetText(std::move(option), s, std::move(section), index))
    {
        value = s.toInt(&ret);
    }
    return ret;
}

// получить догическое значение i-ой опции option
bool IniReadWrite::GetBool(QString option, bool & value, QString section, int index)
{
    bool ret = false;
    QString s;
    if (GetText(std::move(option), s, std::move(section), index))
    {
        s = s.toUpper();
        value = (s == "ON") || (s == "TRUE");
        ret = true;
    }
    return ret;
}

IniReadWrite::~IniReadWrite()
{
    foreach(QString s, hashOfSections->keys())
    {
        delete hashOfSections->value(s);
    }
    delete hashOfSections;
    delete hashUnnamedSection;
}

QMap<codecs, QString> IniReadWrite::initMapOfCodecs()
{
    QMap<codecs, QString> map;
    map.insert(Windows1251, "Windows-1251");
    map.insert(UTF8, "UTF-8");
    return map;
}

QHash<QString,QStringList>* IniReadWrite::getHashValue(const QString& strKey, QHash<QString, QHash<QString,QStringList> *>*hashLookup, QString &hashKey)
{
    if(hashLookup != nullptr)
    {
        QList<QString> keys = hashLookup->keys();
        foreach(const QString& key, keys)
        {
            if(strKey.contains(key, Qt::CaseInsensitive))
            {
                hashKey = key;
                return hashLookup->value(key);
            }
        }
    }
    return nullptr;
}
