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
    ui->selectLinks->addItem("Doctor");
    ui->selectLinks->addItem("Dinosaur");

    // Slider
    ui->horizontalSliderLeft->setTickInterval(20);
    ui->horizontalSliderLeft->setSingleStep(1);

    MyGraphicsView* graphicsView = new MyGraphicsView(this);
    graphicsView->setGeometry(10, 180, 10 + 352, 180 + 288);
    graphicsView->resize(352, 288);
    this->layout()->addWidget(graphicsView);

    // Create players
//    MyPlayer* player1 = new MyPlayer("C:/Users/Webber Wang/Downloads/AIFilmOne.avi", 1);
//    this->layout()->addWidget(player1->m_videoWidget);

//    MyPlayer* player2 = new MyPlayer("C:/Users/Webber Wang/Downloads/AIFilmTwo.avi", 2);
//    this->layout()->addWidget(player2->m_videoWidget);

    /*
    Load frames from video, find frame count & create the vector of nodes (1 frame per node).

    For now we create an empty
    */




    // Connect sliders to function
    connect(ui->horizontalSliderLeft, SIGNAL(valueChanged(int)), this, SLOT(on_sliderLeft_changed(int)));
    connect(ui->horizontalSliderRight, SIGNAL(valueChanged(int)), this, SLOT(on_sliderRight_changed(int)));
    connect(ui->createNewHyperLink, SIGNAL(released()), this, SLOT(on_createNewHyperlink_clicked()));
    connect(ui->selectLinks, SIGNAL(activated(int)), this, SLOT(on_selectLinks_changed(int)));

    resize(960, 640);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sliderLeft_changed(int value) {
    qDebug() << "on slider left changed: " << value;
    ui->frameCountLeft->setText(QString::number(value));
}

void MainWindow::on_sliderRight_changed(int value) {
    qDebug() << "on slider right changed " << value;
    ui->frameCountRight->setText(QString::number(value));
}

void MainWindow::on_createNewHyperlink_clicked() {
    qDebug() << "on button clicked";

    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Link label:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty())
        ui->selectLinks->addItem(text);
        qDebug() << "Confirmed:" << text;
}

void MainWindow::on_selectLinks_changed(int index) {
    qDebug() << "Link index selected: " << index;

}
