#ifndef CONNECTIONSWIDGET_H
#define CONNECTIONSWIDGET_H

#include <QWidget>

namespace Ui {
class ConnectionsWidget;
}

class ConnectionsWidget : public QWidget
{
    Q_OBJECT

public Q_SLOTS:
    void clientStateChanged(QString ipclient, bool isConnected);

public:
    explicit ConnectionsWidget(QWidget *parent = nullptr);
    ~ConnectionsWidget();

private slots:
    void on_connections_itemSelectionChanged();

    void on_clearSelectedClientsButton_clicked();

private:
    Ui::ConnectionsWidget *ui;
};

#endif // CONNECTIONSWIDGET_H
