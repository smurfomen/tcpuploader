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
    auto items = ui->connections->findItems(ipclient, Qt::MatchContains);
    if(isConnected && items.isEmpty())
    {
        ui->connections->insertItem(0, ipclient);
    }
    else if (!isConnected)
    {
        for(auto it : items)
            ui->connections->removeItemWidget(it);
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
