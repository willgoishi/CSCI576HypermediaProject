#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myframe.h"
#include "myvideo.h"
#include "mygraphicsview.h"
#include "myplaylist.h"

#include <QVector>
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
//    ui->horizontalSliderLeft->setTickInterval(1);
    ui->horizontalSliderLeft->setSingleStep(1);
    ui->horizontalSliderLeft->setMaximum(100);


    // Create playlist to hold all the videos
    MyPlaylist playlist;

    int totalFrames = 100;

    // Create a video
    MyVideo video;

    // Generate frames for video
    for (int i = 0; i < totalFrames; i++) {

        // Constructor for video
        int frameCount = i;
        int videoId = 0;
        int linkId = 0;
        QRect boundary = QRect();

        // Create node & append
        MyFrame* frame = new MyFrame(frameCount);
        video.addFrame(frame);
    }

    playlist.addVideo(video);


    // Left video
    graphicsViewPrimary = new MyGraphicsView(playlist, 0, this);
    graphicsViewPrimary->setGeometry(10, 180, 10 + 352, 180 + 288);
    graphicsViewPrimary->resize(352, 288);
    this->layout()->addWidget(graphicsViewPrimary);

    // Right video
    graphicsViewSecondary = new MyGraphicsView(playlist, 1, this);
    graphicsViewSecondary->setGeometry(370, 180, 370 + 352, 180 + 288);
    graphicsViewSecondary->resize(352, 288);
    this->layout()->addWidget(graphicsViewSecondary);

    // Create players
//    MyPlayer* player1 = new MyPlayer("C:/Users/Webber Wang/Downloads/AIFilmOne.avi", 1);
//    this->layout()->addWidget(player1->m_videoWidget);

//    MyPlayer* player2 = new MyPlayer("C:/Users/Webber Wang/Downloads/AIFilmTwo.avi", 2);
//    this->layout()->addWidget(player2->m_videoWidget);

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
    qDebug() << "currentPrimaryFrame = " << value;
    ui->frameCountLeft->setText(QString::number(value));
    currentPrimaryFrame = value;
    graphicsViewPrimary->updateBoundary(currentPrimaryFrame);
}

void MainWindow::on_sliderRight_changed(int value) {
    qDebug() << "currentSecondaryFrame = " << value;
    ui->frameCountRight->setText(QString::number(value));
    currentSecondaryFrame = value;
    graphicsViewPrimary->updateBoundary(currentPrimaryFrame);
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
    currentLinkId = index;

    qDebug() << "Link index selected: " << index;
}
