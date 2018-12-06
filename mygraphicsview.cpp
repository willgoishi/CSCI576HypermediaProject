#include "mygraphicsview.h"
#include "mainwindow.h"
#include "myframe.h"
#include "myvideo.h"

#include "QVector"
#include "QtDebug"
#include <QApplication>

MyGraphicsView::MyGraphicsView(MyPlaylist *&myPlaylist, int graphicsLocation,
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

  qDebug() << "graphicsLocation" << graphicsLocation;

  if (graphicsLocation == SECONDARY_LOCATION) {
    return;
  }

  start = ev->pos();

  if (graphicsLocation == PLAYER_LOCATION) {
    qDebug() << "Clicked: (" << start.x() << "," << start.y();

    // Check if coordinate clicked is inside rectangle
    // Check if boundary exists for current frame
    video = myPlaylist->getVideo(currentVideoId);
    videoFrame = video->getFrame(currentFrame);

    // Loop for each link
    QMapIterator<int, QGraphicsRectItem *> i(videoFrame->getLinks());

    qDebug() << "Boundaries:";

    while (i.hasNext()) {
      i.next();
      qDebug() << i.key() << ": " << i.value()->rect();

      int linkId = i.key();

      if (videoFrame->hasBoundary(linkId)) {
        QRectF rect = videoFrame->getBoundary(linkId)->rect();

        if (rect.contains(start)) {
          MyFrame *targetFrame = video->getHyperlinkTarget(linkId);
          qDebug() << "\nPoint inside boundaries!\n";
          qDebug() << "Target video -> " << targetFrame->videoTitle;
          qDebug() << "Target frame -> " << targetFrame->getFrameCount();
        }
      }
    }

    return;
  }

  if (!rubberBand)
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

  rubberBand->setGeometry(QRect(start, QSize()));
  rubberBand->show();

  QGraphicsView::mousePressEvent(ev);
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *ev) {
  if (graphicsLocation == SECONDARY_LOCATION ||
      graphicsLocation == PLAYER_LOCATION) {
    return;
  }

  end = ev->pos();

  QRect rubberBandRect = QRect(start, end).normalized();
  rubberBand->setGeometry(rubberBandRect);

  QGraphicsView::mouseMoveEvent(ev);
}

void MyGraphicsView::paintEvent(QPaintEvent *ev) {
  qDebug() << "paintEvent()";

  QGraphicsView::paintEvent(ev);
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *ev) {
  if (graphicsLocation == SECONDARY_LOCATION ||
      graphicsLocation == PLAYER_LOCATION) {
    return;
  }

  qDebug() << "mouseReleaseEvent()";

  QPoint releasePoint = ev->pos();

  if (releasePoint == start) {

    // Delete boundary
    MyVideo *video = myPlaylist->getVideo(currentVideoId);
    MyFrame *frame = video->getFrame(currentFrame);
    frame->removeBoundary(currentLinkId);
    updateBoundary(currentFrame);

  } else {

    rect = QRectF(start, end).normalized();
    rubberBand->hide();

    MyVideo *video = myPlaylist->getVideo(currentVideoId);
    QGraphicsRectItem *boundary = new QGraphicsRectItem(rect);

    qDebug() << "Current frame: " << currentFrame;

    video->addBoundary(currentFrame, currentLinkId, boundary);

    updateBoundary(currentFrame);
  }
  QGraphicsView::mouseReleaseEvent(ev);
}

void MyGraphicsView::updateBoundary(int frameId) {
  if (graphicsLocation == SECONDARY_LOCATION) {
    return;
  }

  qDebug() << "updateBoundary() " << frameId;

  currentFrame = frameId;

  clearBoundary();

  qDebug() << "currentVideoId: " << currentVideoId;

  // Check if boundary exists for current frame
  video = myPlaylist->getVideo(currentVideoId);

  qDebug() << "current frame " << currentFrame;

  videoFrame = video->getFrame(currentFrame);

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
  if (pixMapPlayer && graphicsLocation == PLAYER_LOCATION) {
    qDebug() << "Player clearBoundary()";
    scene->addItem(pixMapPlayer);
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
