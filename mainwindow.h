#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mygraphicsview.h"
#include "myplaylist.h"
#include "myvideo.h"

#include <QMainWindow>
#include <QWidget>
#include <QMediaPlayer>
#include <QApplication>

QT_BEGIN_NAMESPACE
class QAbstractItemView;
class QLabel;
class QMediaPlayer;
class QModelIndex;
class QPushButton;
class QSlider;
class QStatusBar;
class QVideoProbe;
class QVideoWidget;
class QAudioProbe;
QT_END_NAMESPACE


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_sliderLeft_changed(int);
    void on_sliderRight_changed(int);
    void on_createNewHyperlink_clicked();
    void on_selectLinks_changed(int);
    void on_selectLinks_edited(QString);
    void on_connectVideo_clicked();

private:
    Ui::MainWindow* ui;
    MyGraphicsView* graphicsViewPrimary;
    MyGraphicsView* graphicsViewSecondary;

    MyPlaylist playlist;
    MyVideo* primaryVideo;
    MyVideo* secondaryVideo;

    int currentLinkId = 0;
    int currentPrimaryFrame = 0;    // Frame for left video
    int currentSecondaryFrame = 0;  // Frame for right video
    int primaryVideoId = 0;         // Id of
    int secondaryVideoId = 0;
};

#endif // MAINWINDOW_H
