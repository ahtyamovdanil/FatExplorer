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

    void on_actionopen_triggered();

    void set_disk(QString path);

    void on_actionupdate_triggered();

    void on_actioninfo_triggered();

    void on_rootTable_cellDoubleClicked(int row, int column);

    void on_fatTable_cellClicked(int row, int column);

    void on_fatTable_cellActivated(int row, int column);

    void fatCustomMenuRequested(const QPoint& pos);

    void bootCustomMenuRequested(const QPoint& pos);

    void rootCustomMenuRequested(const QPoint& pos);

    void setWhite();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
