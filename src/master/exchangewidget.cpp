#include "exchangewidget.h"
#include "ui_exchangewidget.h"
#include <QFileDialog>
#include "ini.h"
#include "response/remoterq.h"
#include "response/response.h"
ExchangeWidget::ExchangeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExchangeWidget)
{
    ui->setupUi(this);
}

ExchangeWidget::~ExchangeWidget()
{
    delete ui;
}

void ExchangeWidget::sendFiles(QStringList files, QStringList clients)
{
    for(auto ip : clients)
    {
        // отправляем все файлы найденному клиенту
        for(auto filepath : files)
        {
            QFileInfo fi (filepath);
            filepath = fi.absoluteFilePath();

            // перед этим сначала формируем фейковый запрос, куда вкладываем информацию о путях к файлам
            std::shared_ptr<RemoteRq> rq = std::shared_ptr<RemoteRq>(new RemoteRq(rqRead, ip));
            rq->setsrc(QHostAddress(ip));
            rq->setdst(QHostAddress::Any);                          // TODO: Указывать мой адрес
            rq->setParam(QVariant(filepath));                       // src (что и откуда взять)
            rq->setParam2(0);                                       // offset (с какого места читать)
            rq->setParam3(fi.size());                               // length сколько читать
            rq->setParam4("/home/pcuser/upload/"+fi.fileName());    // dst (что и куда)

            QByteArray data = FactoryResponse().fromRequest(rq, logger)->Serialize();
            if(server->findClientAndSend(data,ip))
                fileSended(filepath, ip);
        }
    }
}



// выбрать директорию-источник файлов
void ExchangeWidget::on_selectSourceDirButton_clicked()
{
    sourceDir = QFileDialog::getExistingDirectory(this,"Выберите директорию с файлами для загрузки", QDir::homePath(), QFileDialog::ShowDirsOnly);
    ui->selectedSourceDir->setText(sourceDir.absolutePath());
    QFileInfoList files = sourceDir.entryInfoList();
    ui->sourceFiles->clear();

    for(auto file : files)
    {
        if(!file.isFile())
            continue;
        ui->sourceFiles->insertItem(0,file.fileName());
        ui->sourceFiles->item(0)->setData(Qt::UserRole, file.filePath());
    }
}



// передать файлы
void ExchangeWidget::on_sendSelectedFile_clicked()
{
    QList<QListWidgetItem*> selectedItems = ui->sourceFiles->selectedItems();
    //файлы к передаче
    QStringList toSendFiles;

    for(auto it : selectedItems)
    {
        toSendFiles.append(it->data(Qt::UserRole).toString());
        it->setSelected(false);
    }

    if(toSendFiles.size())
        sendFiles(toSendFiles, selectedCients);
}



void ExchangeWidget::fileSended(QString filepath, QString ipdest)
{
    // выделяем в source то, что отправили зеленым
    for(int i = 0; i < ui->sourceFiles->count(); i++)
    {
        if(ui->sourceFiles->item(i)->data(Qt::UserRole).toString() == filepath)
        {
            ui->sourceFiles->item(i)->setBackgroundColor(Qt::darkGreen);
            break;
        }
    }

    // проверяем был ли файл уже отправлен
    int row = -1;
    for(int i = 0; i < ui->destFiles->count(); i++)
    {
        if(ui->destFiles->item(i)->data(Qt::UserRole).toString() == filepath)
        {
            row = i;
            break;
        }
    }

    // если уже такой есть - добавляем ip к которому была передача
    if(row >= 0)
    {
        QListWidgetItem * it = ui->destFiles->item(row);
        if(!it->text().contains(ipdest))
            it->setText(QString("%1:%2").arg(it->text()).arg(ipdest));
    }
    // если еще нет в списке
    else
    {
        ui->destFiles->insertItem(0, QString("%1:%2").arg(QFileInfo(filepath).fileName()).arg(ipdest));
        ui->destFiles->item(0)->setData(Qt::UserRole, filepath);
    }
}
