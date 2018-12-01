#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

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
           QGraphicsScene *scene;

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
       //QGraphicsScene *scene;
       QGraphicsRectItem *rectangle;

};

#endif // MYGRAPHICSVIEW_H
