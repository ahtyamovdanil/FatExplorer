#include "selectDiskDialog.h"
#include "ui_selectdiskdialog.h"
#include <QDir>
#include <QModelIndex>
#include <mainwindow.h>
#include <iostream>

SelectDiskDialog::SelectDiskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectDiskDialog)
{
    ui->setupUi(this);
    QList drives = QDir::drives();
    //connect(ui->buttonBox, SIGNAL(accepted()), SLOT(MainWindow::on_actionopen_triggered()));
    //auto qlist = new QListWidget;
    //MainWindow::on_actionopen_triggered()
    for(int i=0; i<drives.length(); i++)
       ui->listWidget->addItem(drives[i].absoluteFilePath());
}

SelectDiskDialog::~SelectDiskDialog()
{
    delete ui;
}

QString SelectDiskDialog::getSelectedDisk()
{
    QModelIndex index = ui->listWidget->currentIndex();
    QString itemText = index.data(Qt::DisplayRole).toString();
    return itemText;
    //return nullptr;
}
void SelectDiskDialog::on_buttonBox_accepted()
{
    QModelIndex index = ui->listWidget->currentIndex();
    QString itemText = index.data(Qt::DisplayRole).toString();
    emit buttonOkClickedSignal(itemText);
    //std::cout << itemText.toLocal8Bit().constData() << std::endl;
    this->close();
}
