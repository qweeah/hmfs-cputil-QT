#include "cpdetail_form.h"
#include "ui_cpdetail_form.h"

#include <QFile>
#include <QTextStream>


cpDetail_form::cpDetail_form(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cpDetail_form)
{
    ui->setupUi(this);
}

cpDetail_form::cpDetail_form(QMainWindow *retForm, int cnt):
    ui(new Ui::cpDetail_form)
{
    this->retForm = retForm;
    ui->setupUi(this);
    /* binding */
    hash["checkpoint_ver"] = ui->verNum_text;
    hash["alloc_block_count"] = ui->allocBlk_text;
    hash["valid_block_count"] = ui->validBlk_text;
    hash["free_segment_count"] = ui->freeSeg_text;
    hash["cur_node_segno"] = ui->nodeSegNo_text;
    hash["cur_node_blkoff"] = ui->nodeSegOfs_text;
    hash["cur_data_segno"] = ui->dataSegNo_text;
    hash["cur_data_blkoff"] = ui->dataSegOfs_text;
    hash["valid_inode_count"] = ui->validInode_text;
    hash["valid_node_count"] = ui->validNode_text;
    hash["wall_time"] = ui->wallTime_text;

    QFile cp_file("/sys/kernel/debug/hmfs/2147483648/info");
    if(!cp_file.open(QIODevice::ReadWrite)){
        /* TODO: error prompt */
        return;
    }

    QTextStream out(&cp_file);
    out << "cp "<< cnt <<endl;
    QTextStream in(&cp_file);

    QString line = in.readLine();
    while(!in.atEnd()) {
        if(!line.length()) continue;
        QStringList list = line.split(": ");
        QLineEdit *t=hash.value(list[0]);
        if(t != NULL){
            t->setText(list[1]);
        }
        line = in.readLine();
    }
}

cpDetail_form::~cpDetail_form()
{
    delete ui;
}

void cpDetail_form::on_cpDetail_form_finished(int result)
{
    this->retForm->show();
    this->hash.clear();
}
