#include "myplayer.h"
#include "image.h"
#include "drawarea.h"
#include "videowidget.h"

//#include <QMediaPlayer>
//#include <QVideoWidget>
//#include <QDebug>
#include <QtWidgets>


//https://stackoverflow.com/questions/51644685/pyqt5-display-widget-on-top-of-qvideowidget-with-qmediaplayer
//https://www.reddit.com/r/Qt5/comments/9s45yw/drawing_rectangle_on_custom_video_widget_is_not/
//https://stackoverflow.com/questions/53024104/how-to-draw-rectangle-on-custom-video-widget-t-in-qt
MyPlayer::MyPlayer(QString filename, int videoIndex, QWidget *parent)
    : QWidget(parent)
{
    // Create our player & widget/canvas
    m_player = new QMediaPlayer(this);
    m_videoWidget = new VideoWidget(this);
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
//    m_player->play();

    qDebug() << m_player->state();
}

void MyPlayer::import()
{
    QFileDialog upload(this);
    upload.setFileMode(QFileDialog::Directory);
    upload.setAcceptMode(QFileDialog::AcceptOpen);
    upload.setWindowTitle("Import Files");
    upload.exec();

    std::string directoryPath = upload.directory().path().toStdString();
    QDir directory(upload.directory().path());
    QStringList imageFileNames = directory.entryList(QStringList() << "*.rgb", QDir::Files);

    foreach(QString filename, imageFileNames)
    {
        QString filePath = upload.directory().path() + filename;
        const char* imagePath = filePath.toLocal8Bit().data();

        Image image;
        image.setHeight(352);
        image.setWidth(288);
        image.setImagePath(imagePath);
        image.ReadImage();

        Images.push_back(new QImage((const unsigned char*)image.getImageData(), 352, 288, QImage::Format_RGB32));
    }
}
