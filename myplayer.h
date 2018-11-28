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

private slots:
    void import();

private:
    QMediaPlayer *m_player = nullptr;
    std::vector<QImage*> PrimaryUploadImages;
    std::vector<QImage*> SecondaryUploadImages;
};

#endif // MYPLAYER_H
