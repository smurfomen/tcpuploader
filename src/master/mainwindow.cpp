#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include "ini.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setFixedSize(size());

    exchange = new ExchangeWidget(this);
    connections = new ConnectionsWidget(this);
    ui->gridLayout->addWidget(exchange);
    ui->gridLayout->addWidget(connections);

    hideAllWidgets();
    connections->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::hideAllWidgets()
{
    connections->hide();
    exchange->hide();
}

void MainWindow::on_exchangeAction_triggered()
{
    hideAllWidgets();
    exchange->show();
}

void MainWindow::on_connectionsAction_triggered()
{
    hideAllWidgets();
    connections->show();
}
