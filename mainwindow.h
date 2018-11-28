#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMediaPlayer>
#include <QApplication>

#include "player.h"
#include "myplayer.h"

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
    Player *hyperLinkWindow;
    ~MainWindow();


private slots:
    void on_sliderLeft_changed();
    void on_sliderRight_changed();

    void on_importPrimaryButton_clicked();
    void on_importSecondaryButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
