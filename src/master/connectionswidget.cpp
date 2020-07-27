#include "connectionswidget.h"
#include "ui_connectionswidget.h"
#include "ini.h"
ConnectionsWidget::ConnectionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionsWidget)
{
    ui->setupUi(this);
    connect(server.get(), &Server::clientStateChanged, this, &ConnectionsWidget::clientStateChanged);
}

ConnectionsWidget::~ConnectionsWidget()
{
    delete ui;
}

void ConnectionsWidget::clientStateChanged(QString ipclient, bool isConnected)
{
    QList<QListWidgetItem*> findClones;
    for(int i = 0; i < ui->connections->count(); i++)
    {
        QListWidgetItem * item = ui->connections->item(i);
        if(item->text() == ipclient)
            findClones.append(item);
    }

    if(isConnected && findClones.isEmpty())
    {
        ui->connections->insertItem(0, ipclient);
    }
    else if (!isConnected)
    {
        for(auto clone : findClones)
        {
            ui->connections->removeItemWidget(clone);
            delete clone;
        }
    }
}

void ConnectionsWidget::on_connections_itemSelectionChanged()
{
    selectedCients.clear();
    for(auto it : ui->connections->selectedItems())
    {
        selectedCients.append(it->text());
    }
}

void ConnectionsWidget::on_clearSelectedClientsButton_clicked()
{
    ui->connections->clearSelection();
}
