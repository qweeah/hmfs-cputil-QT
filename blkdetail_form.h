#ifndef BLKDETAIL_FORM_H
#define BLKDETAIL_FORM_H

#include <QDialog>
#include <QMainWindow>
#include <QPen>
#include <QPixmap>
#include <QBrush>

#define TYPE_INVALID 9

namespace Ui {
class Blkdetail_form;
}

class RenderArea : public QWidget {
    Q_OBJECT
public:
    RenderArea(QWidget *parent = 0);
    ~RenderArea();
    void paint();
    void setInit(bool flag);
    int blkType[512];
private:
    bool init;
    const int typeColor[10] = {
        0x6E8B3D,   // data block
        0x6E8B3D,   // extended data block == data
        0x00688B,   // inode block
        0xB0E2FF,   // direct block
        0x00B2EE,   // indirect block
        0xFF7F00,   // nat node block
        0xFFE4C4,   // nat data block
        0x696969,   // checkppoint block
        0x696969,   // orphan block == cp block
        0xFFFFFF    // invalid block
    };
protected:
    void paintEvent ( QPaintEvent * event);
};

class Blkdetail_form : public QDialog
{
    Q_OBJECT
public:
    explicit Blkdetail_form(QWidget *parent, QString path);
    ~Blkdetail_form();
    RenderArea *renderArea;

private slots:
    void on_blkDetail_btn_clicked();

    void on_segno_spin_editingFinished();

private:
    Ui::Blkdetail_form *ui;
};

#endif // BLKDETAIL_FORM_H
