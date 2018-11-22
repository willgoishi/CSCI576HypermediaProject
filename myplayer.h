#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QString>

class MyPlayer :
    public QWidget
{
    Q_OBJECT
public:
    explicit MyPlayer(QString, int, QWidget *parent = nullptr);

    QVideoWidget *m_videoWidget = nullptr;

private:
    QMediaPlayer *m_player = nullptr;
};

#endif // MYPLAYER_H
