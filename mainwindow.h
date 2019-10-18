#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <application.h>
//#include <fatAPI.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    Application* app;
    ~MainWindow();

private slots:
    //void on_reloadAction_triggered();

    void on_actionopen_triggered();

    void set_disk(QString path);

    //void on_selectButton_clicked();

    //void on_infoButton_clicked();

    //void on_updateButton_clicked();

    void on_actionupdate_triggered();

    void on_actioninfo_triggered();

    void on_rootTable_cellDoubleClicked(int row, int column);

    void on_fatTable_cellClicked(int row, int column);

    void on_fatTable_cellActivated(int row, int column);

    void customMenuRequested(const QPoint& pos);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
