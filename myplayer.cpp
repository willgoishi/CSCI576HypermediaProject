#include "myplayer.h"

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
