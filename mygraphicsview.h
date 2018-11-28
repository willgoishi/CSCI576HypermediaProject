#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include "mainwindow.h"

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>
#include <QRubberBand>
#include <QLabel>

class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit MyGraphicsView(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *ev);
    void debugCoord(QString name, QPoint point);

private:
       QRubberBand *rubberBand;
       QPoint start;
       QPoint end;
       QRect rect;
       QRect lastRect;
       QGraphicsScene *scene;
       QGraphicsRectItem *rectangle;

       // Lets us know where to store the rectangle info
       int currentFrame = 0;
       int currentLink = 0;
       int currentVideo = 0;
};

#endif // MYGRAPHICSVIEW_H
