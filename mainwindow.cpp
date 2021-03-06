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
#include <QFileDialog>
#include <QProcess>

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
    ui->cpTabView->setColumnWidth(1,300);
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
    int cpCnt = ui->cpTabView->model()->rowCount();
    if(cpCnt == 0) {
        return;
    }
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

    if(cpCnt == ui->cpTabView->model()->rowCount()) {
        errMsg->showMessage("No file change yet!");
    }
}

void MainWindow::on_btnRemount_clicked()
{
    QString mountPath = QFileDialog::getExistingDirectory(this,"Choose the mounting point",
                                                          "/home",
                                                          QFileDialog::ShowDirsOnly
                                                          | QFileDialog::DontResolveSymlinks);
    QProcess umount;
    umount.start("umount", QStringList() << mountPath);
    if(!umount.waitForStarted()){
        errMsg->showMessage("Start umount failed");
        return;
    }
    umount.waitForFinished();
    QString output = QString(umount.readAllStandardError());

    if(output != "") {
        errMsg->showMessage("Unmount failed!\n\n"+output);
        return;
    }

    QProcess remount;
    QString i = ui->cboxAddr->currentText();
    bool convertOK;
    long long intNum = i.toLongLong(&convertOK, 10);
    QString hexStr = QString::number(intNum, 16).prepend("0x");
    QString mntCMD="mount -t hmfs -o physaddr=" + hexStr + ",uid=1000,gid=1000 none " + mountPath;
    remount.start("sh", QStringList() << "-c" << mntCMD);
    if(!remount.waitForStarted()){
        errMsg->showMessage("Start remount failed");
        return;
    }
    output = QString(remount.readAllStandardError());
    remount.waitForFinished();
    if(output!=""){
        errMsg->showMessage("Remount failed!\n\n "+output);
        return;
    }
}
