#ifndef EXCHANGEWIDGET_H
#define EXCHANGEWIDGET_H
#include "server.h"
#include <QDir>
#include <memory>
#include <QStringList>
#include <QWidget>

namespace Ui {
class ExchangeWidget;
}

class ExchangeWidget : public QWidget
{
    Q_OBJECT
private slots:
    // выбрать директорию-источник с файлами
    void on_selectSourceDirButton_clicked();

    // передать выделенные файлы
    void on_sendSelectedFile_clicked();

    // отметить в списке отправленных файлов переданный файл
    void fileSended(QString filepath, QString ipdest);


public:
    explicit ExchangeWidget(QWidget *parent = nullptr);
    ~ExchangeWidget();

private:
    void sendFiles(QStringList files, QStringList clients);
    QDir sourceDir;
    Ui::ExchangeWidget *ui;
};

#endif // EXCHANGEWIDGET_H
