#include "interface/mainwindow.h"
#include "ui_mainwindow.h"
#include <QRadioButton>
#include <QHBoxLayout>
#include <QList>
#include <QVector>
#include <QLabel>
#include <QDebug>
#include <QTimer>
#include "emulator/kyiv.h"
#include "emulator/asm_disasm.h"
#include <unistd.h>
#include <QSlider>
#include <QSpinBox>
#include <QGroupBox>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QDir>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

/* Window constructor
 * Initialize
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    /* Some basic initializations */
    setMinimumSize(1600, 800);
    setWindowTitle("ЕОМ \"Київ\"");
    scrollArea = new QScrollArea(this);
    widget = new QWidget;
    kyivLay = new QVBoxLayout();
    panelAndCode = new QHBoxLayout();
    mainLay = new QVBoxLayout();
    setCentralWidget(scrollArea);

    /* Some language fixes, needed for asm/disasm section */
    QLocale curLocale(QLocale("C"));
    QLocale::setDefault(curLocale);
    std::setlocale(LC_ALL, "C");

    /* Init all panels */
    InitSignalPanel();
    InitControlPanel();
    InitROMPanel();
    InitASMDisASMPanel();
    InitProgramsPanel();
    InitPunchCardsPanel();
    InitDrumsPanel();

    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    panelAndCode->addLayout(mainLay);
    kyivLay->addLayout(panelAndCode);
    widget->setLayout(kyivLay);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(widget);
//    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitSignalPanel()
{
    for (size_t row = 0; row < 7; ++row) {
        QVector<QRadioButton*> rowButtons;
        int end = 0;
        switch (row) {
            case 0: end = 43; break;
            case 1: end = 42; break;
            case 2: [[fallthrough]];
            case 3: end = 38; break;
            case 4: [[fallthrough]];
            case 5: end = 41; break;
            case 6: end = 38; break;
            default: break;
        }

        for (size_t i = 0; i < end; ++i) {
            auto *radioRowButton = new QRadioButton();
            radioRowButton->setAutoExclusive(false);
            radioRowButton->setEnabled(false);
            radioRowButton->setChecked(false);
            rowButtons.push_back(radioRowButton);
        }
        machine.signalization.push_back(rowButtons);
    }

    /*          FIRST ROW       */
    auto *row = new QHBoxLayout();
    for (size_t i = 0; i < 43; ++i) {
        auto *radioLay = new QVBoxLayout();
        auto *text = new QLabel();
        if (i == 0) { text->setText("ЗнР2"); }
        else if (i == 1) { text->setText("Зн2С"); }
        else if (i == 2) { text->setText("Зн1С"); }
        else { text->setText(QString::number(43 - i)); }
        radioLay->addWidget(machine.signalization[0][i]);
        radioLay->addWidget(text);
        row->addItem(radioLay);
    }

    auto *rowName = new QLabel();
    rowName->setText("См");
    row->addWidget(rowName);

    mainLay->addLayout(row);

    /* SECOND ROW */
    row = new QHBoxLayout();
    auto *radioLay = new QVBoxLayout();
    auto *text = new QLabel();
    radioLay->addWidget(machine.signalization[1][0]);
    radioLay->addWidget(new QLabel("+  -"));
    row->addItem(radioLay);


    radioLay = new QVBoxLayout();

    radioLay->addWidget(new QLabel("ЗнЧ"));
    auto *text2 = new QLabel();
    radioLay->addWidget(new QLabel());
    row->addItem(radioLay);

    QList<QString> texts{"СпК", "Ср1", "Ср2", "|-|", "Ц+", "X", "⮾", ":", "Л->", "v", "^", "Ср3", "≅", "В1", "", "", "", "П3"};

    for (int i = 10; i > 0; i--) {
        texts.append(QString::number(i));
    }
    for (int i = 4; i > 0; i--) {
        texts.append("");
    }

    texts.append({"ива1", "вз1", "ива2", "вз2", "икоп", "ива3", "вз3", "ива4", "вз4"});

    for (int i = 0; i < 41; i++) {
        radioLay = new QVBoxLayout();
        text = new QLabel();
        text->setText(texts[i]);
        radioLay->addWidget(machine.signalization[1][i+1]);
        radioLay->addWidget(text);
        row->addItem(radioLay);
    }

    row->addWidget(new QLabel("РЧ")); // add row name

    mainLay -> addLayout(row);

    /* THIRD ROW */
    row = new QHBoxLayout();
    QList<QString> texts3 {"B2", "Печ", "Мб", "ЗМБЧ", "МbП", "НГО", "ОГО", "УПП", "УПЧ", "Прв", "Ост", "Ф", "н", "", "", ""};

    for (int i = 0; i < 16; i++) {
        radioLay = new QVBoxLayout();
        text = new QLabel();
        text->setText(texts3[i]);
        radioLay->addWidget(machine.signalization[2][i]);
        radioLay->addWidget(text);
        radioLay->addWidget(machine.signalization[3][i]);
        row->addItem(radioLay);
    }

    radioLay = new QVBoxLayout();

    auto *empty = new QLabel();
    empty->setText("");

    radioLay->addWidget(new QLabel("Тр"));
    radioLay->addWidget(new QLabel("КОп"));
    radioLay-> addWidget(empty);
    row->addItem(radioLay);

    radioLay = new QVBoxLayout();

    radioLay->addWidget(new QLabel("Выб"));
    radioLay->addWidget(machine.signalization[2][16]);
    radioLay->addWidget(empty);
    row->addItem(radioLay);

    texts3.clear();
    texts3.append("ПЗ");
    for (int i = 10; i > 0; i--) {
        texts3.append(QString::number(i));
    }

    for (int i = 0; i < 11; i++) {
        radioLay = new QVBoxLayout();
        radioLay->addWidget(machine.signalization[2][17+i]);
        radioLay->addWidget(new QLabel(texts3[i]));
        radioLay->addWidget(machine.signalization[3][16+i]);
        row->addItem(radioLay);
    }

    radioLay = new QVBoxLayout();
    radioLay->addWidget(new QLabel("PA"));
    radioLay->addWidget(empty);
    radioLay->addWidget(new QLabel("PB"));
    row->addItem(radioLay);

    radioLay = new QVBoxLayout();
    radioLay->addWidget(new QLabel("ТрПр"));
    radioLay->addWidget(machine.signalization[2][28]);
    radioLay->addWidget(empty);
    row->addItem(radioLay);

    radioLay = new QVBoxLayout();
    radioLay->addWidget(empty);
    radioLay->addWidget(new QLabel("ПЗ"));
    radioLay->addWidget(machine.signalization[3][27]);
    row->addItem(radioLay);

    for (int i = 10; i > 1; i--) {
        radioLay = new QVBoxLayout();
        radioLay->addWidget(machine.signalization[2][39-i]);
        radioLay->addWidget(new QLabel(QString::number(i)));
        radioLay->addWidget(machine.signalization[3][38-i]);
        row->addItem(radioLay);
    }

    radioLay = new QVBoxLayout();
    radioLay->addWidget(new QLabel("ЦУ"));
    radioLay->addWidget(new QLabel("1"));
    radioLay->addWidget(machine.signalization[3][37]);
    row->addItem(radioLay);
    row->addWidget(new QLabel("УВК"));

    mainLay->addLayout(row);

    /* FORTH ROW */
    row = new QHBoxLayout();

    for (int i = 5; i > 0; i--) {
        radioLay = new QVBoxLayout();
        radioLay->addWidget(new QLabel(QString::number(i)));
        radioLay->addWidget(machine.signalization[4][5-i]);
        row->addItem(radioLay);
    }

    row->addWidget(new QLabel("АОп"));

    QList<QString> texts4 {"ГО", "ПЗ"};
    for (int i = 10; i > 0; i--) {
        texts4.append(QString::number(i));
    }

    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < texts4.length(); i++) {
            radioLay = new QVBoxLayout();
            radioLay->addWidget(new QLabel(texts4[i]));
            radioLay->addWidget(machine.signalization[4][5 + 12*j + i]);
            row->addItem(radioLay);
        }
        if (j == 0) {
            row->addWidget(new QLabel("IA"));
        } else if (j == 1) {
            row->addWidget(new QLabel("IIA"));
        } else {
            radioLay = new QVBoxLayout();
            radioLay->addWidget(new QLabel("IIIA"));
            radioLay->addWidget(new QLabel("PK"));
            row->addItem(radioLay);
        }
    }

    mainLay->addLayout(row);

    /* FIFTH ROW */

    row = new QHBoxLayout();

    radioLay = new QVBoxLayout();
    radioLay->addWidget(new QLabel("Зн"));
    radioLay->addWidget(machine.signalization[5][0]);
    row->addItem(radioLay);

    for (int i = 40; i > 0; i--) {
        radioLay = new QVBoxLayout();
        radioLay->addWidget(new QLabel(QString::number(i)));
        radioLay->addWidget(machine.signalization[5][41-i]);
        row->addItem(radioLay);
    }

    radioLay = new QVBoxLayout();

    radioLay->addWidget(empty);
    radioLay->addWidget(new QLabel("РП"));
    row->addItem(radioLay);

    mainLay->addLayout(row);

    /* SIX ROW */

    row = new QHBoxLayout();

    for (int i = 0; i < 10; i++) {
        radioLay = new QVBoxLayout();
        text = new QLabel();
        if (i == 0) { text->setText("XP"); }
        else if (i == 2) { text->setText("ИМБ"); }
        else if (i == 7) {  text->setText("РТр"); }
        radioLay->addWidget(text);
        radioLay->addWidget(machine.signalization[6][i]);
        row->addItem(radioLay);
    }

    row->addWidget(new QLabel(" "));

    for (int i = 0; i < 10; i++) {
        radioLay = new QVBoxLayout();
        text = new QLabel();
        QString label = "";
        if (i == 4) { label = "HM"; }
        text->setText(label);
        radioLay->addWidget(text);
        radioLay->addWidget(machine.signalization[6][10+i]);
        row->addItem(radioLay);
    }

    row->addWidget(new QLabel("СчА"));

    radioLay = new QVBoxLayout();
    radioLay->addWidget(new QLabel("Зп"));
    radioLay->addWidget(machine.signalization[6][20]);
    row->addItem(radioLay);

    row->addWidget(new QLabel(" "));

    radioLay = new QVBoxLayout();
    radioLay->addWidget(new QLabel("ЧТ"));
    radioLay->addWidget(machine.signalization[6][21]);
    row->addItem(radioLay);

    row->addWidget(new QLabel(" "));

    for (int i = 0; i < 10; i++) {
        radioLay = new QVBoxLayout();
        text = new QLabel();
        QString label = "";
        if (i == 3) { label = "КР";}
        else if (i == 8) { label = "Сч"; }
        else if (i == 9) { label = "МИ"; }
        text->setText(label);
        radioLay->addWidget(text);
        radioLay->addWidget(machine.signalization[6][22+i]);
        row->addItem(radioLay);
    }

    row->addWidget(new QLabel("РЦ"));

    for (int i = 0; i < 6; i++) {
        radioLay = new QVBoxLayout();
        text = new QLabel();
        QString label = "";
        if (i == 0) { label = "Нач"; }
        else if (i == 1) { label = "Авт"; }
        else if (i == 2) { label = "Гот"; }
        text->setText(label);
        radioLay->addWidget(text);
        radioLay->addWidget(machine.signalization[6][32+i]);
        row->addItem(radioLay);
    }

    row->addWidget(new QLabel(" "));

    mainLay->addLayout(row);
}

void MainWindow::InitControlPanel() {
    for (int row = 0; row < 3; row++) {
        QVector<QRadioButton*> rowButtons;
        word_t currentROMValue = machine.kmem.read_memory(1536 + row);
        std::ostringstream oct;
        oct << std::setw(14) << std::setfill('0') << std::oct << currentROMValue;
        QString octal = QString::fromStdString(oct.str());
        int binary_octal[42];
        binary_octal[0] = 0;
        int firstNum = octal[0].digitValue();
        binary_octal[1] = firstNum % 2;
        binary_octal[2] = (firstNum / 2) % 2;
        for (int i = 1; i < 14; i++) {
            int curr = octal[i].digitValue();
            binary_octal[2+3*i] = curr % 2;
            curr = curr / 2;
            binary_octal[1+3*i] = curr % 2;
            curr = curr / 2;
            binary_octal[3*i] = curr % 2;
        }
        for (int i = 0; i < 42; i++) {
            auto *radioButton = new QRadioButton();
            radioButton->setAutoExclusive(false);
            rowButtons.push_back(radioButton);
            rowButtons[i]->setEnabled(false);
            rowButtons[i]->setChecked(binary_octal[i]);
        }
        upControl.push_back(rowButtons);
    }

    QString space = "";
    for (int i = 0; i < 57; i++) {
        space += " ";
    }

    auto *upControlBox = new QVBoxLayout();

    upControlBox->addWidget(new QLabel(space));
    upControlBox->addWidget(new QLabel("Панель управління"));

    for (int i = 0; i < 3; i++) {
        auto *rowBox = new QHBoxLayout();
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

    mainLay->addLayout(upControlBox);

    /*      PART WITH BUTTONS AND SLIDERS        */

    for (int row = 0; row < 2; row++) {
        QVector<QRadioButton*> rowButtons;
        int end = (row == 0) ? 42 : 22;
        for (int i = 0; i < end; i++) {
            auto *radioButton = new QRadioButton();
            radioButton->setAutoExclusive(false);
            rowButtons.push_back(radioButton);
            rowButtons[i]->setEnabled(row);
        }
        lowControl.push_back(rowButtons);
    }

    auto *lowControlBox = new QVBoxLayout();

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

    QVector<QString> nearStrings{" ", "АОп", "А1", "А2", "А3"};

    auto *rowButtons = new QHBoxLayout();
    int currNear = 0;
    for (int j = 0; j < 42; j++) {
        auto *elem = new QVBoxLayout();
        elem->addWidget(new QLabel(buttonNames[j]));
        elem->addWidget(lowControl[0][j]);
        rowButtons->addItem(elem);
        if (j == 0 || j == 5 || j == 17 || j == 29 || j == 41) {
            rowButtons->addWidget(new QLabel(nearStrings[currNear]));
            currNear++;
        }
    }

    lowControlBox->addItem(rowButtons);

    auto *lowestRowButtons = new QHBoxLayout();
    auto *el1 = new QVBoxLayout();
    el1->addWidget(new QLabel("Тр"));
    lowControl[1][0]->setChecked(true);
    el1->addWidget(lowControl[1][0]);
    connect(lowControl[1][0], &QRadioButton::clicked, this, &MainWindow::OnAvarOstTrigButtonClicked);
    el1->addWidget(new QLabel("авар"));
    el1->addWidget(new QLabel("ОСТ"));
    lowestRowButtons->addItem(el1);
    lowestRowButtons->addWidget(lowControl[1][0]);
    lowestRowButtons->addWidget(new QLabel(" "));

    auto *el2 = new QVBoxLayout();
    el2->addWidget(new QLabel("HP"));
    el2->addWidget(lowControl[1][1]);
    connect(lowControl[1][1], &QRadioButton::clicked, this, &MainWindow::OnBlockCButtonClicked);
    el2->addWidget(new QLabel("бл"));
    el2->addWidget(new QLabel("УВК"));
    lowestRowButtons->addItem(el2);

    lowestRowButtons->addWidget(new QLabel(" "));
    auto *el3 = new QVBoxLayout();
    el3->addWidget(new QLabel("Пч"));
    el3->addWidget(lowControl[1][2]);
    el3->addWidget(new QLabel(" "));
    el3->addWidget(new QLabel(" "));
    lowestRowButtons->addItem(el3);

    lowestRowButtons->addWidget(new QLabel(" "));
    lowestRowButtons->addWidget(lowControl[1][3]);
    connect(lowControl[1][3], &QRadioButton::clicked, this, &MainWindow::OnBlockAButtonClicked);
    lowestRowButtons->addWidget(new QLabel("УОСчА "));

    auto *el4 = new QVBoxLayout();
    el4->addWidget(new QLabel("Переск"));
    el4->addWidget(lowControl[1][4]);
    connect(lowControl[1][4], &QRadioButton::clicked, this, &MainWindow::OnBlockOstanovButtonClicked);
    el4->addWidget(new QLabel("ОСТ"));
    lowestRowButtons->addItem(el4);

    lowestRowButtons->addWidget(new QLabel("    "));

    auto *el5 = new QVBoxLayout();
    auto *slider1 = new QSlider(Qt::Vertical);
    slider1->setValue(3);
    machine.signalization[6][33] -> setChecked(1);
    connect(slider1, &QSlider::valueChanged, this, &::MainWindow::OnSetSpeedClicked);
    lowestRowButtons->addWidget(slider1);
    slider1->setRange(1, 3);
    el5->addWidget(new QLabel("|Авт"));
    el5->addWidget(new QLabel("|Цикл"));
    el5->addWidget(new QLabel("|Такт"));
    lowestRowButtons->addItem(el5);
    lowestRowButtons->addWidget(new QLabel(" "));

    auto *el6 = new QVBoxLayout();

    slider2->setValue(3);
    connect(slider2, &QSlider::valueChanged, this, &MainWindow::OnSetFromPultButtonClicked);
    lowestRowButtons->addWidget(slider2);
    slider2->setRange(1, 3);
    el6->addWidget(new QLabel("|Норм раб"));
    el6->addWidget(new QLabel("|Ввод"));
    el6->addWidget(new QLabel("|УСТ с ПУ"));
    lowestRowButtons->addItem(el6);

    lowestRowButtons->addWidget(new QLabel(" "));

    auto *el7 = new QVBoxLayout();
    el7->addWidget(new QLabel("Подгот "));
    connect(lowControl[1][7], &QRadioButton::clicked, this, &::MainWindow::OnuoButtonClicked);
    el7->addWidget(lowControl[1][7]);
    el7->addWidget(new QLabel("УО"));
    lowestRowButtons->addItem(el7);

    auto *el8 = new QVBoxLayout();
    el8->addWidget(new QLabel("запуска"));
    connect(lowControl[1][8], &QRadioButton::clicked, this, &::MainWindow::OnKButtonClicked);
    el8->addWidget(lowControl[1][8]);
    el8->addWidget(new QLabel("К"));
    lowestRowButtons->addItem(el8);

    auto *el9 = new QVBoxLayout();
    el9->addWidget(new QLabel(" "));
    connect(lowControl[1][9], &QRadioButton::clicked, this, &::MainWindow::OnOstanovButtonClicked);
    el9->addWidget(lowControl[1][9]);
    el9->addWidget(new QLabel("Останов"));
    lowestRowButtons->addItem(el9);

    auto *el10 = new QVBoxLayout();
    auto *slider3 = new QSlider(Qt::Vertical);
    slider3->setValue(3);
    connect(slider3, &QSlider::valueChanged, this, &MainWindow::OnOstanovPultButtonClicked);
    lowestRowButtons->addWidget(slider3);
    slider3->setRange(1, 3);
    el10->addWidget(new QLabel("|Нейтральное"));
    el10->addWidget(new QLabel("|По N команды"));
    el10->addWidget(new QLabel("|По III адресу"));
    lowestRowButtons->addItem(el10);

    lowestRowButtons->addWidget(new QLabel(" "));

    auto *el11 = new QVBoxLayout();
    el11->addWidget(new QLabel(" "));
    el11->addWidget(lowControl[1][10]);
    el11->addWidget(new QLabel(" "));
    lowestRowButtons->addItem(el11);

    auto *el12 = new QVBoxLayout();
    el12->addWidget(new QLabel("П3"));
    el12->addWidget(lowControl[1][11]);
    el12->addWidget(new QLabel(" "));
    lowestRowButtons->addItem(el12);

    for (int i = 12; i < 22; i++) {
        auto *lastButtons = new QVBoxLayout();
        lastButtons->addWidget(new QLabel(buttonNames[i+20]));
        lastButtons->addWidget(lowControl[1][i]);
        lastButtons->addWidget(new QLabel(" "));
        lowestRowButtons->addItem(lastButtons);
    }
    lowestRowButtons->addWidget(new QLabel(" "));

    lowControlBox->addItem(lowestRowButtons);

    mainLay -> addLayout(lowControlBox);
}

void MainWindow::InitROMPanel() {
    mainLay->addWidget(new QLabel("\n\nROM"));

    for (int row = 0; row < 8; row++) {
        QVector<QRadioButton*> rowButtons;
        word_t currentROMValue = machine.kmem.read_memory(1536 + row);
        std::ostringstream oct;
        oct << std::setw(14) << std::setfill('0') << std::oct << currentROMValue;
        QString octal = QString::fromStdString(oct.str());
        int binary_octal[41];
        int firstNum = octal[0].digitValue();
        binary_octal[0] = firstNum % 2;
        binary_octal[1] = (firstNum / 2) % 2;
        for (int i = 1; i < 14; i++) {
            int curr = octal[i].digitValue();
            binary_octal[3*i+1] = curr % 2;
            curr = curr / 2;
            binary_octal[3*i] = curr % 2;
            curr = curr / 2;
            binary_octal[3*i-1] = curr % 2;
        }
        for (int i = 0; i < 41; i++) {
            auto *radioButton = new QRadioButton();
            radioButton->setAutoExclusive(false);
            rowButtons.push_back(radioButton);
            rowButtons[i]->setEnabled(true);
            rowButtons[i]->setChecked(binary_octal[i]);
        }
        rom_buttons.push_back(rowButtons);
    }

    auto *romBox = new QVBoxLayout();

    for (int i = 0; i < 8; i++) {

        auto *rowBox = new QHBoxLayout();
        rowBox->addWidget(new QLabel(QString::number(3000 + i) + " "));

        for (int j = 0; j < 41; j++) {
            rowBox->addWidget(rom_buttons[i][j]);
            if (j == 1 || (j % 3 == 1)) {
                rowBox->addWidget(new QLabel(" "));
            }
        }
        romBox->addItem(rowBox);
        romBox->addWidget(new QLabel(""));
    }

    romBox->addWidget(new QLabel(""));

    auto *saveROMBtn = new QPushButton();
    saveROMBtn->setText("SAVE ROM");
    connect(saveROMBtn, SIGNAL(clicked()), this, SLOT(OnSaveROMButtonClicked()));

    romBox->addWidget(saveROMBtn);

    mainLay -> addLayout(romBox);
}

void MainWindow::InitASMDisASMPanel() {
    mainLay->addWidget(new QLabel("\n\nDISASM | ASM"));

    auto* asmDisasmBox = new QGroupBox();

    auto* asmDisasmLayout = new QVBoxLayout();
    asmDisasmBox->setLayout(asmDisasmLayout);

    auto* textEditLayout = new QHBoxLayout();
    auto* asmButtonsLayout = new QHBoxLayout();

    auto *codeArea = new QTextEdit();
    codeArea->setFontFamily("Ubuntu");


    auto *asCodeArea = new QTextEdit();
    asCodeArea->setFontFamily("Ubuntu");
    asCodeArea->setReadOnly(true);
    asCodeArea->setMinimumSize(150, 300);

    auto *assemblyBtn = new QPushButton();
    assemblyBtn->setText("ASSEMBLY");
    connect(assemblyBtn, SIGNAL(clicked()), this, SLOT(OnAssemblyButtonClicked()));

    auto *openFileBtn = new QPushButton();
    openFileBtn->setText("OPEN FILE");
    connect(openFileBtn, SIGNAL(clicked()), this, SLOT(OnOpenDisasmFileButtonClicked()));

    textEditLayout->addWidget(codeArea);
    textEditLayout->addWidget(asCodeArea);
    asmDisasmLayout->addLayout(textEditLayout);
    asmButtonsLayout->addWidget(openFileBtn);
    asmButtonsLayout->addWidget(assemblyBtn);
    asmDisasmLayout->addLayout(asmButtonsLayout);

    mainLay->addWidget(asmDisasmBox);
}

void MainWindow::InitProgramsPanel() {
    mainLay->addWidget(new QLabel("\n\nPROGRAMS"));

    auto* programsLayout = new QHBoxLayout();

    komytatorsBtn = new QPushButton("KOMYTATORS");
    rungeKuttaBtn = new QPushButton("RUNGE KUTTA");
    sqrtAndFriendsBtn = new QPushButton("SQRT AND FRIENDS");

    connect(komytatorsBtn,  SIGNAL(clicked()), this, SLOT(OnKomytatorsButtonClicked()));
    connect(rungeKuttaBtn,  SIGNAL(clicked()), this, SLOT(OnRungeKuttaButtonClicked()));
    connect(sqrtAndFriendsBtn,  SIGNAL(clicked()), this, SLOT(OnSqrtAndFriendsButtonClicked()));

    programsLayout->addWidget(komytatorsBtn);
    programsLayout->addWidget(rungeKuttaBtn);
    programsLayout->addWidget(sqrtAndFriendsBtn);

    mainLay->addLayout(programsLayout);
}

void MainWindow::InitPunchCardsPanel() {
    mainLay->addWidget(new QLabel("\n\nPUNCHCARDS"));

    auto* layout = new QVBoxLayout();

    auto* scrollAreaCards = new QScrollArea(this);
    scrollAreaCards->setMinimumSize(1400, 600);
    scrollAreaCards->setWidgetResizable(true);

    toolBox = new QToolBox();
    scrollAreaCards->setWidget(toolBox);

    auto *addBtn = new QPushButton("ADD PUNCH CARD");
    connect(addBtn,  SIGNAL(clicked()), this, SLOT(OnAddButtonClicked()));

    auto *delBtn = new QPushButton("DELETE PUNCH CARD");
    connect(delBtn,  SIGNAL(clicked()), this, SLOT(OnDelButtonClicked()));

    auto *savePerfoDataBth = new QPushButton("SAVE PERFO DATA");
    connect(savePerfoDataBth,  SIGNAL(clicked()), this, SLOT(OnSavePerfoDataButtonClicked()));

    auto* perfoButtons = new QHBoxLayout();
    perfoButtons->addWidget(addBtn);
    perfoButtons->addWidget(delBtn);
    perfoButtons->addWidget(savePerfoDataBth);

    layout->addWidget(scrollAreaCards);
    layout->addLayout(perfoButtons);

    mainLay->addLayout(layout);
}

void MainWindow::InitDrumsPanel() {
    mainLay->addWidget(new QLabel("\n\nDRUMS"));

    panelAndCode ->addLayout(mainLay);

    auto *drums = new QHBoxLayout();

    /* Creating same widgets for 3 drums */
    for (int j = 0; j < 3; j++) {
        auto *lay = new QVBoxLayout();
        for (size_t i = 0; i < 3288; i++) {
            auto num = new QLineEdit();
            num->setReadOnly(true);
            num->setText("0");
            num->setMinimumSize(400, 30);
            num->setFont(QFont( "Timers" , 15 ,  QFont::Bold));
            switch (j) {
                case 0: machine.drum1.push_back(num); break;
                case 1: machine.drum2.push_back(num); break;
                case 2: machine.drum3.push_back(num); break;
                default:
                    break;
            }
            lay->addWidget(num);
        }

        // creating scrolling area for each of 3 drums
        auto *sc = new QScrollArea();

        // creating widgets area for each of 3 drums
        auto *widget1=new QWidget;

        widget1->setLayout(lay);
        sc->setWidget(widget1);
        sc->setMinimumSize(400, 450);
        drums->addWidget(sc);
    }

    kyivLay->addLayout(panelAndCode);
    kyivLay->addLayout(drums);
}

void MainWindow::OnSaveButtonClicked() {
    std::vector<std::string> argv;

    auto *inputLine = sender()->parent()->findChild<QLineEdit *>();
    auto *grid = sender()->parent()->findChild<QGridLayout *>();

    boost::split(argv, inputLine->text().toStdString(), boost::is_any_of(" "), boost::algorithm::token_compress_off);

    for (size_t i = 0; i < 80; i++) {
        for (size_t j = 0; j < 12; j++) {
            auto *label = qobject_cast<QLabel *>(grid->itemAtPosition(j, i)->widget());
            label->setStyleSheet("QLabel{font-family: 'Agency FB';}");
        }
    }

    int count = 0;
    std::string perfoLine;
    for(auto& part : argv){
        if (part.empty())
            continue;

        if (count != 0)
            perfoLine += ' ';

        if (part.front() == '-') {
            if (count == 79 || part.size() == 1) {
                break;
            }
            auto *label = qobject_cast<QLabel *>(grid->itemAtPosition(0, count)->widget());
            part.erase(0,1);
            label->setStyleSheet("QLabel { background-color : black; }");
            count++;
            perfoLine += '-';
        }

        for(auto elem : part) {
            if (!std::isdigit(elem)) {
                break;
            }
            int value = (int)elem - '0';
            auto *label = qobject_cast<QLabel *>(grid->itemAtPosition(value + 2, count)->widget());
            label->setStyleSheet("QLabel { background-color : black; }");
            perfoLine += elem;
            count++;

            if (count > 79) {
                break;
            }
        }
        count++;
        if (count > 79) {
            break;
        }
    }
    toolBox->setItemText(toolBox->currentIndex(), QString::fromStdString(perfoLine));
    inputLine->setText(QString::fromStdString(perfoLine));
}

void MainWindow::OnAddButtonClicked() {
    auto *box = new QGroupBox();
    box->setFixedHeight(400);

    auto* tempLayout = new QVBoxLayout(box);

    auto *grid = new QGridLayout();
    tempLayout->addLayout(grid);
    toolBox->insertItem(toolBox->currentIndex() + 1, box, "NEW");

    for (size_t i = 0; i < 12; i++) {
        for (size_t j = 0; j < 80; j++) {
            auto *number = new QLabel();
            if (i > 1) {
                number->setText(QString::number(i-2));
                number->setStyleSheet("QLabel{font-family: 'Agency FB';}");
            }
            grid->addWidget(number, i, j);
        }
    }

    auto *inputLayout = new QHBoxLayout();
    tempLayout->addLayout(inputLayout);

    auto *saveButton = new QPushButton("SAVE");
    connect(saveButton,  SIGNAL(clicked()), this, SLOT(on_saveButton_clicked()));
    saveButton->setFixedSize(QSize(60, 30));

    inputLayout->addWidget(new QLineEdit());
    inputLayout->addWidget(saveButton);

    toolBox->setMinimumHeight(407 + toolBox->count() * 31);
}

void MainWindow::OnDelButtonClicked() {
    auto index = toolBox->currentIndex();
    if (index < 0) {
        qDebug() << "unable to delete, punch card don't find";
    } else {
        toolBox->removeItem(index);
        if (toolBox->count() == 0)
            toolBox->setMinimumHeight(600);
        else
            toolBox->setMinimumHeight(407 + toolBox->count() * 31);
    }
}

void MainWindow::OnSavePerfoDataButtonClicked() {
    std::ofstream perfoFile;
    perfoFile.open("../mem/punched_tape.txt", std::ios_base::out | std::ios_base::trunc);
    if (!perfoFile.is_open())
        std::cout << "Unable to open perfo file";

    for (size_t i = 0; i < toolBox->count(); i++) {
        std::vector<std::string> argv;
        auto line = toolBox->itemText(i).toStdString();
        if (line == "NEW" || line == "") {
            continue;
        }
        boost::split(argv, line, boost::is_any_of(" "), boost::algorithm::token_compress_off);
        for(auto& part : argv){
            perfoFile << part << '\n';
        }
    }
    perfoFile.close();
}

void MainWindow::OnOpenDisasmFileButtonClicked() {
    auto *codeArea = qobject_cast<QTextEdit *>(sender()->parent()->children().at(1));
    auto file_name = QFileDialog::getOpenFileName(this, "Open a file", QDir::currentPath());
    QFile openFile(file_name);
    if (openFile.open(QFile::ReadOnly)) {
        QTextStream ReadFile(&openFile);
        codeArea->setText(ReadFile.readAll());
    } else {
        std::cout << "Unable to open file";
    }
    openFile.close();
}

void MainWindow::OnAssemblyButtonClicked() {
    std::cout << "start" << std::endl;
    auto *codeArea = qobject_cast<QTextEdit *>(sender()->parent()->children().at(1));
    if (codeArea->toPlainText().isEmpty()) {
        return;
    }
    std::cout << "start1" << std::endl;

    auto *asCodeArea = qobject_cast<QTextEdit *>(sender()->parent()->children().at(2));

    QFile tempDisasm("../tempDisasm.txt");
    std::cout << "start2" << std::endl;

    if (tempDisasm.open(QFile::ReadWrite | QFile::Truncate)) {
        QTextStream stream(&tempDisasm);
        stream << codeArea->toPlainText();
        tempDisasm.flush();
    } else {
        std::cout << "Unable to open disasm file";
    }
    tempDisasm.close();
    std::cout << "start4" << std::endl;

    Assembly as(std::ref(machine));
    as.read_file("../tempDisasm.txt", false);
    std::cout << "start5" << std::endl;

    QFile tempAsm(outputf);
    if (tempAsm.open(QFile::ReadOnly)) {
        QTextStream ReadFile(&tempAsm);
        asCodeArea->setText(ReadFile.readAll());
    } else {
        std::cout << "Unable to open asm file";
    }
    tempAsm.close();
    size_t start = 03600;
    QFuture<void> future = QtConcurrent::run(as, &Assembly::execute);
}

void MainWindow::OnSaveROMButtonClicked() {
    for (int row = 0; row < 8; row++) {
        std::string octal;
        int currDigit = rom_buttons[row][0]->isChecked() * 2 + rom_buttons[row][1]->isChecked();
        octal += std::to_string(currDigit);
        for (int i = 1; i < 14; i++) {
            int octalDigit =
                    rom_buttons[row][i * 3 - 1]->isChecked() * 4 + rom_buttons[row][i * 3]->isChecked() * 2 +
                    rom_buttons[row][1 + i * 3]->isChecked();
            octal += std::to_string(octalDigit);
        }
        unsigned long long value = std::stoull(octal, 0, 8);
        qDebug() << "command octal string: " << QString::fromStdString(octal);
        qDebug() << "command octal int: " << QString::number(value);

        qDebug() << "ROM cell before: " << QString::number(machine.kmem.read_memory(03000 + row));
        machine.kmem.write_rom(03000 + row, value);
        qDebug() << "ROM cell after: " << QString::number(machine.kmem.read_memory(03000 + row));
    }
}

void MainWindow::OnuoButtonClicked() {
    qDebug() << "занулення усіх регістрів";
    auto *btn = qobject_cast<QRadioButton *>(sender());
    btn->setChecked(true);
    machine.null_registers();
    qDebug() << "P register =" << machine.P_reg << "\nA register =" << machine.A_reg;
    QTimer::singleShot(400,[btn]()->void{btn->setChecked(false);});
}

void MainWindow::OnOstanovButtonClicked() {
    qDebug() << "T register before ostanov =" << machine.T_reg;
    qDebug() << "останов";
    auto *btn = qobject_cast<QRadioButton *>(sender());
    btn->setChecked(true);
    machine.opcode_flow_control(word_to_addr3(0), 0'33, word_to_addr3(0));
    qDebug() << "T register after ostanov =" << machine.T_reg;
    QTimer::singleShot(400,[btn]()->void{btn->setChecked(false);});
}

void MainWindow::OnBlockCButtonClicked() {
    qDebug() << "С block before =" << machine.C_block;
    auto *btn = qobject_cast<QRadioButton *>(sender());
    bool toBlock = btn->isChecked();
    if (toBlock) {
        qDebug() << "блокування лічильника команд";
    } else {
        qDebug() << "розблокування лічильника команд";
    }
    machine.C_block = toBlock;
    qDebug() << "С block after =" << machine.C_block;
}

void MainWindow::OnBlockAButtonClicked() {
    qDebug() << "A block before =" << machine.A_block;
    auto *btn = qobject_cast<QRadioButton *>(sender());
    bool toBlock = btn->isChecked();
    if (toBlock) {
        qDebug() << "блокування регістрів повернення, циклів та модифікації адрес";
    } else {
        qDebug() << "розблокування регістрів повернення, циклів та модифікації адрес";
    }
    machine.A_block = toBlock;
    qDebug() << "A block after =" << machine.A_block;
}

void MainWindow::OnSetFromPultButtonClicked() {
    auto *sld = qobject_cast<QSlider *>(sender());
    qDebug() << sld->value();
    int pos = sld->value();
    if (pos == 2) {
        for (int row = 0; row < 3; row++) {
            for (int i = 0; i < 42; i++) {
                upControl[row][i]->setEnabled(true);
            }
        }
        for (int i = 0; i < 42; i++) {
            lowControl[0][i]->setEnabled(true);
        }
    } else if (pos == 1) {
        for (int row = 0; row < 3; row++) {
            for (int i = 0; i < 42; i++) {
                upControl[row][i]->setEnabled(false);
            }
        }
        for (int i = 0; i < 42; i++) {
            lowControl[0][i]->setEnabled(false);
        }

        for (int row = 0; row < 3; row++) {
            if (upControl[row][0]->isChecked()) { // tumbler for pult work is on
                std::string octal;
                int currDigit = upControl[row][1]->isChecked() * 2 + upControl[row][2]->isChecked();
                octal += std::to_string(currDigit);
                for (int i = 1; i < 14; i++) {
                    int octalDigit =
                            upControl[row][i * 3]->isChecked() * 4 + upControl[row][1 + i * 3]->isChecked() * 2 +
                            upControl[row][2 + i * 3]->isChecked();
                    octal += std::to_string(octalDigit);
                }
                unsigned long long value = std::stoull(octal, 0, 10);
                qDebug() << "command octal string: " << QString::fromStdString(octal);
                qDebug() << "command octal int: " << QString::number(value);

                qDebug() << "ROM cell before: " << QString::number(machine.kmem.read_memory(1536+row));
                machine.kmem.norm_rom[row] = false;
                machine.kmem.writable_rom_pu[row] = value; // upper 3 rows: if the first button is enabled,
                // replace ROM cell with this value
                qDebug() << "ROM cell after: " << QString::number(machine.kmem.read_memory(1536+row));
            } else {
                machine.kmem.norm_rom[row] = true;
            }
        }
        if (lowControl[0][0]->isChecked()) {
            // from binary to octal
            std::string octal;
            int currDigit = lowControl[0][1]->isChecked() * 2 + lowControl[0][2]->isChecked();
            octal += std::to_string(currDigit);
            for (int i = 1; i < 14; i++) {
                int octalDigit =
                        lowControl[0][i * 3]->isChecked() * 4 + lowControl[0][1 + i * 3]->isChecked() * 2 +
                        lowControl[0][2 + i * 3]->isChecked();
                octal += std::to_string(octalDigit);
            }
            qDebug() << "command octal string: " << QString::fromStdString(octal);
            unsigned long long value = std::stoull(octal, 0, 8);
            qDebug() << "command decimal int: " << QString::number(value);
            qDebug() << "kmem[7]: " << QString::number(machine.kmem.read_memory(7));
            machine.K_reg = value; // execute command from pult
            machine.execute_opcode(false);
            qDebug() << "kmem[7]: " << QString::number(machine.kmem.read_memory(7)); // може мати попереднє значення, бо цей прінт виконується раніше за виконання команди
        }
    } else if (pos == 3) {
        for (int row = 0; row < 3; row++) {
            for (int i = 0; i < 42; i++) {
                upControl[row][i]->setEnabled(0);
            }
        }
        for (int i = 0; i < 42; i++) {
            lowControl[0][i]->setEnabled(0);
        }
        machine.execute_opcode();
    }
}

void MainWindow::OnOstanovPultButtonClicked() {
    auto *sld = qobject_cast<QSlider *>(sender());
    qDebug() << sld->value();
    int pos = sld->value();
    machine.ostanov_state = pos;
    machine.B_tumb = pos;
    if (pos == 1) { // by third address
        machine.ostanovCommand = 0;
        std::string octal;
        for (int i = 0; i < 4; i++) {
            int octalDigit =
                    lowControl[1][10 + i * 3]->isChecked() * 4 + lowControl[1][11 + i * 3]->isChecked() * 2 +
                    lowControl[1][12 + i * 3]->isChecked();
            octal += std::to_string(octalDigit);
        }
        qDebug() << "address octal string: " << QString::fromStdString(octal);
        addr_t value = std::stoull(octal, 0, 10);
        qDebug() << "address decimal int: " << QString::number(value);
        machine.ostanovAdrr_t = value;
    } else if (pos == 2) { // by command number
        machine.ostanovAdrr_t = 0;
        std::string octal;
        int currDigit = lowControl[0][1]->isChecked() * 2 + lowControl[0][2]->isChecked();
        octal += std::to_string(currDigit);
        for (int i = 1; i < 14; i++) {
            int octalDigit =
                    lowControl[0][i * 3]->isChecked() * 4 + lowControl[0][1 + i * 3]->isChecked() * 2 +
                    lowControl[0][2 + i * 3]->isChecked();
            octal += std::to_string(octalDigit);
        }
        qDebug() << "command octal string: " << QString::fromStdString(octal);
        unsigned long long value = std::stoull(octal, 0, 8);
        qDebug() << "command decimal int: " << QString::number(value);
        machine.ostanovCommand = value;
    } else if (pos == 3) { // neutral
        machine.ostanovCommand = 0;
        machine.ostanovAdrr_t = 0;
    }
}

void MainWindow::OnBlockOstanovButtonClicked() {
    auto *btn = qobject_cast<QRadioButton *>(sender());
    qDebug() << "Ostanov block before: " << machine.Ostanov_block;
    bool machineOstanovBlock = btn->isChecked();
    if (machineOstanovBlock) {
        qDebug() << "заборона останову";
    } else {
        qDebug() << "розблокування заборони останову";
    }
    machine.Ostanov_block = machineOstanovBlock;
    qDebug() << "Ostanov block after: " << machine.Ostanov_block;
}

void MainWindow::OnAvarOstTrigButtonClicked() {
    auto *btn = qobject_cast<QRadioButton *>(sender());
    qDebug() << "Ostanov trigger before: " << machine.T_tr;
    bool machineTTr = btn->isChecked();
    if (machineTTr) {
        qDebug() << "ввімкнення тригеру останову";
    } else {
        qDebug() << "вимкнення тригеру останову";
    }
    machine.T_tr = machineTTr;
    qDebug() << "Ostanov trigger after: " << machine.T_tr;
}

void MainWindow::OnSetSpeedClicked() {
    auto *sld = qobject_cast<QSlider *>(sender());
    qDebug() << sld->value();
    int pos = sld->value();
    machine.work_state = pos;
    if (pos == 3) {
        machine.signalization[6][33] -> setChecked(1);
    } else {
        machine.signalization[6][33] -> setChecked(0);
    }
}

void MainWindow::OnKButtonClicked() {
    auto *btn = qobject_cast<QRadioButton *>(sender());
    btn->setChecked(true);
    QTimer::singleShot(400,[btn]()->void{btn->setChecked(true);});
    bool exec_mode = (slider2->value() > 1);
    if (machine.work_state == 3) {
        while (machine.execute_opcode(exec_mode)) {
            std::cout << "C reg: " << machine.C_reg << std::endl;
        }
    } else if (machine.work_state == 2) {
        machine.execute_opcode(exec_mode);
    } else if (machine.work_state == 1) {
        if (!machine.T_reg) {
            addr_t addr;
            switch (curr_takt) {
                case 0: {
                    opcode_t opcode = machine.get_opcode(exec_mode);
                    addr = 0;
                    std::cout << "opcode: " << opcode << std::endl;
                    break;
                }
                case 1: {
                    addr = machine.get_addr1();
                    std::cout << "Addr 1: " << addr << std::endl;
                    break;
                }
                case 2: {
                    addr = machine.get_addr2();
                    std::cout << "Addr 2: " << addr << std::endl;
                    break;
                }
                case 3: {
                    addr = machine.get_addr3();
                    std::cout << "Addr 3: " << addr << std::endl;
                    machine.execute_opcode(exec_mode);
                    curr_takt = -1;
                    break;
                }
            }
            curr_takt++;
            std::bitset<11> b(addr);
            std::string addr_str = b.to_string();
            for (size_t i = 0; i < 11; i++) {
                bool val = (addr_str[i] - '0');
                machine.signalization[2][i+17]->setChecked(val);
            }
        }
    }
}

void MainWindow::OnKomytatorsButtonClicked() {
    /* Just some visual, so user can see which lib is chosen */
    komytatorsBtn->setDown(true);
    rungeKuttaBtn->setDown(false);
    sqrtAndFriendsBtn->setDown(false);
    /* Write program to memory */
    qDebug() << "03300 before: " << machine.kmem.read_memory(03300);
    qDebug() << "03600 before: " << machine.kmem.read_memory(03600);
    ReadProgram("../libs/komytators.txt");
    qDebug() << "03300 after: " << machine.kmem.read_memory(03300);
    qDebug() << "03600 after: " << machine.kmem.read_memory(03600);
}

void MainWindow::OnRungeKuttaButtonClicked() {
    rungeKuttaBtn->setDown(true);
    komytatorsBtn->setDown(false);
    sqrtAndFriendsBtn->setDown(false);
    qDebug() << "03300 before: " << machine.kmem.read_memory(03300);
    qDebug() << "03600 before: " << machine.kmem.read_memory(03600);
    ReadProgram("../libs/runge_kutta.txt");
    qDebug() << "03300 after: " << machine.kmem.read_memory(03300);
    qDebug() << "03600 after: " << machine.kmem.read_memory(03600);
}

void MainWindow::OnSqrtAndFriendsButtonClicked() {
    sqrtAndFriendsBtn->setDown(true);
    rungeKuttaBtn->setDown(false);
    komytatorsBtn->setDown(false);
    qDebug() << "03300 before: " << machine.kmem.read_memory(03300);
    qDebug() << "03600 before: " << machine.kmem.read_memory(03600);
    ReadProgram("../libs/sqrt_and_friends.txt");
    qDebug() << "03300 after: " << machine.kmem.read_memory(03300);
    qDebug() << "03600 after: " << machine.kmem.read_memory(03600);
}

void MainWindow::ReadProgram(const std::string& filepath) {
    std::ifstream infile(filepath);
    std::string line;
    while (std::getline(infile, line)) {
        line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
        std::ostringstream str;
        str << std::oct << line;
        std::string data = str.str();
        std::string address = data.substr(0, 4);
        data = data.substr(4);
        if (data.size() != 13 && data.size() != 14) {
            continue;
        }
        if (data.size() != 14) {
            data.insert(0, "0");
        }
        machine.kmem.write_rom(std::stoi(address, 0, 8), std::stol(data, 0, 8));
    }
}
