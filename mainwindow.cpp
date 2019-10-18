#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "selectDiskDialog.h"
#include <QMenuBar>
#include <QAction>
#include <QTableWidgetItem>
#include <fatAPI.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <application.h>
#include <QStorageInfo>
#include <QMessageBox>
#include <QListWidget>
#include <iostream>
#include <QDebug>
#include <QClipboard>
#include <QMimeData>
#include "info.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
        ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionopen_triggered()
{
    SelectDiskDialog selDialog;
    connect(&selDialog, SIGNAL(buttonOkClickedSignal(QString)), this, (SLOT(set_disk(QString))));
    selDialog.setModal(true);
    selDialog.exec();
}

void MainWindow::set_disk(QString path)
{
    char letter = path.toStdString().c_str()[0];
    char *final_path = new char[8]{'\\','\\','.','\\','-',':'};
    //{"\\\\.\\-:"};
    final_path[4] = letter;
    LPCSTR wszPath = final_path;

    this->app =  new Application(wszPath);
    std::cout<<std::string(app->path)<< " " << 1 << std::endl;
    app->setBootTable(ui->bootTable);
    app->setFatTable(ui->fatTable);
    app->setRootFolder(ui->rootTable);
    std::cout<<std::string(this->app->path)<< " " << 2 <<std::endl;

    ui->fatTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->fatTable, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(customMenuRequested(QPoint)));
}

void MainWindow::on_actionupdate_triggered()
{
    std::cout<<std::string(this->app->path)<<std::endl;
    this->app->setBootTable(ui->bootTable);
    this->app->setFatTable(ui->fatTable);
    //ui->rootTable->clear();
    this->app->setRootFolder(ui->rootTable);
    ui->rootTable->update();
    ui->fatTable->update();
}

void MainWindow::on_actioninfo_triggered()
{
    info inf;
    inf.setModal(true);
    inf.setVisible(true);
    inf.exec();
}

void MainWindow::on_rootTable_cellDoubleClicked(int row, int column)
{
    if(ui->rootTable->item(row,0)->background() == Qt::gray){
        this->app->setRootFolder(ui->rootTable, ui->rootTable->item(row, 3)->text().toInt(), 10);
        ui->rootTable->scrollToTop();
        ui->fatTable->update();
    }
}

void MainWindow::on_fatTable_cellClicked(int row, int column)
{
    std::function<bool(int,int)> kek;
    kek = [&](int r, int c) {
        //std::cout << ui->fatTable->item(r, c)->text().toInt() << std::endl;
        std::cout << r << ' ' << c << std::endl;
        if((ui->fatTable->item(r, c)->text().toStdString() == "RES") ||
           (ui->fatTable->item(r, c)->text().toStdString() == "0"))
            return false;
        else
            if(ui->fatTable->item(r, c)->text().toStdString() == "EOC"){
                ui->fatTable->item(r,c)->setSelected(true);
                //ui->fatTable->item(r, c)->setBackground(Qt::green);
                return false;
            }
        else
        {
            ui->fatTable->item(r,c)->setSelected(true);
            //ui->fatTable->item(r, c)->setBackground(Qt::green);
            int clmn_nxt = ui->fatTable->item(r, c)->text().toInt() % 10;
            int row_nxt = ui->fatTable->item(r, c)->text().toInt()/10;
            return (kek(row_nxt, clmn_nxt));
        };
    };
    kek(row, column);
}

void MainWindow::customMenuRequested(const QPoint& pos){
    std::cout<<"lolkek"<<std::endl;
    QPoint qpoint = ui->fatTable->viewport()->mapToGlobal(pos);
    auto menu = QMenu();
    auto copyAction = menu.addAction("Копировать");
    QAction *action =  menu.exec(qpoint);
    if (action == copyAction){
        QAbstractItemModel * model = ui->fatTable->model();
        QItemSelectionModel * selection = ui->fatTable->selectionModel();
        QModelIndexList indexes = selection->selectedIndexes();

        qSort(indexes);

            // You need a pair of indexes to find the row changes
            QModelIndex previous = indexes.first();
            indexes.removeFirst();
            QString selected_text;
            QModelIndex current;
            Q_FOREACH(current, indexes)
            {
                QVariant data = model->data(previous);
                QString text = data.toString();
                selected_text.append(text);

                if (current.row() != previous.row())
                    selected_text.append(QLatin1Char('\n'));
                else
                    selected_text.append(QLatin1Char('\t'));
                previous = current;
            }

            // add last element
            selected_text.append(model->data(current).toString());
            QMimeData * md = new QMimeData;
            md->setText(selected_text);
            qApp->clipboard()->setMimeData(md);
    }
}

void MainWindow::on_fatTable_cellActivated(int row, int column){}
