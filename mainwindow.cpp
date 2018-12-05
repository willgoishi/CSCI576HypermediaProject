#include "mainwindow.h"
#include "image.h"
#include "mygraphicsview.h"
//#include "myplayer.h"
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
#include <QMediaPlayer>
#include <QPainter>
#include <QPoint>
#include <QVideoWidget>
#include <QtWidgets>

#define TOTAL_FRAMES 1000 // Max to load

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

  /**
   * Create 1st video
   * @brief videoTitle
   */
  QString videoTitle = "First Video";
  primaryVideo = new MyVideo(videoTitle);
  for (int i = 0; i < TOTAL_FRAMES; i++) {

    // Constructor for video
    int frameCount = i;

    // Create node & append
    MyFrame *frame = new MyFrame(frameCount, videoTitle);
    primaryVideo->addFrame(frame);
  }
  playlist.addVideo(primaryVideo);

  /**
   * Create 2nd video
   * @brief videoTitle
   */
  videoTitle = "Second Video";
  secondaryVideo = new MyVideo(videoTitle);
  for (int i = 0; i < TOTAL_FRAMES; i++) {

    // Constructor for video
    int frameCount = i;

    // Create node & append
    MyFrame *frame = new MyFrame(frameCount, videoTitle);
    secondaryVideo->addFrame(frame);
  }
  playlist.addVideo(secondaryVideo);

  // Left video
  int WIDTH = 352;
  int HEIGHT = 288;
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
  qDebug() << "currentPrimaryFrame = " << currentPrimaryFrame;

  // Update boundaries
  ui->frameCountLeft->setText(QString::number(currentPrimaryFrame));
  //  graphicsViewPrimary->updateBoundary(currentPrimaryFrame);

  pixMapPrim = new QGraphicsPixmapItem(
      QPixmap::fromImage(primList.at(currentPrimaryFrame)));

  graphicsViewPrimary->scene->addItem(pixMapPrim);
  graphicsViewPrimary->pixMapPrim = pixMapPrim;
}

void MainWindow::on_sliderRight_changed(int currentSecondaryFrame) {
  qDebug() << "currentSecondaryFrame = " << currentSecondaryFrame;

  ui->frameCountRight->setText(QString::number(currentSecondaryFrame));

  pixMapSec = new QGraphicsPixmapItem(
      QPixmap::fromImage(secList.at(currentSecondaryFrame)));

  graphicsViewSecondary->scene->addItem(pixMapSec);
  graphicsViewSecondary->pixMapSec = pixMapSec;
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
    ui->horizontalSliderLeft->setValue(frameIndex);
  }

  qDebug() << "1st frame for linkId:" << currentLinkId << " is " << frameIndex;

  qDebug() << "Link index selected: " << index;
}

void MainWindow::on_selectLinks_edited(QString label) {
  qDebug() << "edit label: " << label;

  ui->selectLinks->setItemText(ui->selectLinks->currentIndex(), label);
}

void MainWindow::on_connectVideo_clicked() {
  qDebug() << "on_connectVideo_clicked";

  // Get info of primary video
  int srcFrameCount = ui->horizontalSliderLeft->value();

  // Get info of secondary video
  //    int targetVideoId = secondaryVideo.getVideoId();
  int targetFrameCount = ui->horizontalSliderRight->value();

  //    MyFrame* srcFrame = primaryVideo->getFrame(srcFrameCount);
  MyFrame *targetFrame = secondaryVideo->getFrame(targetFrameCount);

  primaryVideo->addHyperlinkTarget(currentLinkId, targetFrame);

  //    // Get data from playlist
  //    MyVideo* primaryVideo = playlist.getVideo(0);
  //    srcFrame = primaryVideo->getFrame(srcFrameCount);
  //    targetFrame = srcFrame->getHyperlinkTarget(currentLinkId);

  //    QMap<int, MyFrame*>::iterator i(primaryVideo->hyperlinks);

  foreach (int key, primaryVideo->hyperlinks.keys()) {
    MyFrame *targetFrame = primaryVideo->hyperlinks.value(key);
    qDebug() << "Link ID: " << key;
    qDebug() << "Frame Count: " << targetFrame->getFrameCount();
    //        qDebug() << "Frame Video: " << targetFrame->videoId;
  }

  //    qDebug() << "Src link ID: " << currentLinkId;
  //    qDebug() << "Target frame: " << targetFrame->getFrameCount();
  //    qDebug() << "Target video: " << targetFrame->getVideoId();
}

void MainWindow::on_saveFile_clicked() {
  qDebug() << "Save file";

  QJsonArray jsonArray;

  QVectorIterator<MyVideo *> i(playlist.playlist);

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
    //        qDebug() << filepath;

    QJsonDocument doc = loadJson(filepath);
    QJsonArray jsonArray = doc.array();

    foreach (const QJsonValue &v, jsonArray) {

      QJsonObject video = v.toObject();
      QString myVideoTitle = video.value("videoTitle").toString();

      MyVideo *myVideo = new MyVideo(myVideoTitle);

      // Load boundary for frames
      QJsonArray frames = video.value("frames").toArray();

      foreach (const QJsonValue &v2, frames) {
        QJsonObject frame = v2.toObject();
        QJsonArray boundaries = frame.value("boundaries").toArray();

        int frameCount = frame.value("frameCount").toInt();
        QString videoTitle = frame.value("videoTitle").toString();

        qDebug() << "frameCount: " << frameCount;
        qDebug() << "videoTitle: " << videoTitle;

        // Create frame
        MyFrame *myFrame = new MyFrame(frameCount, videoTitle);
        myVideo->addFrame(myFrame);

        // If we have boundaries
        if (boundaries.count() > 0) {

          qDebug() << "\n Boundaries \n" << boundaries << "\n";

          foreach (const QJsonValue &v3, boundaries) {

            QJsonObject boundary = v3.toObject();
            QJsonArray coords = boundary.value("coords").toArray();
            int linkId = boundary.value("linkId").toInt();

            qDebug() << "\n Coords \n" << coords << "\n";
            qDebug() << "\n Coords[0] \n" << coords.at(0) << "\n";
            qDebug() << "\n Coords[1] \n" << coords.at(1) << "\n";
            qDebug() << "\n Coords[2] \n" << coords.at(2) << "\n";
            qDebug() << "\n Coords[3] \n" << coords.at(3) << "\n";
            qDebug() << "\n linkId \n" << linkId << "\n";

            qreal x = coords.at(0).toDouble();
            qreal y = coords.at(1).toDouble();
            qreal w = coords.at(2).toDouble();
            qreal h = coords.at(3).toDouble();

            QRectF rect = QRectF(x, y, w, h);
            QGraphicsRectItem *boundaryRect = new QGraphicsRectItem(rect);
            myFrame->addBoundary(linkId, boundaryRect);
          }
        }
      }

      qDebug() << "\nFrames: \n" << frames << "\n";

      // Load hyperlinks
      QJsonArray hyperlinks = video.value("hyperlinks").toArray();

      foreach (const QJsonValue &v4, hyperlinks) {
        QJsonObject hyperlink = v4.toObject();
        int frameCount = hyperlink.value("frameCount").toInt();
        int linkId = hyperlink.value("linkId").toInt();
        QString videoTitle = hyperlink.value("videoTitle").toString();

        myVideo->addHyperlinkTarget(linkId,
                                    new MyFrame(frameCount, videoTitle));
      }

      qDebug() << v.toObject() << "\n";
    }

  } else {
    graphicsViewPrimary->show();
    graphicsViewSecondary->show();
  }
}

void MainWindow::on_playerPlay_clicked() { qDebug() << "Player play clicked"; }

void MainWindow::on_playerPause_clicked() {
  qDebug() << "Player pause clicked";
}

void MainWindow::on_playerStop_clicked() { qDebug() << "Player stop clicked"; }

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
                              QVector<QImage> *images, QStringList *fileNames,
                              QStringList *fileNamesPrev) {
  int count = 0;
  foreach (QString filename, imageFileNames) {

    if (constStrs[2] == "primary") {
      ui->primaryVideoProgressBar->setValue(count);
    }
    if (constStrs[2] == "secondary") {
      ui->secondaryVideoProgressBar->setValue(count);
    }

    if (count > TOTAL_FRAMES) {
      break;
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
    for (int i = 0; i < 352 * 288; i++) {
      uchar pixel1 = data[i];
      uchar pixel2 = data[i + 1];
      uchar pixel3 = data[i + 2];
      pixels[i] = qRgb(pixel1, pixel2, pixel3);
    }

    img = img.convertToFormat(
        QImage::Format_RGB444); // Format_Indexed8 //Format_RGB16
    images->push_back(img);

    if (constStrs.at(1) == "primaryNextFramesButton" ||
        constStrs.at(1) == "secondaryNextFramesButton" ||
        constStrs.at(1) == "importPrimaryButton" ||
        constStrs.at(1) == "importSecondaryButton") {
      fileNamesPrev->append(filename);
      fileNames->removeFirst();
    } else {
      QString s = fileNamesPrev->last();
      fileNamesPrev->prepend(s);
      fileNames->removeLast();
    }
    qDebug() << filename;
    count++;
  }
  qDebug() << "Done Loading!";
}

void MainWindow::import() {
  QFileDialog upload(this);
  upload.setFileMode(QFileDialog::Directory);
  upload.setAcceptMode(QFileDialog::AcceptOpen);
  upload.setWindowTitle("Import Files");
  upload.exec();

  QString directoryPath = upload.directory().path();
  QDir directory(upload.directory().path());
  QStringList imageFileNames =
      directory.entryList(QStringList() << "*.rgb", QDir::Files);

  QString caller = QObject::sender()->objectName();

  qDebug() << "Caller: " << caller;

  caller == "importPrimaryButton" ? ui->primaryVideoProgressBar->show()
                                  : ui->secondaryVideoProgressBar->show();

  if (caller == "importPrimaryButton") {
    numberImageFilesPrimary = imageFileNames.size();
    staticConstStringsPrimary.append(directoryPath);
    staticConstStringsPrimary.append(caller);
    staticConstStringsPrimary.append("primary");
    primaryFileNames_n = imageFileNames;
    QFuture<void> f1 =
        QtConcurrent::run(this, &MainWindow::imageLoading, primaryFileNames_n,
                          staticConstStringsPrimary, &primList,
                          &primaryFileNames_n, &primaryFileNames_p);
    //    f1.waitForFinished();
  } else {
    numberImageFilesSecondary = imageFileNames.size();
    staticConstStringsSecondary.append(directoryPath);
    staticConstStringsSecondary.append(caller);
    staticConstStringsSecondary.append("secondary");
    secondaryFileNames_n = imageFileNames;

    QFuture<void> f2 =
        QtConcurrent::run(this, &MainWindow::imageLoading, secondaryFileNames_n,
                          staticConstStringsSecondary, &secList,
                          &secondaryFileNames_n, &secondaryFileNames_p);
    //    f2.waitForFinished();
  }

  //  caller == "importPrimaryButton"
  //      ? ui->primaryNextFramesButton->setEnabled(true)
  //      : ui->secondaryNextFramesButton->setEnabled(true);
  qDebug() << "Done Loading!";
}
