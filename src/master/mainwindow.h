#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include "exchangewidget.h"
#include "connectionswidget.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private Q_SLOTS:
    void on_exchangeAction_triggered();

    void on_connectionsAction_triggered();

private:
    void hideAllWidgets();
    Ui::MainWindow *ui;
    ExchangeWidget * exchange;
    ConnectionsWidget * connections;
};

#endif // MAINWINDOW_H
