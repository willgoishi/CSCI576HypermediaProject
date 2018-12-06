#include "mainwindow.h"
#include "image.h"
#include "mygraphicsview.h"
//#include "myplayer.h"
#include "myplaylist.h"
#include "ui_mainwindow.h"
#include <QtConcurrent/qtconcurrentrun.h>

#include <QAudioProbe>
#include <QLabel>
#include <QMediaMetaData>
#include <QMediaPlaylist>
#include <QMediaService>
#include <QVideoProbe>

#include <QDebug>
#include <QFrame>
#include <QImage>
#include <QMediaPlayer>
#include <QPoint>
#include <QString>
#include <QVideoWidget>
#include <QtWidgets>

#define TOTAL_FRAMES 2999 // Max to load

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // Add placeholder items to combox dropdown
  ui->selectLinks->addItem("Doctor");
  ui->selectLinks->addItem("Dinosaur");

  ui->horizontalSliderLeft->setMaximum(TOTAL_FRAMES);
  ui->horizontalSliderRight->setMaximum(TOTAL_FRAMES);
  ui->primaryVideoProgressBar->setMaximum(TOTAL_FRAMES);
  ui->secondaryVideoProgressBar->setMaximum(TOTAL_FRAMES);
  ui->sliderPlayer->setMaximum(TOTAL_FRAMES);
  ui->playerVideoProgressBar->setMaximum(TOTAL_FRAMES);

  // Left video
  int WIDTH = 352;
  int HEIGHT = 288;
  playlist = new MyPlaylist();
  graphicsViewPrimary = new MyGraphicsView(playlist, PRIMARY_LOCATION, 0, this);
  graphicsViewPrimary->setGeometry(10, 180, 10 + WIDTH, 180 + HEIGHT);
  graphicsViewPrimary->setFixedSize(WIDTH, HEIGHT);
  graphicsViewPrimary->setSceneRect(0, 0, WIDTH, HEIGHT);
  graphicsViewPrimary->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  graphicsViewPrimary->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  graphicsViewPrimary->resize(WIDTH, HEIGHT);
  this->layout()->addWidget(graphicsViewPrimary);

  // Right video
  graphicsViewSecondary =
      new MyGraphicsView(playlist, SECONDARY_LOCATION, 1, this);
  graphicsViewSecondary->setGeometry(370, 180, 370 + WIDTH, 180 + HEIGHT);
  graphicsViewSecondary->setFixedSize(WIDTH, HEIGHT);
  graphicsViewSecondary->setSceneRect(0, 0, WIDTH, HEIGHT);
  graphicsViewSecondary->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  graphicsViewSecondary->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  graphicsViewSecondary->resize(WIDTH, HEIGHT);
  this->layout()->addWidget(graphicsViewSecondary);

  primList = QVector<QImage>();
  secList = QVector<QImage>();
  //  playerList = QVector<QImage>();

  // Video Player
  fpsTimer = new QTimer();

  // Connect sliders to function
  connect(ui->horizontalSliderLeft, SIGNAL(valueChanged(int)), this,
          SLOT(on_sliderLeft_changed(int)));
  connect(ui->horizontalSliderRight, SIGNAL(valueChanged(int)), this,
          SLOT(on_sliderRight_changed(int)));
  connect(ui->createNewHyperLink, SIGNAL(released()), this,
          SLOT(on_createNewHyperlink_clicked()));
  //    connect(ui->connectVideo, SIGNAL(released()), this,
  //    SLOT(on_connectVideo_clicked()));
  connect(ui->selectLinks, SIGNAL(activated(int)), this,
          SLOT(on_selectLinks_changed(int)));
  connect(ui->selectLinks, SIGNAL(editTextChanged(QString)), this,
          SLOT(on_selectLinks_edited(QString)));
  //    connect(ui->saveFile, SIGNAL(released()), this,
  //    SLOT(on_saveFile_clicked()));
  connect(ui->tabWidget, SIGNAL(currentChanged(int)), this,
          SLOT(tabSelected(int)));

  connect(ui->importPrimaryButton, SIGNAL(clicked()), this, SLOT(import()));
  connect(ui->importSecondaryButton, SIGNAL(clicked()), this, SLOT(import()));

  resize(740, 680);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_sliderLeft_changed(int currentPrimaryFrame) {

  if (currentPrimaryFrame >= primaryFramesLoaded) {
    qDebug() << "Frames not loaded! " << currentPrimaryFrame;
    return;
  }

  qDebug() << "currentPrimaryFrame = " << currentPrimaryFrame;

  // Update boundaries
  ui->frameCountLeft->setText(QString::number(currentPrimaryFrame));

  pixMapPrim = new QGraphicsPixmapItem(
      QPixmap::fromImage(primList.at(currentPrimaryFrame)));

  graphicsViewPrimary->scene->addItem(pixMapPrim);
  graphicsViewPrimary->pixMapPrim = pixMapPrim;
  graphicsViewPrimary->updateBoundary(currentPrimaryFrame);
}

void MainWindow::on_sliderRight_changed(int currentSecondaryFrame) {

  if (currentSecondaryFrame >= secondaryFramesLoaded) {
    qDebug() << "Frames not loaded! " << secondaryFramesLoaded;
    return;
  }

  qDebug() << "currentSecondaryFrame = " << currentSecondaryFrame;

  ui->frameCountRight->setText(QString::number(currentSecondaryFrame));

  pixMapSec = new QGraphicsPixmapItem(
      QPixmap::fromImage(secList.at(currentSecondaryFrame)));

  graphicsViewSecondary->scene->addItem(pixMapSec);
  graphicsViewSecondary->pixMapSec = pixMapSec;
}

void MainWindow::on_sliderPlayer_valueChanged(int currentPlayerFrame) {

  if (currentPlayerFrame >= playerFramesLoaded) {
    qDebug() << "Frames not loaded! " << currentPlayerFrame;
    return;
  }

  qDebug() << "on_sliderPlayer_valueChanged()";

  // Update boundaries
  ui->frameCountPlayer->setText(QString::number(currentPlayerFrame));

  qDebug() << "playerFilepath: " << playerFilepath;

  QVector<QImage> *imageList = playerLists[playerFilepath];

  pixMapPlayer = new QGraphicsPixmapItem(
      QPixmap::fromImage((*imageList)[currentPlayerFrame]));

  graphicsViewPlayer->scene->addItem(pixMapPlayer);
  graphicsViewPlayer->pixMapPlayer = pixMapPlayer;

  // Don't update boundary if we aren't playing the original video
  if (originalVideoTitle == playerFilepath) {
    graphicsViewPlayer->graphicsLocation = PLAYER_LOCATION;
  } else {
    graphicsViewPlayer->graphicsLocation = HYPERLINK_LOCATION;
  }

  graphicsViewPlayer->updateBoundary(currentPlayerFrame);
}

void MainWindow::on_createNewHyperlink_clicked() {
  qDebug() << "on button clicked";

  bool ok;
  QString text =
      QInputDialog::getText(this, tr("QInputDialog::getText()"),
                            tr("Link label:"), QLineEdit::Normal, "", &ok);
  if (ok && !text.isEmpty()) {
    ui->selectLinks->addItem(text);
  }

  qDebug() << "Confirmed:" << text;
}

void MainWindow::on_selectLinks_changed(int index) {
  currentLinkId = index;

  graphicsViewPrimary->updateCurrentLink(index);

  // Get 1st frame of link in video
  int frameIndex =
      primaryVideo->getFirstFrameWithBoundaryFromLinkId(currentLinkId);

  if (primaryVideo->hasFirstFrameWithBoundaryFromLinkId(currentLinkId)) {
    qDebug() << "hasFirstFrameWithBoundaryFromLinkId";
    ui->horizontalSliderLeft->setValue(frameIndex);
    ui->horizontalSliderLeft->update();
  } else {
    frameIndex = ui->horizontalSliderLeft->value();
  }

  // Redo boundary
  pixMapPrim =
      new QGraphicsPixmapItem(QPixmap::fromImage(primList.at(frameIndex)));
  graphicsViewPrimary->scene->addItem(pixMapPrim);
  graphicsViewPrimary->pixMapPrim = pixMapPrim;
  graphicsViewPrimary->updateBoundary(frameIndex);

  qDebug() << "1st frame for linkId:" << currentLinkId << " is " << frameIndex;

  qDebug() << "Link index selected: " << index;
}

void MainWindow::on_selectLinks_edited(QString label) {
  qDebug() << "edit label: " << label;

  ui->selectLinks->setItemText(ui->selectLinks->currentIndex(), label);
}

void MainWindow::on_connectVideo_clicked() {
  qDebug() << "on_connectVideo_clicked";

  // Get info of secondary video
  int targetFrameCount = ui->horizontalSliderRight->value();

  qDebug() << "targetFrameCount: " << targetFrameCount;

  MyFrame *targetFrame = secondaryVideo->getFrame(targetFrameCount);

  qDebug() << "target frame count" << targetFrame->getFrameCount();

  primaryVideo->addHyperlinkTarget(currentLinkId, targetFrame);

  foreach (int key, primaryVideo->hyperlinks.keys()) {
    MyFrame *targetFrame = primaryVideo->hyperlinks.value(key);
    qDebug() << "Link ID: " << key;
    qDebug() << "Frame Count: " << targetFrame->getFrameCount();
  }
}

void MainWindow::on_saveFile_clicked() {
  qDebug() << "Save file";

  QJsonArray jsonArray;

  QVectorIterator<MyVideo *> i(playlist->playlist);

  while (i.hasNext()) {
    MyVideo *video = i.next();

    // Create object
    QJsonObject jsonObj = video->toJson();
    //        QJsonArray jsonFrames;

    //        jsonObj.insert("videoId", video->videoId);
    //        jsonObj.insert("frames", jsonFrames);

    // Add obj to array
    jsonArray.push_back(jsonObj);
  }

  qDebug() << jsonArray;

  QJsonDocument jsonDocument = QJsonDocument(jsonArray);

  qDebug() << jsonDocument;

  saveJson(jsonDocument, "./data.json");
}

void MainWindow::tabSelected(int tab) {
  qDebug() << "Tab selected";

  // If showing player
  if (tab == 1) {
    graphicsViewPrimary->hide();
    graphicsViewSecondary->hide();

    // Load data

    // Get path
    QString filepath = qApp->applicationDirPath() + "/data.json";
    qDebug() << filepath;

    QJsonDocument doc = loadJson(filepath);
    QJsonArray jsonArray = doc.array();

    // Create playlist, add video at end of loop
    playerPlaylist = new MyPlaylist();

    if (jsonArray.size() == 0) {
      return;
    }

    foreach (const QJsonValue &v, jsonArray) {

      QJsonObject video = v.toObject();
      QString myVideoTitle = video.value("videoTitle").toString();

      playerVideo = new MyVideo(myVideoTitle);

      // Load boundary for frames
      QJsonArray frames = video.value("frames").toArray();

      foreach (const QJsonValue &v2, frames) {
        QJsonObject frame = v2.toObject();
        QJsonArray boundaries = frame.value("boundaries").toArray();

        int frameCount = frame.value("frameCount").toInt();
        QString videoTitle = frame.value("videoTitle").toString();

        //        qDebug() << "frameCount: " << frameCount;
        //        qDebug() << "videoTitle: " << videoTitle;

        // Create frame
        MyFrame *myFrame = new MyFrame(frameCount, videoTitle);
        playerVideo->addFrame(myFrame);

        // If we have boundaries
        if (boundaries.count() > 0) {

          //          qDebug() << "\n Boundaries \n" << boundaries << "\n";

          foreach (const QJsonValue &v3, boundaries) {

            QJsonObject boundary = v3.toObject();
            QJsonArray coords = boundary.value("coords").toArray();
            int linkId = boundary.value("linkId").toInt();

            qreal x = coords.at(0).toDouble();
            qreal y = coords.at(1).toDouble();
            qreal x2 = coords.at(2).toDouble();
            qreal y2 = coords.at(3).toDouble();

            QRectF rect = QRectF(x, y, x2 - x, y2 - y);
            QGraphicsRectItem *boundaryRect = new QGraphicsRectItem(rect);
            myFrame->addBoundary(linkId, boundaryRect);
          }
        }
      }

      // Load hyperlinks
      QJsonArray hyperlinks = video.value("hyperlinks").toArray();

      foreach (const QJsonValue &v4, hyperlinks) {
        QJsonObject hyperlink = v4.toObject();
        int frameCount = hyperlink.value("frameCount").toInt();
        int linkId = hyperlink.value("linkId").toInt();
        QString videoTitle = hyperlink.value("videoTitle").toString();

        playerVideo->addHyperlinkTarget(linkId,
                                        new MyFrame(frameCount, videoTitle));
      }

      playerPlaylist->addVideo(playerVideo);
      break; // Only need 1 loop
    }

    //
    // Create player
    //
    int WIDTH = 352;
    int HEIGHT = 288;
    graphicsViewPlayer =
        new MyGraphicsView(playerPlaylist, PLAYER_LOCATION, 0, this);
    graphicsViewPlayer->setGeometry(10, 180, 10 + WIDTH, 180 + HEIGHT);
    graphicsViewPlayer->setFixedSize(WIDTH, HEIGHT);
    graphicsViewPlayer->setSceneRect(0, 0, WIDTH, HEIGHT);
    graphicsViewPlayer->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsViewPlayer->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsViewPlayer->resize(WIDTH, HEIGHT);
    this->layout()->addWidget(graphicsViewPlayer);

    // Connect functions
    connect(graphicsViewPlayer, SIGNAL(updatePlayerFrame(int)), this,
            SLOT(onUpdatePlayerFrame(int)));
    connect(graphicsViewPlayer, SIGNAL(updatePlayerFile(QString)), this,
            SLOT(onUpdatePlayerFile(QString)));

    // Import primary video
    QString videoTitle = playerVideo->videoTitle;
    originalVideoTitle = videoTitle;
    QString directoryPath = videoTitle;
    qDebug() << videoTitle;
    importWithDirPath(directoryPath, "videoPlayer");

    // Get remaining videos
    MyVideo *hyperlinkVideo = playerPlaylist->getVideo(0);
    foreach (MyFrame *hyperlinkFrame, hyperlinkVideo->hyperlinks) {
      QString videoTitle = hyperlinkFrame->videoTitle;

      qDebug() << "videoTitle: " << videoTitle;

      // If video title not loaded before, load
      if (!playerLists.contains(videoTitle)) {
        importWithDirPath(videoTitle, "videoPlayer");
      }
    }

  } else {
    graphicsViewPrimary->show();
    graphicsViewSecondary->show();
  }
}

void MainWindow::on_playerPlay_clicked() {
  qDebug() << "Player play clicked";
  fpsTimer->setInterval(17);
  connect(fpsTimer, SIGNAL(timeout()), this, SLOT(setterFunction()));
  fpsTimer->start();
}

void MainWindow::on_playerPause_clicked() {
  qDebug() << "Player pause clicked";
  fpsTimer->stop();
}

void MainWindow::on_playerStop_clicked() {
  qDebug() << "Player stop clicked";
  if (fpsTimer->isActive()) {
    fpsTimer->stop();
    ui->sliderPlayer->setValue(0);
  }

  // Go back to previous video
  qDebug() << "originalVideoTitle" << originalVideoTitle;
  qDebug() << "playerFilepath" << playerFilepath;
  qDebug() << "originalFramePaused" << originalFramePaused;

  if (originalVideoTitle != playerFilepath) {
    qDebug() << "Go back to original video";
    playerFilepath = originalVideoTitle;
    ui->sliderPlayer->setValue(originalFramePaused);
  }
}

void MainWindow::setterFunction() {
  ui->sliderPlayer->setValue(ui->sliderPlayer->value() + 1);
}

void MainWindow::onUpdatePlayerFrame(int value) {
  delay();
  qDebug() << "onUpdatePlayerFrame():" << value;
  originalFramePaused = ui->sliderPlayer->value();
  ui->sliderPlayer->setValue(value);
}

void MainWindow::onUpdatePlayerFile(QString value) {
  qDebug() << "onUpdatePlayerFile():" << value;

  // Set file path for slider to use
  playerFilepath = value;
}

void MainWindow::emitPrimaryProgressBarSignal(int value) {
  emit primarySignalProgress(value);
}

void MainWindow::emitSecondaryProgressBarSignal(int value) {
  emit secondarySignalProgress(value);
}

void MainWindow::emitPlayerProgressBarSignal(int value) {
  emit playerSignalProgress(value);
}

void MainWindow::saveJson(QJsonDocument document, QString fileName) {
  QFile jsonFile(fileName);
  jsonFile.open(QFile::WriteOnly);
  jsonFile.write(document.toJson());
}

QJsonDocument MainWindow::loadJson(QString fileName) {
  QFile jsonFile(fileName);
  jsonFile.open(QFile::ReadOnly);
  return QJsonDocument().fromJson(jsonFile.readAll());
}

void MainWindow::imageLoading(QStringList imageFileNames, QStringList constStrs,
                              QVector<QImage> *images) {

  qDebug() << "imageLoading" << constStrs[1];

  connect(this, SIGNAL(primarySignalProgress(int)), ui->primaryVideoProgressBar,
          SLOT(setValue(int)));
  connect(this, SIGNAL(secondarySignalProgress(int)),
          ui->secondaryVideoProgressBar, SLOT(setValue(int)));
  connect(this, SIGNAL(playerSignalProgress(int)), ui->playerVideoProgressBar,
          SLOT(setValue(int)));

  int count = 0;
  foreach (QString filename, imageFileNames) {

    if (count > TOTAL_FRAMES) {
      break;
    }

    if (count % 1000 == 0) {
      qDebug() << "Loaded " << filename;
    }

    if (constStrs[1] == "importPrimaryButton") {
      primaryFramesLoaded = count;
      this->emitPrimaryProgressBarSignal(primaryFramesLoaded);
      // Emit on first frame only
      if (count == 0) {
        qDebug() << "Emit!!";
        ui->horizontalSliderLeft->setValue(1);
        ui->horizontalSliderLeft->setValue(0);
      }
    }
    if (constStrs[1] == "importSecondaryButton") {
      secondaryFramesLoaded = count;
      this->emitSecondaryProgressBarSignal(secondaryFramesLoaded);
      if (count == 0) {
        qDebug() << "Emit!!";
        ui->horizontalSliderRight->setValue(1);
        ui->horizontalSliderRight->setValue(0);
      }
    }
    if (constStrs[1] == "videoPlayer") {
      playerFramesLoaded = count;
      this->emitPlayerProgressBarSignal(playerFramesLoaded);
      if (count == 0) {
        qDebug() << "Emit!!";
        ui->sliderPlayer->setValue(1);
        ui->sliderPlayer->setValue(0);
      }
    }

    QString filePath = constStrs.at(0) + "/" + filename;
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
      qDebug() << "Issue occurred in opening the file";
      return;
    }

    QByteArray data = file.readAll();
    file.flush();
    file.close();

    QImage img(352, 288, QImage::Format_RGB32);
    QRgb *pixels = reinterpret_cast<QRgb *>(img.bits());
    for (uint i = 0; i < 352 * 288; i++) {
      uchar pixel1 = data[i];
      uchar pixel2 = data[i + 1];
      uchar pixel3 = data[i + 2];
      pixels[i] = qRgb(pixel1, pixel2, pixel3);
    }
    // Format_Indexed8 //Format_RGB16
    img = img.convertToFormat(QImage::Format_RGB444);
    images->push_back(img);

    //    qDebug() << filename;
    count++;
  }
  qDebug() << "Done Loading!";
}

void MainWindow::delay() {
  QTime dieTime = QTime::currentTime().addSecs(0.5);
  while (QTime::currentTime() < dieTime)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::import() {
  QFileDialog upload(this);
  upload.setFileMode(QFileDialog::Directory);
  upload.setAcceptMode(QFileDialog::AcceptOpen);
  upload.setWindowTitle("Import Files");
  upload.exec();

  QString directoryPath = upload.directory().path();
  QString caller = QObject::sender()->objectName();

  importWithDirPath(directoryPath, caller);
}

void MainWindow::importWithDirPath(QString directoryPath, QString caller) {

  qDebug() << "directoryPath: " << directoryPath;
  qDebug() << "caller: " << caller;

  QDir directory(directoryPath);

  QStringList imageFileNames =
      directory.entryList(QStringList() << "*.rgb", QDir::Files);

  qDebug() << "Size: " << imageFileNames.size();

  if (caller == "importPrimaryButton") {
    //
    // Create Video
    //
    primaryVideo = new MyVideo(directoryPath);
    for (int i = 0; i <= TOTAL_FRAMES; i++) {

      // Constructor for video
      int frameCount = i;

      // Create node & append
      MyFrame *frame = new MyFrame(frameCount, directoryPath);
      primaryVideo->addFrame(frame);
    }
    playlist->addVideo(primaryVideo);

    //
    // Import
    //
    staticConstStringsPrimary.append(directoryPath);
    staticConstStringsPrimary.append(caller);
    QFuture<void> f1 =
        QtConcurrent::run(this, &MainWindow::imageLoading, imageFileNames,
                          staticConstStringsPrimary, &primList);
    //    f1.waitForFinished();

  } else if (caller == "importSecondaryButton") {
    //
    // Create Video
    //
    secondaryVideo = new MyVideo(directoryPath);
    for (int i = 0; i <= TOTAL_FRAMES; i++) {

      // Constructor for video
      int frameCount = i;

      // Create node & append
      MyFrame *frame = new MyFrame(frameCount, directoryPath);
      secondaryVideo->addFrame(frame);
    }
    playlist->addVideo(secondaryVideo);

    //
    // Import
    //
    staticConstStringsSecondary.append(directoryPath);
    staticConstStringsSecondary.append(caller);

    QFuture<void> f2 =
        QtConcurrent::run(this, &MainWindow::imageLoading, imageFileNames,
                          staticConstStringsSecondary, &secList);
    //    f2.waitForFinished();

  } else if (caller == "videoPlayer") {

    qDebug() << "caller == videoPlayer";

    //
    // Import
    //
    QStringList staticConstStringsPlayer;
    staticConstStringsPlayer.append(directoryPath);
    staticConstStringsPlayer.append(caller);

    qDebug() << "staticConstStringsPlayer";

    QVector<QImage> *playerList = new QVector<QImage>();
    playerLists.insert(directoryPath, playerList);

    playerFilepath = playerPlaylist->getVideo(0)->videoTitle;
    //    playerFilepath = "/Users/webber/Downloads/London/LondonOne";

    qDebug() << "QtConcurrent::run";

    QtConcurrent::run(this, &MainWindow::imageLoading, imageFileNames,
                      staticConstStringsPlayer, &*playerList);
  }
}
