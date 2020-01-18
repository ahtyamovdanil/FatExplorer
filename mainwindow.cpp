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
#include <QWidget>
#include "info.h"
#include <waitingspinnerwidget.h>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
//#include <QtWaitingSpinner/waitingspinnerwidget.h>

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


    WaitingSpinnerWidget* spinner = new WaitingSpinnerWidget(this);

    spinner->setRoundness(70.0);
    spinner->setMinimumTrailOpacity(15.0);
    spinner->setTrailFadePercentage(70.0);
    spinner->setNumberOfLines(12);
    spinner->setLineLength(10);
    spinner->setLineWidth(5);
    spinner->setInnerRadius(10);
    spinner->setRevolutionsPerSecond(1);
    spinner->setColor(QColor(81, 4, 71));

    spinner->start();

    SelectDiskDialog selDialog;
    connect(&selDialog, SIGNAL(buttonOkClickedSignal(QString)), this, (SLOT(set_disk(QString))));
    selDialog.setModal(true);
    selDialog.exec();

    spinner->stop();
}

void MainWindow::set_disk(QString path)
{

    char letter = path.toStdString().c_str()[0];
    char *final_path = new char[8]{'\\','\\','.','\\','-',':'};
    //{"\\\\.\\-:"};
    final_path[4] = letter;
    LPCSTR wszPath = final_path;

    this->app = new Application(wszPath);
    ui->menunone->setTitle(path);

    QFuture<void> b =  QtConcurrent::run(this->app, &Application::setBootTable, ui->bootTable);
    b.waitForFinished();

    QFuture<void> c =  QtConcurrent::run(this->app, &Application::setFolderTable, ui->rootTable);
    c.waitForFinished();

    QFuture<void> f =  QtConcurrent::run(this->app, &Application::setFatTable, ui->fatTable);
    f.waitForFinished();

    //spinner->stop(); //test

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
    if(this->app == nullptr){
        return;
    }

    WaitingSpinnerWidget* spinner = new WaitingSpinnerWidget(this);

    spinner->setRoundness(70.0);
    spinner->setMinimumTrailOpacity(15.0);
    spinner->setTrailFadePercentage(70.0);
    spinner->setNumberOfLines(12);
    spinner->setLineLength(10);
    spinner->setLineWidth(5);
    spinner->setInnerRadius(10);
    spinner->setRevolutionsPerSecond(1);
    spinner->setColor(QColor(81, 4, 71));

    spinner->start();

    this->app->setBootTable(ui->bootTable);
    //QThread *thread = QThread::create(this->call_set_fat( ui->fatTable));
    this->app->setFatTable(ui->fatTable);
    this->app->setFolderTable(ui->rootTable);
    this->app->pointer = 0;
    ui->rootTable->update();
    ui->fatTable->update();

    spinner->stop();

    MainWindow::setStyleSheet("QMainWindow{background-color: grey}");
    auto mTimer = new QTimer(this);
    mTimer->setSingleShot(true);
    connect(mTimer, SIGNAL(timeout()), SLOT(setWhite()));
    mTimer->start(125);
}

void MainWindow::call_set_fat(QTableWidget *qtable){
    this->app->setFatTable(qtable);
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
    std::cout << "---------------------" << std::endl;
    if(ui->rootTable->item(row,0)->background() == Qt::gray){
         std::cout << (ui->rootTable->item(row, 0)->text().toStdString().substr(0, 2)) << std::endl;
        if(ui->rootTable->item(row, 0)->text().toStdString().substr(0, 2) == ".."){
           app->pointer --;
           std::cout << "pointer1 " << app->pointer << std::endl;
           ui->tabWidget->setTabText(2, QString::fromStdString((app->folders[app->pointer])));
        }
        else
        if(ui->rootTable->item(row, 0)->text().toStdString().substr(0, 2) != ". "){
           app->pointer ++;
           std::cout << "pointer2 " << app->pointer << std::endl;
           app->folders[app->pointer] = ui->rootTable->item(row, 0)->text().toStdString();
           ui->tabWidget->setTabText(2, ui->rootTable->item(row, 0)->text());
        }
        std::cout << "is equal " << (app->folders[app->pointer].substr(0, 2) == "..") << std::endl;
        std::cout << "element " << app->folders[app->pointer] << std::endl;
        //std::cout << (app->folders[pointer].compare("..")) << std::endl;
        //std::cout << ui->rootTable->item(row, 0)->text().toStdString() << std::endl;
        this->app->setFolderTable(ui->rootTable, ui->rootTable->item(row, 3)->text().toInt(), 10);
        ui->rootTable->scrollToTop();
        ui->fatTable->update();        
    }
}

void MainWindow::on_fatTable_cellClicked(int row, int column)
{
    int limit = 2000;
    std::function<bool(int,int)> forward;   
    forward = [&](int r, int c) {
        //std::cout << ui->fatTable->item(r, c)->text().toInt() << ' ' << r*10+c << std::endl;
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
            limit --;
            std::cout << limit << std::endl;
            if(limit == 0)
                return false;
            return (forward(row_nxt, clmn_nxt));
        }
    };

    forward(row, column);

    int cur_row = row;
    int cur_col = column;
    limit = 1000;
    for(int i=row; i>=0; i--)
        for(int j=9; j>=0; j--){
            //std::cout << ui->fatTable->item(i, j)->text().toInt() << ' ' << cur_row*10+cur_col << std::endl;
            std::cout << limit << std::endl;
            if(ui->fatTable->item(i, j)->text().toInt() == cur_row*10+cur_col){
                ui->fatTable->item(i, j)->setSelected(true);
                cur_row = i;
                cur_col = j;
                limit --;
                if(limit==0)
                   i=0;

            }

        }
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
    //std::cout<<"lolkek"<<std::endl;
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
    //std::cout<<"lolkek"<<std::endl;
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
