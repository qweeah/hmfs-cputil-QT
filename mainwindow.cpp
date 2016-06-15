#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cpdetail_form.h"
#include "blkdetail_form.h"

#include <QFile>
#include <QTextStream>
#include <QStandardItemModel>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //init table header
    QStandardItemModel *model = new QStandardItemModel();
    ui->cpTabView->setModel(model);
    ui->cpTabView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //get all formatted volumes
    QDir dir("/sys/kernel/debug/hmfs");
    if (!dir.exists()) {
        qWarning("Cannot find the example directory");
        exit(EXIT_FAILURE);
    }
    QFileInfoList list = dir.entryInfoList();
    for (int i = 2; i < list.size(); ++i) { // skip '.' and '..'
        QFileInfo fileInfo = list.at(i);
        ui->cboxAddr->addItem(fileInfo.fileName());
    }
    this->errMsg = new QErrorMessage();
}

MainWindow::~MainWindow()
{
    delete ui;
    free(errMsg);
}
/* on_btnShow_clicked
 *    -- Show all checkpoints from selected volume
 */
void MainWindow::on_btnShow_clicked()
{
    QString path = ui->cboxAddr->currentText();
    if(path == NULL) {
        errMsg->showMessage("Please choose a volume!");
        return;
    }
    QFile cp_file("/sys/kernel/debug/hmfs/"+path+"/info");
    if(!cp_file.open(QIODevice::ReadWrite)){
        errMsg->showMessage("Cannot open info file in debugfs!");
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
    //FIXME: repeated strectch cause expansion in first column
    //QHeaderView *hv = ui->cpTabView->horizontalHeader();
    //hv->setStretchLastSection(true);

}

void MainWindow::refresh()
{
    on_btnShow_clicked();
}

void MainWindow::on_pushButton_clicked()
{
    QString path = ui->cboxAddr->currentText();
    if(path == NULL) {
        errMsg->showMessage("Please choose a volume!");
        return;
    }
    Blkdetail_form *cp = new Blkdetail_form(0, path);
    cp->show();
}

void MainWindow::on_cpTabView_doubleClicked(const QModelIndex &index)
{
    QString path = ui->cboxAddr->currentText();
    if(path == NULL) {
        errMsg->showMessage("Please choose a volume!");
        return;
    }
    int verNum = index.sibling(index.row(), 0).data().toInt();
    cpDetail_form *cp = new cpDetail_form(this, verNum, path);
    cp->show();
    this->hide();
}

void MainWindow::on_pushButton_2_clicked()
{
    QString path = ui->cboxAddr->currentText();
    if(path == NULL) {
        errMsg->showMessage("Please choose a volume!");
        return;
    }
    QFile cp_file("/sys/kernel/debug/hmfs/"+path+"/info");
    if(!cp_file.open(QIODevice::ReadWrite)){
        errMsg->showMessage("Cannot open info file in debugfs!");
        return;
    }

    QTextStream out(&cp_file);
    out << "cp t" << endl;
    QTextStream in(&cp_file);
    bool done = false;
    QStandardItemModel *model = (QStandardItemModel*)ui->cpTabView->model();
    model->clear();
    QString line = in.readLine();
    if(line.contains("added")){
        done = true;
    }
    cp_file.close();
    if(done) {
        on_btnShow_clicked();
    } else {
        errMsg->showMessage("Failed to add a new checkpoint");
    }//TODO else prompt
}
