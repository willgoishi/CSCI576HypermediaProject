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
#include "image.h"

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

    graphicsView = new MyGraphicsView(this);
    graphicsView->setGeometry(10, 180, 10 + 352, 180 + 288);
    graphicsView->resize(352, 288);
    this->layout()->addWidget(graphicsView);


    graphicsView2 = new MyGraphicsView(this);
    graphicsView2->setGeometry(352 + 20, 180, 10 + 352, 180 + 288);
    graphicsView2->resize(352, 288);
    this->layout()->addWidget(graphicsView2);

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

    //MyPlayer* player2 = new MyPlayer("C:/Users/Webber Wang/Downloads/AIFilmTwo.avi", 2);
    //this->layout()->addWidget(player2->m_videoWidget);


    // Connect sliders to function
    connect(ui->horizontalSliderLeft, SIGNAL(valueChanged(int)), this, SLOT(on_sliderLeft_changed()));
    connect(ui->horizontalSliderRight, SIGNAL(valueChanged(int)), this, SLOT(on_sliderRight_changed()));

    connect(ui->importPrimaryButton, SIGNAL(clicked()), this, SLOT(import()));
    connect(ui->importSecondaryButton, SIGNAL(clicked()), this, SLOT(import()));
    resize(960, 640);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sliderLeft_changed()
{
    qDebug() << "on slider left changed";
    if(PrimaryUploadImages.size() == 0) { return; }

    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(*PrimaryUploadImages.at(ui->horizontalSliderLeft->value())));
    graphicsView->scene->addItem(item);
    ui->left_sliderLabel->setText(QString::number(ui->horizontalSliderLeft->value()));
}

void MainWindow::on_sliderRight_changed() {
    qDebug() << "on slider right changed";
    if(SecondaryUploadImages.size() == 0) { return; }

    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(*SecondaryUploadImages.at(ui->horizontalSliderRight->value())));
    graphicsView2->scene->addItem(item);
    ui->right_sliderLabel->setText(QString::number(ui->horizontalSliderRight->value()));
}

void MainWindow::import()
{
    QFileDialog upload(this);
    upload.setFileMode(QFileDialog::Directory);
    upload.setAcceptMode(QFileDialog::AcceptOpen);
    upload.setWindowTitle("Import Files");
    upload.exec();

    std::string directoryPath = upload.directory().path().toStdString();
    QDir directory(upload.directory().path());
    QStringList imageFileNames = directory.entryList(QStringList() << "*.rgb", QDir::Files);

    int count = 0;

    QString caller = QObject::sender()->objectName();
    qDebug() << caller;
    foreach(QString filename, imageFileNames)
    {
        if(count > 200)
        {
            continue;
        }

        QString filePath = upload.directory().path() + "/" + filename;
        QFile file(filePath);
        if(!file.open(QFile::ReadOnly))
        {
            qDebug() << "Issue occurred in opening the file";
            return;
        }

        QByteArray data = file.readAll();
        file.flush();
        file.close();

        QImage img(352,288,QImage::Format_RGB32);
        QRgb *pixels = reinterpret_cast<QRgb*>(img.bits());
        for(size_t i = 0; i < 352*288; i++)
        {
            uchar pixel1 = data[i];
            uchar pixel2 = data[i+1];
            uchar pixel3 = data[i+2];
            pixels[i] = qRgb(pixel1,pixel2,pixel3);
        }

        caller == "importPrimaryButton" ? PrimaryUploadImages.push_back(new QImage(img)) : SecondaryUploadImages.push_back(new QImage(img));
        count++;
    }
    qDebug() << PrimaryUploadImages.size();
    qDebug() << SecondaryUploadImages.size();
    qDebug() << "DONE Loading!";
}
