#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "emulator/kyiv.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_uoButton_clicked();
    void on_ostanovButton_clicked();

private:
    Ui::MainWindow *ui;
    Kyiv_t machine;
};
#endif // MAINWINDOW_H
