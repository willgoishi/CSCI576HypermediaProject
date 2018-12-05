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

  debugCoord("Start", start);

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
  debugCoord("End", end);

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
    video->addBoundary(currentFrame, currentLinkId, boundary);

    updateBoundary(currentFrame);
  }
  QGraphicsView::mouseReleaseEvent(ev);
}

void MyGraphicsView::paintEvent(QPaintEvent *ev) {
  if (graphicsLocation == SECONDARY_LOCATION) {
    return;
  }

  QGraphicsView::paintEvent(ev);
}

void MyGraphicsView::debugCoord(QString name, QPoint point) {
  //    qDebug() << name << " point: (" << point.x() << ", " << point.y() <<
  //    ")";
}

void MyGraphicsView::updateBoundary(int frameId) {
  qDebug() << "Update boundary";

  currentFrame = frameId;

  clearBoundary();

  // Check if boundary exists for current frame
  video = myPlaylist.getVideo(currentVideoId);

  MyFrame *frame = video->getFrame(currentFrame);

  // Loop for each link
  QMapIterator<int, QGraphicsRectItem *> i(frame->getLinks());

  while (i.hasNext()) {
    i.next();
    qDebug() << i.key() << ": " << i.value()->rect();

    int linkId = i.key();

    if (frame->hasBoundary(linkId)) {
      showBoundary(frame->getBoundary(linkId), linkId);
    }
  }
}

void MyGraphicsView::clearBoundary() {
  qDebug() << "clearBoundary()";

  scene = new QGraphicsScene(this);
  this->setScene(scene);
  this->viewport()->update();

  if (rubberBand) {
    rubberBand->hide();
  }
}

void MyGraphicsView::showBoundary(QGraphicsRectItem *boundary, int linkId) {
  qDebug() << "showBoundary()";

  if (!scene) {
    scene = new QGraphicsScene(this);
  }
  this->setScene(scene);
  scene->addRect(boundary->rect(), QPen(linkColorMap[linkId]));

  QList<QGraphicsItem *> items = scene->items();

  foreach (QGraphicsItem *item, items) { qDebug() << "Item!"; }
}

void MyGraphicsView::updateCurrentLink(int linkId) {
  currentLinkId = linkId;

  qDebug() << "Current link updated to: " << currentLinkId;
}
