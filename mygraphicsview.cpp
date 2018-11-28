#include "mainwindow.h"
#include "mygraphicsview.h"

#include <QApplication>
#include "QtDebug"

MyGraphicsView::MyGraphicsView(QWidget *parent)
    : QGraphicsView (parent), rubberBand(nullptr)
{
//    scene = new QGraphicsScene(this);
//    this->setScene(scene);
//    QPen pen(Qt::black);
//    rectangle = scene->addRect(10, 10, 100, 100, pen);
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

    rect = QRect(start, end).normalized();
//    update();
    this->viewport()->update();
    rubberBand->hide();

    // Set frame info


    QGraphicsView::mouseReleaseEvent(ev);
}

void MyGraphicsView::paintEvent(QPaintEvent *ev)
{
    qDebug() << "paintEvent";
    QGraphicsView::paintEvent(ev);

    QPainter painter(this->viewport());
    painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine, Qt::SquareCap,
                        Qt::RoundJoin));

    if (!rect.isNull())
        painter.drawRect(rect);
        update();
}

void MyGraphicsView::debugCoord(QString name, QPoint point)
{
    qDebug() << name << " point: (" << point.x() << ", " << point.y() << ")";
}


