#include "cpdetail_form.h"
#include "ui_cpdetail_form.h"

#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include <QFileDialog>
#include <QErrorMessage>
#include <QMessageBox>


cpDetail_form::cpDetail_form(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cpDetail_form)
{
    ui->setupUi(this);
}

cpDetail_form::cpDetail_form(MainWindow *retForm, int cnt, QString path):
    ui(new Ui::cpDetail_form)
{
    this->retForm = retForm;
    ui->setupUi(this);
    /* binding */
    hash["checkpoint_ver"] = ui->verNum_text;
    hash["alloc_block_count"] = ui->allocBlk_text;
    hash["valid_block_count"] = ui->validBlk_text;
//Version 1
/*
    hash["free_segment_count"] = ui->freeSeg_text;
    hash["cur_node_segno"] = ui->nodeSegNo_text;
    hash["cur_node_blkoff"] = ui->nodeSegOfs_text;
    hash["cur_data_segno"] = ui->dataSegNo_text;
    hash["cur_data_blkoff"] = ui->dataSegOfs_text;
*/
//Version 2
    hash["current segment (0)"] = ui->freeSeg_text;
    hash["current segment (1)"] = ui->nodeSegNo_text;
    hash["current segment (2)"] = ui->nodeSegOfs_text;
    hash["current segment (3)"] = ui->dataSegNo_text;
    hash["current segment (4)"] = ui->dataSegOfs_text;

    hash["valid_inode_count"] = ui->validInode_text;
    hash["valid_node_count"] = ui->validNode_text;
    hash["wall_time"] = ui->wallTime_text;
    verNum=cnt;
    dbgPath = "/sys/kernel/debug/hmfs/" + path;
    errMsg = new QErrorMessage(this);

    QFile cp_file("/sys/kernel/debug/hmfs/"+path+"/info");
    if(!cp_file.open(QIODevice::ReadWrite)){
        /* TODO: error prompt */
        return;
    }

    QTextStream out(&cp_file);
    out << "cp "<< verNum <<endl;
    QTextStream in(&cp_file);

    QString line = in.readLine();
    while(!in.atEnd()) {
        if(!line.length()) {
            line = in.readLine();
            continue;
        }
        QStringList list = line.split(":");
        QLineEdit *t=hash.value(list[0]);
        if(t != NULL){
            t->setText(list[1].trimmed());
        }
        line = in.readLine();
    }
}

cpDetail_form::~cpDetail_form()
{
    delete ui;
    free(errMsg);
}

void cpDetail_form::on_cpDetail_form_finished(int result)
{
    this->retForm->show();
    this->retForm->refresh();
    this->hash.clear();
}

void cpDetail_form::on_btnMount_clicked()
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
    //qDebug() << "Str: " << output;
    if(output != "") {
        errMsg->showMessage("Unmount failed!\n\n"+output);
        return;
    }

    QProcess remount;
    QString mntCMD="mount -t hmfs -o physaddr=0x80000000,uid=1000,gid=1000,ro,mnt_cp=" +QString::number(verNum)+" none " + mountPath;
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

void cpDetail_form::on_btnDelete_clicked()
{
    int r = QMessageBox::question(this,"Warning",
                                  "Do you want to delete checkpoint"+QString::number(verNum),
                                  QMessageBox::Yes|QMessageBox::Default,
                                  QMessageBox::No|QMessageBox::Escape);
    if(r == QMessageBox::Yes){
        QFile cp_file(dbgPath +"/info");
        if(!cp_file.open(QIODevice::ReadWrite)){
             errMsg->showMessage("Cannot open file\n");
            return;
        }

        QTextStream out(&cp_file);
        out << "cp d "<< verNum <<endl;
        QTextStream in(&cp_file);

        QString line = in.readLine();
        if(line.contains("deleted")) {
            this->close();
        } else {
            errMsg->showMessage("Error occurs during deleting checkpoint "
                                + QString::number(verNum));
        }
    }
}
