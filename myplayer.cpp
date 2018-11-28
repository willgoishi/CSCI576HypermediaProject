#include "myplayer.h"
#include "image.h"

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QDebug>
#include <QtWidgets>

MyPlayer::MyPlayer(QString filename, int videoIndex, QWidget *parent)
    : QWidget(parent)
{
    // Create our player & widget/canvas
    m_player = new QMediaPlayer;
    m_videoWidget = new QVideoWidget;
    m_player->setVideoOutput(m_videoWidget);

    m_player->setMedia(QUrl::fromLocalFile(filename));

    // X coordinate based on video index
    int video1X = 0;
    if (videoIndex == 1) {
        video1X = 10;
    }
    if (videoIndex == 2) {
        video1X = 370;
    }

    int video1Y = 180;

    int VIDEO_WIDTH = 352;
    int VIDEO_HEIGHT = 288;

    m_videoWidget->setGeometry(video1X, video1Y, video1X + VIDEO_WIDTH, video1Y + VIDEO_HEIGHT);
    m_videoWidget->resize(VIDEO_WIDTH, VIDEO_HEIGHT);
    m_videoWidget->show();

    m_player->play();

    qDebug() << m_player->state();
}

void MyPlayer::import()
{
    QFileDialog upload(this);
    upload.setFileMode(QFileDialog::Directory);
    upload.setAcceptMode(QFileDialog::AcceptOpen);
    upload.setWindowTitle("Import Files");
    upload.exec();

    /*
    QTextStream out(stdout);
    QStringList fileNames = upload.getOpenFileNames(this,"Select Files to Upload", QDir::homePath(), tr("(*rgb);;AllFiles(*)"));
    if(fileNames.count() > 0)
    {
        for(int fn = 0; fn < fileNames.count(); fn++)
        {
            const char* imagePath = fileNames.at(fn).toLocal8Bit().data();

            Image image;
            image.setHeight(352);
            image.setWidth(288);
            image.setImagePath(imagePath);
            image.ReadImage();

            //Images.push_back(new QImage((const unsigned char*)image.getImageData(), 352, 288, QImage::Format_RGB32));

        }
    }*/

    qDebug() << upload.directory().path();

    QDir directory(upload.directory().path());

    QStringList imageFileNames = directory.entryList(QStringList() << "*.rgb");

    qDebug() << imageFileNames.at(0);
    qDebug() << imageFileNames.at(8999);

    /*foreach(QString filename, imageFileNames)
    {
        const char* imagePath = filename.toLocal8Bit().data();
        Image image;
        image.setHeight(352);
        image.setWidth(288);
        image.setImagePath(imagePath);
        image.ReadImage();
    }*/

}
