#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myplayer.h"
#include "mygraphicsview.h"

#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QMediaMetaData>
#include <QLabel>
#include "drawarea.h"
#include "mywidget.h"

#include <QtWidgets>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QDebug>
#include <QFrame>
#include <QPainter>
#include <QPoint>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Add placeholder items to combox dropdown
    ui->comboBox->addItem("Doctor");
    ui->comboBox->addItem("Dinosaur");
    ui->comboBox->addItem("Dinosaur2");

//    QPainter painter(this);
//    painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine, Qt::RoundCap,
//                        Qt::RoundJoin));
//    QRect rect = QRect(QPoint(10, 180), QPoint(10 + 50, 180 + 50));
//    painter.drawRect(rect);

    // Create left layout
//    QWidget* m_myWidget = new QWidget;
//    m_myWidget->setGeometry(0, 200, 200, 200);
//    m_myWidget->setStyleSheet("background-color:black;");
//    m_myWidget->show();
//    this->layout()->addWidget(m_myWidget);


//    this->layout()->addWidget(m_myWidget);

    MyGraphicsView* graphicsView = new MyGraphicsView(this);
    graphicsView->setGeometry(10, 180, 10 + 352, 180 + 288);
    graphicsView->resize(352, 288);
    this->layout()->addWidget(graphicsView);


//    DrawArea* drawArea = new DrawArea;
//    drawArea->setGeometry(10, 180, 10 + 352, 180 + 288);
//    drawArea->setStyleSheet("border:1px solid rgb(0, 255, 0);background-color: rgba(0, 0, 0, 0);");
//    drawArea->resize(352, 288/2);

//    this->layout()->addWidget(drawArea);

    // Create players
    MyPlayer* player1 = new MyPlayer("C:/Users/Webber Wang/Downloads/AIFilmOne.avi", 1);
//    ui->gridLayout->addWidget(player1->m_videoWidget);
//    this->layout()->addWidget(player1->m_videoWidget);

//    QLabel* myLabel = new QLabel("My Label");
//    QWidget* myWidget = new mywidget;
//    myWidget->setGeometry(10, 180, 352, 288);
//    this->layout()->addWidget(myWidget);

//    ui->gridLayout->addWidget(myWidget, 0, 0);
//    ui->gridLayout->addWidget(myLabel, 0, 0);

    MyPlayer* player2 = new MyPlayer("C:/Users/Webber Wang/Downloads/AIFilmTwo.avi", 2);
    this->layout()->addWidget(player2->m_videoWidget);


    // Connect sliders to function
    connect(ui->horizontalSliderLeft, SIGNAL(valueChanged(int)), this, SLOT(on_sliderLeft_changed()));
    connect(ui->horizontalSliderRight, SIGNAL(valueChanged(int)), this, SLOT(on_sliderRight_changed()));

    connect(ui->importPrimaryButton, SIGNAL(clicked()), player1, SLOT(import()));
    connect(ui->importSecondaryButton, SIGNAL(clicked()), player2, SLOT(import()));
    resize(960, 640);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sliderLeft_changed() {
    qDebug() << "on slider left changed";
}

void MainWindow::on_sliderRight_changed() {
    qDebug() << "on slider right changed";
}

void MainWindow::on_importPrimaryButton_clicked()
{
    qDebug() << "import primary";
}

void MainWindow::on_importSecondaryButton_clicked()
{
    qDebug() << "import secondary";
}
