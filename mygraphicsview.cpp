#include "mygraphicsview.h"
#include "mainwindow.h"
#include "myframe.h"
#include "myvideo.h"

#include "QVector"
#include "QtDebug"
#include <QApplication>

MyGraphicsView::MyGraphicsView(MyPlaylist myPlaylist, int graphicsLocation,
                               int videoId, QWidget *parent)
    : QGraphicsView(parent), rubberBand(nullptr) {

  // Set scene
  scene = new QGraphicsScene(this);
  this->setScene(scene);

  this->myPlaylist = myPlaylist;
  this->currentVideoId = videoId;
  this->graphicsLocation = graphicsLocation;
  this->linkColorMap.insert(0, QColor(Qt::red));
  this->linkColorMap.insert(1, QColor(Qt::blue));
}

void MyGraphicsView::mousePressEvent(QMouseEvent *ev) {
  if (graphicsLocation == SECONDARY_LOCATION) {
    return;
  }

  start = ev->pos();

  //  debugCoord("Start", start);

  if (!rubberBand)
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

  rubberBand->setGeometry(QRect(start, QSize()));
  rubberBand->show();

  QGraphicsView::mousePressEvent(ev);
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *ev) {
  if (graphicsLocation == SECONDARY_LOCATION) {
    return;
  }

  end = ev->pos();
  //  debugCoord("End", end);

  QRect rubberBandRect = QRect(start, end).normalized();
  rubberBand->setGeometry(rubberBandRect);

  QGraphicsView::mouseMoveEvent(ev);
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *ev) {
  qDebug() << "mouseReleaseEvent()";

  if (graphicsLocation == SECONDARY_LOCATION) {
    return;
  }

  QPoint releasePoint = ev->pos();

  if (releasePoint == start) {

    // Delete boundary
    MyVideo *video = myPlaylist.getVideo(currentVideoId);
    MyFrame *frame = video->getFrame(currentFrame);
    frame->removeBoundary(currentLinkId);
    updateBoundary(currentFrame);

  } else {

    rect = QRectF(start, end).normalized();
    rubberBand->hide();

    MyVideo *video = myPlaylist.getVideo(currentVideoId);
    QGraphicsRectItem *boundary = new QGraphicsRectItem(rect);

    qDebug() << "Current frame: " << currentFrame;

    video->addBoundary(currentFrame, currentLinkId, boundary);

    updateBoundary(currentFrame);
  }
  QGraphicsView::mouseReleaseEvent(ev);
}

// void MyGraphicsView::paintEvent(QPaintEvent *ev) {
//  //  qDebug() << "paintEvent()";

//  //  if (graphicsLocation == SECONDARY_LOCATION) {
//  //    return;
//  //  }

//  QGraphicsView::paintEvent(ev);
//}

// void MyGraphicsView::debugCoord(QString name, QPoint point) {
//  qDebug() << name << " point: (" << point.x() << ", " << point.y() << ")";
//}

void MyGraphicsView::updateBoundary(int frameId) {
  if (graphicsLocation == SECONDARY_LOCATION) {
    return;
  }

  qDebug() << "updateBoundary() " << frameId;

  currentFrame = frameId;

  clearBoundary();

  // Check if boundary exists for current frame
  video = myPlaylist.getVideo(currentVideoId);

  videoFrame = video->getFrame(currentFrame);

  qDebug() << "currentFrame: " << currentFrame;

  // Loop for each link
  QMapIterator<int, QGraphicsRectItem *> i(videoFrame->getLinks());

  while (i.hasNext()) {
    i.next();
    qDebug() << i.key() << ": " << i.value()->rect();

    int linkId = i.key();

    if (videoFrame->hasBoundary(linkId)) {
      showBoundary(videoFrame->getBoundary(linkId), linkId);
    }
  }
}

void MyGraphicsView::clearBoundary() {
  qDebug() << "clearBoundary()";

  scene = new QGraphicsScene(this);

  if (pixMapPrim && graphicsLocation == PRIMARY_LOCATION) {
    qDebug() << "Primary clearBoundary()";
    scene->addItem(pixMapPrim);
  }
  if (pixMapSec && graphicsLocation == SECONDARY_LOCATION) {
    qDebug() << "Secondary clearBoundary()";
    scene->addItem(pixMapSec);
  }

  this->setScene(scene);

  if (rubberBand) {
    rubberBand->hide();
  }
}

void MyGraphicsView::showBoundary(QGraphicsRectItem *boundary, int linkId) {
  qDebug() << "showBoundary()";

  scene->addRect(boundary->rect(), QPen(linkColorMap[linkId]));
}

void MyGraphicsView::updateCurrentLink(int linkId) {
  currentLinkId = linkId;

  qDebug() << "Current link updated to: " << currentLinkId;
}
