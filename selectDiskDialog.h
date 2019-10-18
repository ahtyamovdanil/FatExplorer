#ifndef SELECTDISKDIALOG_H
#define SELECTDISKDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class SelectDiskDialog;
}

class SelectDiskDialog : public QDialog
{
    Q_OBJECT

signals:
    void buttonOkClickedSignal(QString var);
public:
    explicit SelectDiskDialog(QWidget *parent = nullptr);
    QString getSelectedDisk();
    ~SelectDiskDialog();


public slots:
    void on_buttonBox_accepted();

private:
    Ui::SelectDiskDialog *ui;
};

#endif // SELECTDISKDIALOG_H
