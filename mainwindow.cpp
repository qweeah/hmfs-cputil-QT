#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cpdetail_form.h"
#include "blkdetail_form.h"

#include <QFile>
#include <QTextStream>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //init table header
    QStandardItemModel *model = new QStandardItemModel();
    ui->cpTabView->setModel(model);
    ui->cpTabView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnShow_clicked()
{
    /* TODO: volume-specific file */
    QFile cp_file("/sys/kernel/debug/hmfs/2147483648/info");
    if(!cp_file.open(QIODevice::ReadWrite)){
        /* TODO: error prompt */
        return;
    }

    QTextStream out(&cp_file);
    out << "cp a" << endl;
    QTextStream in(&cp_file);

    QStandardItemModel *model = (QStandardItemModel*)ui->cpTabView->model();
    model->clear();
    int cnt = 0;
    QString line = in.readLine();
    while(!in.atEnd()) {
        if(line.contains("checkpoint_ver")){
            QStringList list = line.split(": ");
            model->setItem(cnt,0,new QStandardItem(list[1]));//XXX
        }
        else if(line.contains("wall_time")){
            QStringList list = line.split(": ");
            model->setItem(cnt,1,new QStandardItem(list[1]));//XXX
            cnt++;
        }
        line = in.readLine();
    }
    cp_file.close();
    model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("Version Number")));
    ui->cpTabView->setColumnWidth(0,140);
    model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Time")));
    ui->cpTabView->horizontalHeader()->setStretchLastSection(true);

}

void MainWindow::on_pushButton_clicked()
{
    Blkdetail_form *cp = new Blkdetail_form();
    cp->show();
}

void MainWindow::on_cpTabView_doubleClicked(const QModelIndex &index)
{
    cpDetail_form *cp = new cpDetail_form(this, index.row());
    cp->show();
    this->hide();


}
