#include "selectDiskDialog.h"
#include "ui_selectdiskdialog.h"
#include <QDir>
#include <QModelIndex>
#include <mainwindow.h>
#include <windows.h>
#include <QPushButton>
#include <iostream>

SelectDiskDialog::SelectDiskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectDiskDialog)
{
    ui->setupUi(this);
    QList drives = QDir::drives();    
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    for(int i=0; i<drives.length(); i++){
        char NameBuffer[MAX_PATH];
        char SysNameBuffer[MAX_PATH];
        DWORD VSNumber;
        DWORD MCLength;
        DWORD FileSF;
        if (GetVolumeInformationA(drives[i].absoluteFilePath().toStdString().c_str(), NameBuffer, sizeof(NameBuffer),
            &VSNumber,&MCLength,&FileSF,SysNameBuffer,sizeof(SysNameBuffer)))
        {
            if(!strncmp(SysNameBuffer, "FAT", 3) && (strncmp(SysNameBuffer, "FAT32", 5)))
                ui->listWidget->addItem(drives[i].absoluteFilePath() + '\t' + SysNameBuffer +"16");
            else
                ui->listWidget->addItem(drives[i].absoluteFilePath() + '\t' + SysNameBuffer);

            if((strncmp(SysNameBuffer, "FAT", 3))&&(strncmp(SysNameBuffer, "FAT32", 5))){
                 ui->listWidget->item(i)->setForeground(Qt::gray);
        }
    }
 }

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
}
void SelectDiskDialog::on_buttonBox_accepted()
{
    QModelIndex index = ui->listWidget->currentIndex();
    QString itemText = index.data(Qt::DisplayRole).toString();
    emit buttonOkClickedSignal(itemText);   
    this->close();
}

void SelectDiskDialog::on_listWidget_itemSelectionChanged()
{
    if(ui->listWidget->currentItem()->foreground() != Qt::gray)
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    else
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}
