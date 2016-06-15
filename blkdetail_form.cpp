#include "blkdetail_form.h"
#include "ui_blkdetail_form.h"

#include <QFile>
#include <QTextStream>
#include <QPainter>
#include <QPalette>

Blkdetail_form::Blkdetail_form(QWidget *parent, QString path) :
//    QDialog(parent),
    ui(new Ui::Blkdetail_form)
{
    ui->setupUi(this);
    /* 1. setup SpinBox */
    QFile cp_file("/sys/kernel/debug/hmfs/"+path+"/info");
    if(!cp_file.open(QIODevice::ReadWrite)){
        /* TODO: error prompt */
        return;
    }
    QTextStream out(&cp_file);
    out << "sit s" <<endl;
    QTextStream in(&cp_file);

    QString line = in.readLine();
    QStringList list = line.split(": ");
    ui->segno_spin->setMinimum(1);
    ui->segno_spin->setMaximum(list[1].toInt());
    cp_file.close();
    /* 2. setup RenderArea*/
    renderArea =  new RenderArea;
    renderArea->setFixedSize(100,100);
    //renderArea->paint();
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(renderArea);
    ui->groupBox->setLayout(vbox);
}

Blkdetail_form::~Blkdetail_form()
{
    delete ui;
}

RenderArea::RenderArea(QWidget *parent) {
    //qpen = new QPen();
    //qbrush = new QBrush();
    QPalette palette(RenderArea::palette());
    palette.setColor(backgroundRole(), Qt::white);
    setPalette(palette);
    this->init = false;
}

void RenderArea::setInit(bool flag) {
    this->init = flag;
}

void RenderArea::paintEvent (QPaintEvent *event){
    if(!this->init) return;
    //this->setMinimumSize(100,100);
    //this->setMaximumSize(200,200);
    this->setFixedSize(800,800);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);
    QBrush brush(Qt::black);

    int x=10,y=5,w=24,h=24;
    //TODO: config macro
    for(int i=0, curY=y; i<16; i++, curY+=h){
        for(int j=0, curX=x; j<32; j++, curX+=w){
            QPainterPath path;
            path.addRect(curX, curY, w, h);
            brush.setColor(typeColor[blkType[i*32+j]]);
            painter.fillPath(path, brush);
            painter.drawPath(path);
        }
    }
    //this->setInit(false);
}

RenderArea::~RenderArea() {
}
void Blkdetail_form::on_blkDetail_btn_clicked()
{
    /* 1. get blk info */
    QFile cp_file("/sys/kernel/debug/hmfs/2147483648/info");
    if(!cp_file.open(QIODevice::ReadWrite)){
        /* TODO: error prompt */
        return;
    }
    QTextStream out(&cp_file);
    out << "ssa "<< ui->segno_spin->value() <<endl;
    QTextStream in(&cp_file);

    QString  line = in.readLine();
    QStringList list;
    int curOfs;
    while(!in.atEnd()) {
        if(0==line.length()) {
            line = in.readLine();
            continue;
        }
        if(line.contains("--")) {
            list = line.split(": "); //FORMAT: `-- [<segno>: <blkofs>] --`
            if(list.count()<1) continue; //TODO error
            list = list[1].split("]");
            curOfs = list[0].toInt();
        } else if(line.contains("type")) {
            list = line.split(": ");
            if(list.count()<1) continue; //TODO error
            renderArea->blkType[curOfs] = list[1].toInt();
        } else if(line.contains("v bit")) {
            list = line.split(": ");
            if(list.count()<1) continue; //TODO error
            if( 0 == list[1].toInt()) {
                renderArea->blkType[curOfs] = TYPE_INVALID;
            }
        }
        line = in.readLine();
    }
    cp_file.close();

    /* 2. visualized result */
    this->renderArea->setInit(true);
    this->renderArea->repaint();
}

void Blkdetail_form::on_segno_spin_editingFinished()
{
    this->on_blkDetail_btn_clicked();
}
