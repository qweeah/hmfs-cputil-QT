#ifndef CPDETAIL_FORM_H
#define CPDETAIL_FORM_H

#include <QDialog>
#include <QHash>
#include <QString>
#include <QLineEdit>
#include <QMainWindow>
#include <QErrorMessage>

#include "mainwindow.h"

namespace Ui {
class cpDetail_form;
}

class cpDetail_form : public QDialog
{
    Q_OBJECT

public:
    explicit cpDetail_form(QWidget *parent);
    explicit cpDetail_form(MainWindow *retForm, int cnt, QString path);
    ~cpDetail_form();

private slots:
    void on_cpDetail_form_finished(int result);

    void on_btnMount_clicked();

    void on_btnDelete_clicked();

private:
    Ui::cpDetail_form *ui;
    QHash<QString, QLineEdit*> hash;
    MainWindow *retForm;
    int verNum;                 //checkpoint number of currently displaying cp
    QErrorMessage *errMsg;
    QString dirName;            //name of debugfs directory
};

#endif // CPDETAIL_FORM_H
