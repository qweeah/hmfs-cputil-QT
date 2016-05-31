#ifndef CPDETAIL_FORM_H
#define CPDETAIL_FORM_H

#include <QDialog>
#include <QHash>
#include <QString>
#include <QLineEdit>
#include <QMainWindow>
#include <QErrorMessage>

namespace Ui {
class cpDetail_form;
}

class cpDetail_form : public QDialog
{
    Q_OBJECT

public:
    explicit cpDetail_form(QWidget *parent);
    explicit cpDetail_form(QMainWindow *retForm, int cnt, QString path);
    ~cpDetail_form();

private slots:
    void on_cpDetail_form_finished(int result);

    void on_btnMount_clicked();

private:
    Ui::cpDetail_form *ui;
    QHash<QString, QLineEdit*> hash;
    QMainWindow *retForm;
    int verNum;
    QErrorMessage *errMsg;
};

#endif // CPDETAIL_FORM_H
