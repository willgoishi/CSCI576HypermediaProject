#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myplayer.h"

#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QMediaMetaData>
#include <QtWidgets>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Add placeholder items to combox dropdown
    ui->comboBox->addItem("Doctor");
    ui->comboBox->addItem("Dinosaur");
    ui->comboBox->addItem("Dinosaur2");

    // Create players
    MyPlayer* player1 = new MyPlayer("C:/Users/Webber Wang/Downloads/AIFilmOne.avi", 1);
    this->layout()->addWidget(player1->m_videoWidget);

    MyPlayer* player2 = new MyPlayer("C:/Users/Webber Wang/Downloads/AIFilmTwo.avi", 2);
    this->layout()->addWidget(player2->m_videoWidget);

    // Connect sliders to function
    connect(ui->horizontalSliderLeft, SIGNAL(valueChanged(int)), this, SLOT(on_sliderLeft_changed()));
    connect(ui->horizontalSliderRight, SIGNAL(valueChanged(int)), this, SLOT(on_sliderRight_changed()));

    connect(ui->importPrimaryButton, SIGNAL(clicked()), player1, SLOT(import()));
    connect(ui->importSecondaryButton, SIGNAL(clicked()), player2, SLOT(import()));


    /*
     *Needs to be placed in the correct function
     *
     *Example Code
     *
     * QGraphicsScene* scene = new QGraphicsScene();
    QGraphicsView* view = new QGraphicsView(scene);
    QGraphicsPixmapItem* image = new QGraphicsPixmapItem(QPixmap::fromImage("QImage will go here"));

    scene->addItem(image);
    view->show();*/


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
