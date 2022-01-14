#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRadioButton>
#include <QLCDNumber>
#include <QSlider>
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
    void on_openDisasmFileBtn_clicked();
    void on_assemblyBtn_clicked();
    void on_saveROMBtn_clicked();
    void on_uoButton_clicked();
    void on_ostanovButton_clicked();
    void on_blockC_clicked();
    void on_blockA_clicked();
    void onSetFromPult_clicked();
    void onOstanovPult_clicked();
    void on_blockOstanov_clicked();
    void on_avar_ost_trig_clicked();
    void onSetSpeed_clicked();
    void on_KButton_clicked();


private:
    Ui::MainWindow *ui;
    Kyiv_t machine;

    int curr_takt = 0;

    QVector<QVector<QRadioButton*>> upControl;
    QVector<QVector<QRadioButton*>> lowControl;
    QSlider *slider2 = new QSlider(Qt::Vertical);
    QVector<QVector<QRadioButton*>> rom_buttons;

};
#endif // MAINWINDOW_H
