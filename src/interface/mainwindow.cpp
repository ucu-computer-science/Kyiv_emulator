#include "interface/mainwindow.h"
#include "ui_mainwindow.h"
#include <QRadioButton>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QList>
#include <QVector>
#include <QLabel>
#include <QDebug>
#include <QTimer>
#include "emulator/kyiv.h"
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Kyiv_t machine;

    setWindowTitle("Панель сигналізації & управління");
    auto *scrollArea=new QScrollArea(this);
    auto *widget=new QWidget;
    auto *mainLay = new QVBoxLayout();

    QVector<QVector<QRadioButton*>> signalization;

    for (int row = 0; row < 7; row++) {
        QVector<QRadioButton*> rowButtons;
        int end = 0;
        if (row == 0) {
            end = 43;
        } else if (row == 1) {
            end = 42;
        } else if (row == 2) {
            end = 38;
        } else if (row == 3) {
            end = 38;
        } else if (row == 4) {
            end = 41;
        } else if (row == 5) {
            end = 41;
        } else if (row == 6) {
            end = 38;
        }

        for (int i = 0; i < end; i++) {
            QRadioButton *radioRowButton = new QRadioButton();
            radioRowButton->setAutoExclusive(0);
            radioRowButton->setEnabled(0);
            radioRowButton->setChecked(0);
            rowButtons.push_back(radioRowButton);
        }
        signalization.push_back(rowButtons);
    }


    QHBoxLayout * row1 = new QHBoxLayout();
    for (int i = 0; i < 43; i++) {
        QVBoxLayout *radioLay = new QVBoxLayout();
        QLabel *text = new QLabel();
        QString label;
        if (i == 0) {
            label = "ЗнР2";
        } else if (i == 1) {
            label = "Зн2С";
        } else if (i == 2) {
            label = "Зн1С";
        } else {
            label = QString::number(43 - i);
        }
        text -> setText(label);
        radioLay->addWidget(signalization[0][i]);
        radioLay->addWidget(text);
        row1 -> addItem(radioLay);
    }

    QLabel *rowName = new QLabel();
    rowName -> setText("См");
    row1 -> addWidget(rowName);

    mainLay -> addLayout(row1);

    QHBoxLayout * row2 = new QHBoxLayout();
    QVBoxLayout * radioLay1 = new QVBoxLayout();
    QLabel *text = new QLabel();
    text -> setText("+");
    radioLay1->addWidget(signalization[1][0]);
    radioLay1->addWidget(text);
    row2 -> addItem(radioLay1);


    QVBoxLayout * radioLay2 = new QVBoxLayout();

    QLabel *text2 = new QLabel();
    text2 -> setText("ЗнЧ");
    radioLay2->addWidget(text2);
    QLabel *text3 = new QLabel();
    text3 -> setText("-");
    radioLay2 -> addWidget(text2);
    radioLay2 -> addWidget(text3);
    row2 -> addItem(radioLay2);

    QList<QString> texts;
    texts = {"СпК", "Ср1", "Ср2", "Н", "Ц", "", "/xor", "", "Л->", "V", "Л", "Ср3", "~", "В", "", "", "П3"};

    for (int i = 10; i > 0; i--) {
        texts.append(QString::number(i));
    }
    for (int i = 5; i > 0; i--) {
        texts.append("");
    }

    texts.append("ива1");
    texts.append("вз1");
    texts.append("ива2");
    texts.append("вз2");
    texts.append("икол");
    texts.append("ива3");
    texts.append("вз3");
    texts.append("ива4");
    texts.append("вз4");

    for (int i = 0; i < 41; i++) {
        QVBoxLayout * radioLay = new QVBoxLayout();
        QLabel *text = new QLabel();
        QString label;
        label = texts[i];
        text -> setText(label);
        radioLay->addWidget(signalization[1][i+1]);
        radioLay->addWidget(text);
        row2 -> addItem(radioLay);
    }

    QLabel *row2Name = new QLabel();
    row2Name -> setText("РЧ");
    row2 -> addWidget(row2Name);

    mainLay -> addLayout(row2);

    QHBoxLayout * row3 = new QHBoxLayout();
    QList<QString> texts3;
    texts3 = {"B2", "Печ", "Мб", "ЗМБЧ", "МbП", "НГО", "ОГО", "УПП", "УПЧ", "Прв", "Ост", "Ф", "н", "", "", ""};

    for (int i = 0; i < 16; i++) {
        QVBoxLayout * radioLay = new QVBoxLayout();
        QLabel *text = new QLabel();
        QString label;
        label = texts3[i];
        text -> setText(label);
        radioLay->addWidget(signalization[2][i]);
        radioLay->addWidget(text);
        radioLay->addWidget(signalization[3][i]);
        row3 -> addItem(radioLay);
    }

    QVBoxLayout * radioLay3 = new QVBoxLayout();

    QLabel *empty = new QLabel();
    empty -> setText("");

    QLabel *text3_1 = new QLabel();
    text3_1 -> setText("Тр");
    QLabel *text3_2 = new QLabel();
    text3_2 -> setText("КОп");
    radioLay3 -> addWidget(text3_1);
    radioLay3 -> addWidget(text3_2);
    radioLay3 -> addWidget(empty);
    row3 -> addItem(radioLay3);

    QVBoxLayout * radioLay4 = new QVBoxLayout();

    QLabel *text3_3 = new QLabel();
    text3_3 -> setText("Выб");
    radioLay4 -> addWidget(text3_3);
    radioLay4->addWidget(signalization[2][16]);
    radioLay4 -> addWidget(empty);
    row3 -> addItem(radioLay4);


    texts3.append("ПЗ");
    for (int i = 10; i > 0; i--) {
        texts3.append(QString::number(i));
    }

    for (int i = 0; i < 11; i++) {
        QVBoxLayout  *radioLay = new QVBoxLayout();
        QLabel *text = new QLabel();
        QString label;
        label = texts3[i+16];
        text -> setText(label);
        radioLay->addWidget(signalization[2][17+i]);
        radioLay->addWidget(text);
        radioLay->addWidget(signalization[2][16+i]);
        row3 -> addItem(radioLay);
    }

    QVBoxLayout * radioLay4_1 = new QVBoxLayout();
    radioLay4_1->addWidget(new QLabel("PA"));
    radioLay4_1->addWidget(empty);
    radioLay4_1->addWidget(new QLabel("PB"));
    row3->addItem(radioLay4_1);

    QVBoxLayout * radioLay5 = new QVBoxLayout();

    QLabel *text3_5 = new QLabel();
    text3_5 -> setText("ТрПр");
    radioLay5 -> addWidget(text3_5);
    radioLay5->addWidget(signalization[2][28]);
    radioLay5->addWidget(empty);
    row3 -> addItem(radioLay5);

    QVBoxLayout * radioLay6 = new QVBoxLayout();

    QLabel *text3_6 = new QLabel();
    text3_6 -> setText("");
    QLabel *text3_7 = new QLabel();
    text3_7 -> setText("ПЗ");
    radioLay6 -> addWidget(text3_6);
    radioLay6 -> addWidget(text3_7);
    radioLay6 -> addWidget(signalization[3][27]);
    row3 -> addItem(radioLay6);

    for (int i = 10; i > 1; i--) {
        QVBoxLayout  *radioLay = new QVBoxLayout();
        QLabel *text = new QLabel();
        QString label;
        label = QString::number(i);
        text -> setText(label);
        radioLay->addWidget(signalization[2][39-i]);
        radioLay->addWidget(text);
        radioLay->addWidget(signalization[3][38-i]);
        row3 -> addItem(radioLay);
    }

    QVBoxLayout * radioLay7 = new QVBoxLayout();

    QLabel *text3_8 = new QLabel();
    text3_8 -> setText("ЦУ");
    QLabel *text3_9 = new QLabel();
    text3_9 -> setText("1");
    radioLay7 -> addWidget(text3_8);
    radioLay7 -> addWidget(text3_9);
    radioLay7 -> addWidget(signalization[3][37]);
    row3 -> addItem(radioLay7);
    row3->addWidget(new QLabel("УВК"));

    mainLay -> addLayout(row3);

    QHBoxLayout * row4 = new QHBoxLayout();

    for (int i = 5; i > 0; i--) {
        QVBoxLayout * radioLay = new QVBoxLayout();
        QLabel *text = new QLabel();
        QString label = QString::number(i);
        text -> setText(label);
        radioLay->addWidget(text);
        radioLay->addWidget(signalization[4][5-i]);
        row4 -> addItem(radioLay);
    }

    row4 -> addWidget(new QLabel("АОп"));

    QList<QString> texts4 = {"ГО", "ПЗ"};
    for (int i = 10; i > 0; i--) {
        texts4.append(QString::number(i));
    }

    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < texts4.length(); i++) {
            QVBoxLayout * radioLay = new QVBoxLayout();
            QLabel *text = new QLabel();
            QString label = texts4[i];
            text -> setText(label);
            radioLay->addWidget(text);
            radioLay->addWidget(signalization[4][5 + 12*j + i]);
            row4 -> addItem(radioLay);
        }
        if (j == 0) {
            row4 -> addWidget(new QLabel("IA"));
        } else if (j == 1) {
            row4 -> addWidget(new QLabel("IIA"));
        } else {
            QVBoxLayout * radioLay = new QVBoxLayout();
            radioLay -> addWidget(new QLabel("IIIA"));
            radioLay -> addWidget(new QLabel("PK"));
            row4 -> addItem(radioLay);
        }
    }

    mainLay -> addLayout(row4);

    QHBoxLayout * row5 = new QHBoxLayout();

    QVBoxLayout * radioLay5_1 = new QVBoxLayout();
    radioLay5_1 -> addWidget(new QLabel("Зн"));
    radioLay5_1 -> addWidget(signalization[5][0]);
    row5 -> addItem(radioLay5_1);

    for (int i = 40; i > 0; i--) {
        QVBoxLayout * radioLay = new QVBoxLayout();
        QLabel *text = new QLabel();
        QString label = QString::number(i);
        text -> setText(label);
        radioLay->addWidget(text);
        radioLay->addWidget(signalization[5][41-i]);
        row5 -> addItem(radioLay);
    }

    QVBoxLayout * radioLay5_3 = new QVBoxLayout();

    radioLay5_3 -> addWidget(new QLabel(""));
    radioLay5_3 -> addWidget(new QLabel("РП"));
    row5 -> addItem(radioLay5_3);

    mainLay -> addLayout(row5);

    QHBoxLayout * row6 = new QHBoxLayout();

    for (int i = 0; i < 10; i++) {
        QVBoxLayout * radioLay = new QVBoxLayout();
        QLabel *text = new QLabel();
        QString label = "";
        if (i == 0) {
            label = "XP";
        } else if (i == 2) {
            label = "ИМБ";
        } else if (i == 7) {
            label = "РТр";
        }
        text -> setText(label);
        radioLay->addWidget(text);
        radioLay->addWidget(signalization[6][i]);
        row6 -> addItem(radioLay);
    }

    row6->addWidget(new QLabel(" "));

    for (int i = 0; i < 10; i++) {
        QVBoxLayout * radioLay = new QVBoxLayout();
        QLabel *text = new QLabel();
        QString label = "";
        if (i == 4) {
            label = "HM";
        }
        text -> setText(label);
        radioLay->addWidget(text);
        radioLay->addWidget(signalization[6][10+i]);
        row6 -> addItem(radioLay);
    }

    row6->addWidget(new QLabel("СчА"));

    QVBoxLayout * radioLay6_1 = new QVBoxLayout();
    radioLay6_1 -> addWidget(new QLabel("Зн"));
    radioLay6_1 -> addWidget(signalization[6][20]);
    row6 -> addItem(radioLay6_1);

    row6->addWidget(new QLabel(" "));

    QVBoxLayout * radioLay6_2 = new QVBoxLayout();
    radioLay6_2 -> addWidget(new QLabel("ЧТ"));
    radioLay6_2 -> addWidget(signalization[6][21]);
    row6 -> addItem(radioLay6_2);

    row6->addWidget(new QLabel(" "));

    for (int i = 0; i < 10; i++) {
        QVBoxLayout * radioLay = new QVBoxLayout();
        QLabel *text = new QLabel();
        QString label = "";
        if (i == 3) {
            label = "КР";
        } else if (i == 8) {
            label = "Сч";
        } else if (i == 9) {
            label = "МИ";
        }
        text -> setText(label);
        radioLay->addWidget(text);
        radioLay->addWidget(signalization[6][22+i]);
        row6 -> addItem(radioLay);
    }

    row6->addWidget(new QLabel("РЦ"));

    for (int i = 0; i < 6; i++) {
        QVBoxLayout  *radioLay = new QVBoxLayout();
        QLabel *text = new QLabel();
        QString label = "";
        if (i == 0) {
            label = "Нач";
        } else if (i == 1) {
            label = "Авт";
        } else if (i == 2) {
            label = "Гот";
        }
        text -> setText(label);
        radioLay->addWidget(text);
        radioLay->addWidget(signalization[6][32+i]);
        row6 -> addItem(radioLay);
    }

    row6->addWidget(new QLabel(" "));

    mainLay -> addLayout(row6);






    /////////////////////////////////////////////////////////////////////////////////////



    QVector<QVector<QRadioButton*>> upControl;

    for (int row = 0; row < 3; row++) {
        QVector<QRadioButton*> rowButtons;
        for (int i = 0; i < 42; i++) {

            QRadioButton *radioButton = new QRadioButton(this);
            radioButton->setAutoExclusive(0);
            rowButtons.push_back(radioButton);
            rowButtons[i]->setEnabled(0);
            rowButtons[i]->setChecked(1);
        }
        upControl.push_back(rowButtons);
    }

//    for (int row = 0; row < 3; row++) {
//        for (int i = 0; i < 39; i++) {

//            upControl[row][i]->setEnabled(0);
//            upControl[row][i]->setChecked(1);
//        }
//    }

    QString space = "";
    for (int i = 0; i < 57; i++) {
        space += " ";
    }

    QVBoxLayout *upControlBox = new QVBoxLayout();

    upControlBox->addWidget(new QLabel(space));
    upControlBox->addWidget(new QLabel("Панель управління"));

    for (int i = 0; i < 3; i++) {

        QHBoxLayout *rowBox = new QHBoxLayout();
        for (int j = 0; j < 42; j++) {
            rowBox->addWidget(upControl[i][j]);
            if (j == 0 || (j % 3 == 2)) {
                rowBox->addWidget(new QLabel(" "));
            }
        }
        upControlBox->addItem(rowBox);
        upControlBox->addWidget(new QLabel(space));
    }

    upControlBox->addWidget(new QLabel(space));

    mainLay -> addLayout(upControlBox);

//    QFrame *line = new QFrame();
//    line->setObjectName(QString::fromUtf8("line"));
//    line->setGeometry(QRect(320, 150, 118, 3));
//    line->setFrameShape(QFrame::HLine);
//    line->setLineWidth(10);
//    line->setFrameShadow(QFrame::Sunken);

//    mainLay->addWidget(line);


    QVector<QVector<QRadioButton*>> lowControl;

    for (int row = 0; row < 2; row++) {
        QVector<QRadioButton*> rowButtons;
        int end = (row == 0) ? 42 : 22;
        for (int i = 0; i < end; i++) {
            QRadioButton *radioButton = new QRadioButton(this);
            radioButton->setAutoExclusive(0);
            rowButtons.push_back(radioButton);
            rowButtons[i]->setEnabled(1);
        }
        lowControl.push_back(rowButtons);
    }

    QVBoxLayout *lowControlBox = new QVBoxLayout();

    QVector<QString> buttonNames;

    buttonNames.push_back(" ");
    for (int i = 5; i > 0; i--) {
        buttonNames.push_back(QString::number(i));
    }
    for (int j = 0; j < 3; j++) {
        buttonNames.push_back("ГО");
        buttonNames.push_back("ПЗ");
        for (int i = 10; i > 0; i--) {
            buttonNames.push_back(QString::number(i));
        }
    }

    QVector<QString> nearStrings;
    nearStrings.push_back(" ");
    nearStrings.push_back("АОп");
    nearStrings.push_back("А1");
    nearStrings.push_back("А2");
    nearStrings.push_back("А3");

    QHBoxLayout *rowButtons = new QHBoxLayout();
    int currNear = 0;
    for (int j = 0; j < 42; j++) {
        QVBoxLayout *elem = new QVBoxLayout();
        elem->addWidget(new QLabel(buttonNames[j]));
        elem->addWidget(lowControl[0][j]);
        rowButtons->addItem(elem);
        if (j == 0 || j == 5 || j == 17 || j == 29 || j == 41) {
            rowButtons->addWidget(new QLabel(nearStrings[currNear]));
            currNear++;
        }
    }

    lowControlBox->addItem(rowButtons);

    QHBoxLayout *lowestRowButtons = new QHBoxLayout();
    lowestRowButtons->addWidget(lowControl[1][0]);
    lowestRowButtons->addWidget(new QLabel(" "));

    QVBoxLayout *el2 = new QVBoxLayout();
    el2->addWidget(new QLabel("HP"));
    el2->addWidget(lowControl[1][1]);
    el2->addWidget(new QLabel("бл"));
    el2->addWidget(new QLabel("УВК"));
    lowestRowButtons->addItem(el2);

    lowestRowButtons->addWidget(new QLabel(" "));
    QVBoxLayout *el3 = new QVBoxLayout();
    el3->addWidget(new QLabel("Пч"));
    el3->addWidget(lowControl[1][2]);
    el3->addWidget(new QLabel(" "));
    el3->addWidget(new QLabel(" "));
    lowestRowButtons->addItem(el3);

    lowestRowButtons->addWidget(new QLabel(" "));
    lowestRowButtons->addWidget(lowControl[1][3]);
    lowestRowButtons->addWidget(new QLabel("УОСчА "));

    QVBoxLayout *el4 = new QVBoxLayout();
    el4->addWidget(new QLabel("Переск"));
    el4->addWidget(lowControl[1][4]);
    el4->addWidget(new QLabel("ОСТ"));
    lowestRowButtons->addItem(el4);

    lowestRowButtons->addWidget(new QLabel("    "));

    lowestRowButtons->addWidget(lowControl[1][5]);
    QVBoxLayout *el5 = new QVBoxLayout();
    el5->addWidget(new QLabel("|Авт"));
    el5->addWidget(new QLabel("|Цикл"));
    el5->addWidget(new QLabel("|Такт"));
    lowestRowButtons->addItem(el5);

    lowestRowButtons->addWidget(new QLabel(" "));

    lowestRowButtons->addWidget(lowControl[1][6]);
    QVBoxLayout *el6 = new QVBoxLayout();
    el6->addWidget(new QLabel("|Норм раб"));
    el6->addWidget(new QLabel("|Ввод"));
    el6->addWidget(new QLabel("|УСТ с ПУ"));
    lowestRowButtons->addItem(el6);

    lowestRowButtons->addWidget(new QLabel(" "));

    QVBoxLayout *el7 = new QVBoxLayout();
    el7->addWidget(new QLabel("Подгот "));
    connect(lowControl[1][7], &QRadioButton::clicked, this, &MainWindow::on_uoButton_clicked);
    el7->addWidget(lowControl[1][7]);
    el7->addWidget(new QLabel("УО"));
    lowestRowButtons->addItem(el7);

    QVBoxLayout *el8 = new QVBoxLayout();
    el8->addWidget(new QLabel("запуска"));
    el8->addWidget(lowControl[1][8]);
    el8->addWidget(new QLabel("К"));
    lowestRowButtons->addItem(el8);

    QVBoxLayout *el9 = new QVBoxLayout();
    el9->addWidget(new QLabel("По N команды"));
    connect(lowControl[1][9], &QRadioButton::clicked, this, &MainWindow::on_ostanovButton_clicked);
    el9->addWidget(lowControl[1][9]);
    el9->addWidget(new QLabel("Останов"));
    lowestRowButtons->addItem(el9);

    QVBoxLayout *el10 = new QVBoxLayout();
    el10->addWidget(new QLabel(" "));
    el10->addWidget(lowControl[1][10]);
    el10->addWidget(new QLabel("По"));
    lowestRowButtons->addItem(el10);

    QVBoxLayout *el11 = new QVBoxLayout();
    el11->addWidget(new QLabel("П3"));
    el11->addWidget(lowControl[1][11]);
    el11->addWidget(new QLabel("III"));
    lowestRowButtons->addItem(el11);

    QVBoxLayout *addr = new QVBoxLayout();
    addr->addWidget(new QLabel("10  9  8"));
    QHBoxLayout *buttons = new QHBoxLayout();
    for (int i = 12; i < 15; i++) {
        buttons->addWidget(lowControl[1][i]);
        buttons->addWidget(new QLabel(" "));

    }
    addr->addItem(buttons);
    addr->addWidget(new QLabel("адресу"));
    lowestRowButtons->addItem(addr);

    for (int i = 15; i < 22; i++) {
        QVBoxLayout *lastButtons = new QVBoxLayout();
        lastButtons->addWidget(new QLabel(buttonNames[i+20]));
        lastButtons->addWidget(lowControl[1][i]);
        lastButtons->addWidget(new QLabel(" "));
        lowestRowButtons->addItem(lastButtons);
    }
    lowestRowButtons->addWidget(new QLabel(" "));

    lowControlBox->addItem(lowestRowButtons);

    mainLay -> addLayout(lowControlBox);


    widget->setLayout(mainLay);
    scrollArea->setWidget(widget);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->resize(1300,800);
    this->setFixedSize(1500, 1000);
}

void MainWindow::on_uoButton_clicked() {
    qDebug() << "занулення усіх регістрів";
    auto *btn = qobject_cast<QRadioButton *>(sender());
    btn->setChecked(1);
    machine.null_registers();
    qDebug() << "P register =" << machine.P_reg << "\nA register =" << machine.A_reg;
    QTimer::singleShot(400,[btn]()->void{btn->setChecked(0);});
}

void MainWindow::on_ostanovButton_clicked() {
    qDebug() << "T register before ostanov =" << machine.T_reg;
    qDebug() << "останов";
    auto *btn = qobject_cast<QRadioButton *>(sender());
    btn->setChecked(1);
    machine.opcode_flow_control(word_to_addr3(0), 0'33);
    qDebug() << "T register after ostanov =" << machine.T_reg;
    QTimer::singleShot(400,[btn]()->void{btn->setChecked(0);});
}

MainWindow::~MainWindow()
{
    delete ui;
}