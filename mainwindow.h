#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mygraphicsview.h"
#include "myplaylist.h"
#include "myvideo.h"

#include <QApplication>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QWidget>

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

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  void emitPrimaryProgressBarSignal(int);
  void emitSecondaryProgressBarSignal(int);
  void emitPlayerProgressBarSignal(int);

private slots:
  void on_sliderLeft_changed(int);
  void on_sliderRight_changed(int);
  void on_sliderPlayer_valueChanged(int);
  void on_createNewHyperlink_clicked();
  void on_selectLinks_changed(int);
  void on_selectLinks_edited(QString);
  void on_connectVideo_clicked();
  void on_saveFile_clicked();
  void saveJson(QJsonDocument document, QString fileName);
  QJsonDocument loadJson(QString fileName);
  void import();
  void importWithDirPath(QString directoryPath, QString caller);

  // Player
  void tabSelected(int);
  void on_playerPlay_clicked();
  void on_playerPause_clicked();
  void on_playerStop_clicked();
  void setterFunction();

  void onUpdatePlayerFrame(int);
  void onUpdatePlayerFile(QString);

signals:
  void primarySignalProgress(int);
  void secondarySignalProgress(int);
  void playerSignalProgress(int);

private:
  void imageLoading(QStringList imageFileNames, QStringList constStrs,
                    QVector<QImage> *images);
  void loadRemainingVideos();
  void delay();

  Ui::MainWindow *ui;
  MyGraphicsView *graphicsViewPrimary;
  MyGraphicsView *graphicsViewSecondary;
  MyGraphicsView *graphicsViewPlayer;

  MyPlaylist *playlist;
  MyVideo *primaryVideo;
  MyVideo *secondaryVideo;
  // Used for playing
  MyPlaylist *playerPlaylist;
  MyVideo *playerVideo;
  QTimer *fpsTimer;
  QString originalVideoTitle;
  int originalFramePaused;
  QStringList playlistTitles;

  int currentLinkId = 0;
  int currentPrimaryFrame = 0;   // Frame for left video
  int currentSecondaryFrame = 0; // Frame for right video
  int primaryVideoId = 0;        // Id of
  int secondaryVideoId = 0;

  // Import Video
  QGraphicsPixmapItem *pixMapPrim;
  QGraphicsPixmapItem *pixMapSec;
  QGraphicsPixmapItem *pixMapPlayer;

  QImage imageQ;
  size_t numberImageFilesPrimary;
  size_t numberImageFilesSecondary;
  size_t numberImageFilesPlayer;

  QVector<QImage> primList;
  QVector<QImage> secList;
  // map of filename to images
  QMap<QString, QVector<QImage> *> playerLists;
  //  QVector<QImage> *playerList;

  //  QString primaryVideoFilePath;
  int primaryFramesLoaded = 0;
  QStringList primaryFileNames_n;
  QStringList primaryFileNames_p;

  //  QString secondaryVideoFilePath;
  int secondaryFramesLoaded = 0;
  QStringList secondaryFileNames_n;
  QStringList secondaryFileNames_p;

  //  QString playerVideoFilePath;
  int playerFramesLoaded = 0;
  QStringList playerFileNames_n;
  QStringList playerFileNames_p;

  // Active player
  QString playerFilepath = "";
  int playerStartFrame = 0;
  int audioPosition = 0;
  QString prevPlayerFilePath = "";

  QStringList staticConstStringsPrimary;
  QStringList staticConstStringsSecondary;
  //  QStringList staticConstStringsPlayer;

  // Audio player
  QMediaPlayer *audioPlayer;
};

#endif // MAINWINDOW_H
