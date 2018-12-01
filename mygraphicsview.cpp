#include "mainwindow.h"
#include "mygraphicsview.h"
#include "myframe.h"
#include "myvideo.h"

#include <QApplication>
#include "QtDebug"
#include "QVector"

MyGraphicsView::MyGraphicsView(MyPlaylist myPlaylist, int videoId, QWidget *parent)
    : QGraphicsView (parent),
      rubberBand(nullptr)
{
    this->myPlaylist = myPlaylist;
    this->currentVideoId = videoId;
}

void MyGraphicsView::mousePressEvent(QMouseEvent *ev)
{
    start = ev->pos();

    debugCoord("Start", start);

    if(!rubberBand)
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    rubberBand->setGeometry(QRect(start, QSize()));
    rubberBand->show();

    QGraphicsView::mousePressEvent(ev);
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *ev)
{
    end = ev->pos();
    debugCoord("End", end);

    QRect rubberBandRect = QRect(start, end).normalized();
    rubberBand->setGeometry(rubberBandRect);

    QGraphicsView::mouseMoveEvent(ev);
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << "mouseReleaseEvent";

    QPoint releasePoint = ev->pos();

    if (releasePoint == start) {

        clearBoundary();

    } else {

        rect = QRect(start, end).normalized();
//        this->viewport()->update();
        rubberBand->hide();
        showBoundary(rect);

        qDebug() << "Add Rectangle to vector";

        MyVideo video = myPlaylist.getActiveVideo(currentVideoId);

        video.addBoundary(currentFrame, currentLinkId, rect);

    }
    QGraphicsView::mouseReleaseEvent(ev);
}

void MyGraphicsView::paintEvent(QPaintEvent *ev)
{
    qDebug() << "paintEvent";
    QGraphicsView::paintEvent(ev);

//    if (!rect.isNull())
//        showBoundary(rect);
}

void MyGraphicsView::debugCoord(QString name, QPoint point)
{
    qDebug() << name << " point: (" << point.x() << ", " << point.y() << ")";
}

void MyGraphicsView::updateBoundary(int frameId)
{
    currentFrame = frameId;

    qDebug() << "Update boundary";

    clearBoundary();

    // Check if boundary exists for current frame
    MyVideo video = myPlaylist.getActiveVideo(currentVideoId);

    qDebug() << "updateBoundary(), frame =" << currentFrame;

    MyFrame* frame = video.getFrame(currentFrame);

    qDebug() << "currentLinkId: " << currentLinkId;

    if (frame->hasBoundary(currentLinkId)) {
        qDebug() << "Current frame has boundary";

        QRect boundary = frame->getBoundary(currentLinkId);

        qDebug() << "Boundary" << boundary.topLeft().x();
        qDebug() << "Boundary" << boundary.topLeft().y();
        qDebug() << "Boundary" << boundary.bottomLeft().x();
        qDebug() << "Boundary" << boundary.bottomLeft().y();


        showBoundary(frame->getBoundary(currentLinkId));

    } else {
        qDebug() << "Current frame has no boundary";
    }
}

void MyGraphicsView::clearBoundary()
{
    qDebug() << "Clear Rectangle!";

    scene = new QGraphicsScene(this);
    this->setScene(scene);

    rect = QRect();
    this->viewport()->update();
    if(rubberBand) {
        rubberBand->hide();
    }
}

void MyGraphicsView::showBoundary(QRect boundary)
{
    scene = new QGraphicsScene(this);
    this->setScene(scene);
    scene->addRect(boundary);

//    QPainter painter(this->viewport());
//    painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine, Qt::SquareCap,
//                        Qt::RoundJoin));
//    painter.drawRect(boundary);
//    update();
}



