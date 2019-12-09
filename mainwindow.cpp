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
#include <QTimer>
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
    ui->menunone->setTitle(path);
    app->setBootTable(ui->bootTable);
    app->setFatTable(ui->fatTable);
    app->setFolderTable(ui->rootTable);
    ui->fatTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->bootTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->rootTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->fatTable, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(fatCustomMenuRequested(QPoint)));
    connect(ui->bootTable, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(bootCustomMenuRequested(QPoint)));
    connect(ui->rootTable, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(rootCustomMenuRequested(QPoint)));

    MainWindow::setStyleSheet("QMainWindow{background-color: grey}");
    auto mTimer = new QTimer(this);
    mTimer->setSingleShot(true);
    connect(mTimer, SIGNAL(timeout()), SLOT(setWhite()));
    mTimer->start(125);
}

void MainWindow::on_actionupdate_triggered()
{
    this->app->setBootTable(ui->bootTable);
    this->app->setFatTable(ui->fatTable);
    this->app->setFolderTable(ui->rootTable);
    ui->rootTable->update();
    ui->fatTable->update();

    MainWindow::setStyleSheet("QMainWindow{background-color: grey}");
    auto mTimer = new QTimer(this);
    mTimer->setSingleShot(true);
    connect(mTimer, SIGNAL(timeout()), SLOT(setWhite()));
    mTimer->start(125);
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
        this->app->setFolderTable(ui->rootTable, ui->rootTable->item(row, 3)->text().toInt(), 10);
        ui->rootTable->scrollToTop();
        ui->fatTable->update();
    }
}

void MainWindow::on_fatTable_cellClicked(int row, int column)
{
    std::function<bool(int,int)> forward;
    forward = [&](int r, int c) {
        if((ui->fatTable->item(r, c)->text().toStdString() == "RES") ||
           (ui->fatTable->item(r, c)->text().toStdString() == "0"))
            return false;
        else
            if(ui->fatTable->item(r, c)->text().toStdString() == "EOC"){
                ui->fatTable->item(r,c)->setSelected(true);
                return false;
            }
        else
        {
            ui->fatTable->item(r,c)->setSelected(true);
            int clmn_nxt = ui->fatTable->item(r, c)->text().toInt() % 10;
            int row_nxt = ui->fatTable->item(r, c)->text().toInt()/10;
            return (forward(row_nxt, clmn_nxt));
        }
    };

    int cur_row = row;
    int cur_col = column;
    for(int i=row; i>=0; i--)
        for(int j=9; j>=0; j--){
            std::cout << ui->fatTable->item(i, j)->text().toInt() << ' ' << cur_row*10+cur_col << std::endl;
            if(ui->fatTable->item(i, j)->text().toInt() == cur_row*10+cur_col){
                ui->fatTable->item(i, j)->setSelected(true);
                cur_row = i;
                cur_col = j;
            }
        }

    forward(row, column);

}

void MainWindow::on_fatTable_cellActivated(int row, int column){}

void MainWindow::setWhite(){
    MainWindow::setStyleSheet("QMainWindow{background-color: white}");
    ui->bootTable->setStyleSheet("background-color: white");
}

void MainWindow::bootCustomMenuRequested(const QPoint& pos){
    QPoint qpoint = ui->bootTable->viewport()->mapToGlobal(pos);
    auto menu = QMenu();
    auto copyAction = menu.addAction("Копировать");
    QAction *action =  menu.exec(qpoint);
    if (action == copyAction){
        QAbstractItemModel * model = ui->bootTable->model();
        QItemSelectionModel * selection = ui->bootTable->selectionModel();
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

void MainWindow::fatCustomMenuRequested(const QPoint& pos){
    std::cout<<"lolkek"<<std::endl;
    QPoint qpoint = ui->fatTable->viewport()->mapToGlobal(pos);
    //QPoint qpoint = qtable->viewport()->mapToGlobal(pos);
    auto menu = QMenu();
    auto copyAction = menu.addAction("Копировать");
    QAction *action =  menu.exec(qpoint);
    if (action == copyAction){
        QAbstractItemModel * model = ui->fatTable->model();
        QItemSelectionModel * selection = ui->fatTable->selectionModel();

        //QAbstractItemModel * model = qtable->model();
        //QItemSelectionModel * selection = qtable->selectionModel();
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

void MainWindow::rootCustomMenuRequested(const QPoint& pos){
    std::cout<<"lolkek"<<std::endl;
    QPoint qpoint = ui->rootTable->viewport()->mapToGlobal(pos);
    //QPoint qpoint = qtable->viewport()->mapToGlobal(pos);
    auto menu = QMenu();
    auto copyAction = menu.addAction("Копировать");
    QAction *action =  menu.exec(qpoint);
    if (action == copyAction){
        QAbstractItemModel * model = ui->rootTable->model();
        QItemSelectionModel * selection = ui->rootTable->selectionModel();

        //QAbstractItemModel * model = qtable->model();
        //QItemSelectionModel * selection = qtable->selectionModel();
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
