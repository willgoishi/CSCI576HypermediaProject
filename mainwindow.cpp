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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // Add placeholder items to combox dropdown
  ui->selectLinks->addItem("Doctor");
  ui->selectLinks->addItem("Dinosaur");

  // Slider
  //  ui->horizontalSliderLeft->setSingleStep(1);
  //  ui->horizontalSliderLeft->setMaximum(100);

  ui->primaryNextFramesButton->setEnabled(false);
  ui->primaryPrevFramesButton->setEnabled(false);
  ui->secondaryNextFramesButton->setEnabled(false);
  ui->secondaryPrevFramesButton->setEnabled(false);

  ui->primaryVideoProgressBar->setValue(0);
  ui->primaryVideoProgressBar->hide();
  ui->secondaryVideoProgressBar->setValue(0);
  ui->secondaryVideoProgressBar->hide();

  // Create playlist to hold all the videos
  int totalFrames = 101;

  /**
   * Create 1st video
   * @brief videoTitle
   */
  QString videoTitle = "First Video";
  primaryVideo = new MyVideo(videoTitle);
  for (int i = 0; i < totalFrames; i++) {

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
  for (int i = 0; i < totalFrames; i++) {

    // Constructor for video
    int frameCount = i;

    // Create node & append
    MyFrame *frame = new MyFrame(frameCount, videoTitle);
    secondaryVideo->addFrame(frame);
  }
  playlist.addVideo(secondaryVideo);

  // Left video
  int width = 352;
  int height = 288;
  graphicsViewPrimary = new MyGraphicsView(playlist, PRIMARY_LOCATION, 0, this);
  graphicsViewPrimary->setGeometry(10, 180, 10 + 352, 180 + 288);
  //graphicsViewPrimary->fitInView(0, 0, 352, 288, Qt::KeepAspectRatio);
  graphicsViewPrimary->setFixedSize(width, height);
  graphicsViewPrimary->setSceneRect(0, 0, width, height);
  graphicsViewPrimary->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  graphicsViewPrimary->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  graphicsViewPrimary->resize(352, 288);
  this->layout()->addWidget(graphicsViewPrimary);

  // Right video
  graphicsViewSecondary = new MyGraphicsView(playlist, SECONDARY_LOCATION, 1, this);
  graphicsViewSecondary->setGeometry(370, 180, 370 + 352, 180 + 288);
  graphicsViewSecondary->resize(352, 288);
  this->layout()->addWidget(graphicsViewSecondary);

  primList = QList<QImage>();
  secList = QList<QImage>();

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

void MainWindow::on_sliderLeft_changed(int value)
{
    qDebug() << "currentPrimaryFrame = " << value;
    currentPrimaryFrame = value;

    // Update boundaries
    ui->frameCountLeft->setText(QString::number(value));
    ui->frameCountLeft->setText(QString::number(value));
    currentPrimaryFrame = value;

    graphicsViewPrimary->updateBoundary(currentPrimaryFrame);

    if ((segmentIndexPrimary * static_cast<unsigned int>(200)) + static_cast<unsigned int>(ui->horizontalSliderLeft->value()) >= static_cast<unsigned int>(primList.size())) { return; }

    qDebug() << "on slider left changed";
    pixMapPrim = new QGraphicsPixmapItem(QPixmap::fromImage(primList.at((segmentIndexPrimary * 200) + ui->horizontalSliderLeft->value())));
    graphicsViewPrimary->scene->addItem(pixMapPrim);
    //  graphicsViewPrimary->scene->addRect(QRectF(0, 0, 150, 150));
    graphicsViewPrimary->updateBoundary(currentPrimaryFrame);
    graphicsViewPrimary->pixMapPrim = pixMapPrim;
    ui->frameCountLeft->setText(QString::number((segmentIndexPrimary * 200) + ui->horizontalSliderLeft->value()));
}

void MainWindow::on_sliderRight_changed(int value) {
      qDebug() << "currentSecondaryFrame = " << value;
      ui->frameCountRight->setText(QString::number(value));
      currentSecondaryFrame = value;
      //graphicsViewPrimary->updateBoundary(currentPrimaryFrame);

      if ((segmentIndexSecondary * 200) + ui->horizontalSliderRight->value() >= secList.size()) { return; }
      pixMapSec = new QGraphicsPixmapItem(QPixmap::fromImage(secList.at((segmentIndexSecondary * 200) + ui->horizontalSliderRight->value())));
      graphicsViewSecondary->scene->addItem(pixMapSec);
      ui->frameCountRight->setText(QString::number((segmentIndexSecondary * 200) + ui->horizontalSliderRight->value()));
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

void imageLoading(QStringList imageFileNames, QStringList constStrs,
                  QList<QImage> *images, QStringList *fileNames,
                  QStringList *fileNamesPrev)
{
  int count = 0;
  foreach (QString filename, imageFileNames)
  {
        if (count > 1000)
        {
          continue;
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
        for (int i = 0; i < 352 * 288; i++)
        {
            uchar pixel1 = data[i];
            uchar pixel2 = data[i + 1];
            uchar pixel3 = data[i + 2];
            pixels[i] = qRgb(pixel1, pixel2, pixel3);
        }

        img = img.convertToFormat(QImage::Format_RGB444); // Format_Indexed8 //Format_RGB16
        images->push_back(img);

        if (constStrs.at(1) == "primaryNextFramesButton" || constStrs.at(1) == "secondaryNextFramesButton" || constStrs.at(1) == "importPrimaryButton" || constStrs.at(1) == "importSecondaryButton")
        {
          fileNamesPrev->append(filename);
          fileNames->removeFirst();
        }
        else
        {
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

  caller == "importPrimaryButton" ? ui->primaryVideoProgressBar->show()
                                  : ui->secondaryVideoProgressBar->show();

  if (caller == "importPrimaryButton")
  {
        numberImageFilesPrimary = imageFileNames.size();
        staticConstStringsPrimary.append(directoryPath);
        staticConstStringsPrimary.append(caller);
        primaryFileNames_n = imageFileNames;
        QFuture<void> f1 = QtConcurrent::run(imageLoading, primaryFileNames_n, staticConstStringsPrimary, &primList, &primaryFileNames_n, &primaryFileNames_p);
        f1.waitForFinished();
  } else {
        numberImageFilesSecondary = imageFileNames.size();
        staticConstStringsSecondary.append(directoryPath);
        staticConstStringsSecondary.append(caller);
        secondaryFileNames_n = imageFileNames;
        QFuture<void> f2 = QtConcurrent::run(imageLoading, secondaryFileNames_n, staticConstStringsSecondary, &secList, &secondaryFileNames_n, &secondaryFileNames_p);
        f2.waitForFinished();
  }

  caller == "importPrimaryButton"
      ? ui->primaryNextFramesButton->setEnabled(true)
      : ui->secondaryNextFramesButton->setEnabled(true);
  qDebug() << "Done Loading!";
}

void MainWindow::on_primaryNextFramesButton_clicked() {
  if (primaryFileNames_n.size() != 0) {
    ui->primaryPrevFramesButton->setEnabled(true);
  }

  qDebug() << ((segmentIndexPrimary + 2) * 200);

  if ((((segmentIndexPrimary + 1) * 200)) == 1000 && primaryFileNames_p.size() == numberImageFilesPrimary) {
    ui->primaryNextFramesButton->setEnabled(false);
  }
  else if ((((segmentIndexPrimary + 1) * 200)) >= 1000)
  {
    QString caller = QObject::sender()->objectName();
    staticConstStringsPrimary[1] = caller;
    primList.clear();
    segmentIndexPrimary = 0;
    QFuture<void> loadNext = QtConcurrent::run(imageLoading, primaryFileNames_n, staticConstStringsPrimary, &primList, &primaryFileNames_n, &primaryFileNames_p);
    loadNext.waitForFinished();
  }
  else
  {
    segmentIndexPrimary++;
  }
}

void MainWindow::on_secondaryNextFramesButton_clicked() {
  if (secondaryFileNames_n.size() != 0) {
    ui->secondaryPrevFramesButton->setEnabled(true);
  }

  if ((((segmentIndexSecondary + 1) * 200)) == 1000 && secondaryFileNames_p.size() == numberImageFilesSecondary) {
    qDebug() << secondaryFileNames_p.size();
    qDebug() << numberImageFilesSecondary;
    ui->secondaryNextFramesButton->setEnabled(false);
  }
  else if ((((segmentIndexSecondary + 1) * 200)) >= 1000)
  {
    QString caller = QObject::sender()->objectName();
    staticConstStringsSecondary[1] = caller;
    secList.clear();
    segmentIndexSecondary = 0;
    QFuture<void> loadNext = QtConcurrent::run(imageLoading, secondaryFileNames_n, staticConstStringsSecondary, &secList, &secondaryFileNames_n, &secondaryFileNames_p);
    loadNext.waitForFinished();
  }
  else
  {
    segmentIndexSecondary++;
  }
}

void MainWindow::on_primaryPrevFramesButton_clicked() {
  if (primaryFileNames_p.size() == 0) {
    ui->primaryPrevFramesButton->setEnabled(false);
  }
  if (segmentIndexPrimary == 0) {
    QString caller = QObject::sender()->objectName();
    staticConstStringsPrimary[1] = caller;
    primList.clear();
    segmentIndexPrimary = (1000 / 200 - 1);
    QFuture<void> loadNext = QtConcurrent::run(imageLoading, primaryFileNames_p, staticConstStringsPrimary, &primList, &primaryFileNames_p, &primaryFileNames_n);
    loadNext.waitForFinished();
  }
  else
  {
    segmentIndexPrimary--;
  }
}

void MainWindow::on_secondaryPrevFramesButton_clicked() {
  if (secondaryFileNames_p.size() == 0) {
    ui->secondaryPrevFramesButton->setEnabled(false);
  }
  if (segmentIndexSecondary == 0) {
    QString caller = QObject::sender()->objectName();
    staticConstStringsSecondary[1] = caller;
    secList.clear();
    segmentIndexSecondary = (1000 / 200 - 1);
    QFuture<void> loadNext = QtConcurrent::run(imageLoading, secondaryFileNames_p, staticConstStringsSecondary, &secList, &secondaryFileNames_p, &secondaryFileNames_n);
    loadNext.waitForFinished();
  }
  else
  {
    segmentIndexSecondary--;
  }
}
