#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMediaPlayer>
#include <QApplication>

#include "player.h"
#include "myplayer.h"
#include "mygraphicsview.h"

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

public slots:
    void import();
    //void imageLoading(QStringList, QString, QString, QList<QImage> &images, QStringList *fileNames);

private slots:
    void on_sliderLeft_changed();
    void on_sliderRight_changed();

    void on_primaryNextFramesButton_clicked();
    void on_secondaryNextFramesButton_clicked();

    void on_primaryPrevFramesButton_clicked();
    void on_secondaryPrevFramesButton_clicked();

private:
    Ui::MainWindow *ui;
    MyGraphicsView* graphicsView;
    MyGraphicsView* graphicsView2;

    std::vector<QImage*> PrimaryUploadImages;
    std::vector<QImage*> SecondaryUploadImages;
    QImage imageQ;
    size_t numberImageFilesPrimary;
    size_t numberImageFilesSecondary;

    QList<QImage> primList;
    QList<QImage> secList;

    QString primaryVideoFilePath;
    QStringList primaryFileNames_n;
    QStringList primaryFileNames_p;

    QString secondaryVideoFilePath;
    QStringList secondaryFileNames_n;
    QStringList secondaryFileNames_p;

    size_t segmentIndexPrimary = 0;
    size_t segmentIndexSecondary = 0;

    QStringList staticConstStringsPrimary;
    QStringList staticConstStringsSecondary;

};

#endif // MAINWINDOW_H
