#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QWidget>
#include <QMediaPlayer>
//#include <QVideoWidget>
#include <QString>
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QMouseEvent>


class MyPlayer :
    public QWidget
{
    Q_OBJECT
public:
    explicit MyPlayer(QString, int, QWidget *parent = nullptr);

    QVideoWidget *m_videoWidget = nullptr;

private slots:
    void import();
protected:

private:
    QMediaPlayer *m_player = nullptr;
    std::vector<QImage*> Images;
};

#endif // MYPLAYER_H
