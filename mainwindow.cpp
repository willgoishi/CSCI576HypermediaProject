#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myframe.h"
#include "mygraphicsview.h"

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
    int totalFrames = 101;

    // Create 1st video & add to playlist
    int videoId = 0;
    primaryVideo = new MyVideo(videoId);

    for (int i = 0; i < totalFrames; i++) {

        // Constructor for video
        int frameCount = i;

        // Create node & append
        MyFrame* frame = new MyFrame(frameCount, videoId);
        primaryVideo->addFrame(frame);
    }

    playlist.addVideo(primaryVideo);

    qDebug() << "primaryVideo size()" << primaryVideo->myVideo.size();

    // Create 2nd video & add to playlist
    videoId = 1;
    secondaryVideo = new MyVideo(videoId);

    for (int i = 0; i < totalFrames; i++) {

        // Constructor for video
        int frameCount = i;

        // Create node & append
        MyFrame* frame = new MyFrame(frameCount, videoId);
        secondaryVideo->addFrame(frame);
    }

    qDebug() << "secondaryVideo size()" << secondaryVideo->myVideo.size();

    playlist.addVideo(secondaryVideo);


    // Left video
    int width = 352;
    int height = 288;
    graphicsViewPrimary = new MyGraphicsView(playlist, PRIMARY_LOCATION, 0, this);
    graphicsViewPrimary->setGeometry(10, 180, 10 + 352, 180 + 288);
//    graphicsViewPrimary->fitInView(0, 0, 352, 288, Qt::KeepAspectRatio);
    graphicsViewPrimary->setFixedSize(width, height);
    graphicsViewPrimary->setSceneRect(0, 0, width, height);
    graphicsViewPrimary->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsViewPrimary->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsViewPrimary->resize(352, 288);
    this->layout()->addWidget(graphicsViewPrimary);

    // Right video
    graphicsViewSecondary = new MyGraphicsView(playlist, SECONDARY_LOCATION, 1, this);
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
//    connect(ui->connectVideo, SIGNAL(released()), this, SLOT(on_connectVideo_clicked()));
    connect(ui->selectLinks, SIGNAL(activated(int)), this, SLOT(on_selectLinks_changed(int)));
    connect(ui->selectLinks, SIGNAL(editTextChanged(QString)), this, SLOT(on_selectLinks_edited(QString)));
//    connect(ui->saveFile, SIGNAL(released()), this, SLOT(on_saveFile_clicked()));

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

    graphicsViewPrimary->updateCurrentLink(index);

    // Get 1st frame of link in video
    int frameIndex = primaryVideo->getFirstFrameWithBoundaryFromLinkId(currentLinkId);

    if (primaryVideo->hasFirstFrameWithBoundaryFromLinkId(currentLinkId)) {
        ui->horizontalSliderLeft->setValue(frameIndex);
    }

    qDebug() << "1st frame for linkId:" << currentLinkId << " is " << frameIndex;

    qDebug() << "Link index selected: " << index;
}

void MainWindow::on_selectLinks_edited(QString label)
{
    qDebug() << "edit label: "  << label;

    ui->selectLinks->setItemText(ui->selectLinks->currentIndex(), label);
}

void MainWindow::on_connectVideo_clicked()
{
    qDebug() << "on_connectVideo_clicked";

    // Get info of primary video
    int srcFrameCount = ui->horizontalSliderLeft->value();

    // Get info of secondary video
//    int targetVideoId = secondaryVideo.getVideoId();
    int targetFrameCount = ui->horizontalSliderRight->value();

    MyFrame* srcFrame = primaryVideo->getFrame(srcFrameCount);
    MyFrame* targetFrame = secondaryVideo->getFrame(targetFrameCount);

    srcFrame->addHyperlinkTarget(currentLinkId, targetFrame);

//    // Get data from playlist
    MyVideo* primaryVideo = playlist.getVideo(0);
    srcFrame = primaryVideo->getFrame(srcFrameCount);
    targetFrame = srcFrame->getHyperlinkTarget(currentLinkId);

    qDebug() << "Src link ID: " << currentLinkId;
    qDebug() << "Target frame: " << targetFrame->getFrameCount();
    qDebug() << "Target video: " << targetFrame->getVideoId();

}

void MainWindow::on_saveFile_clicked()
{
    qDebug() << "Save file";

    QJsonArray jsonArray;

    QVectorIterator<MyVideo*> i(playlist.playlist);

    while(i.hasNext()) {
        MyVideo* video = i.next();

        // Create object
        QJsonObject jsonObj = video->toJson();
//        QJsonArray jsonFrames;

//        jsonObj.insert("videoId", video->videoId);
//        jsonObj.insert("frames", jsonFrames);

        // Add obj to array
        jsonArray.push_back(jsonObj);

    }

    qDebug() << jsonArray;

    QJsonDocument jsonDocument = QJsonDocument(jsonArray);

    qDebug() << jsonDocument;

    saveJson(jsonDocument, "./data.json");


}

void MainWindow::saveJson(QJsonDocument document, QString fileName)
{
    QFile jsonFile(fileName);
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(document.toJson());
}

QJsonDocument MainWindow::loadJson(QString fileName)
{
    QFile jsonFile(fileName);
    jsonFile.open(QFile::ReadOnly);
    return QJsonDocument().fromJson(jsonFile.readAll());
}
