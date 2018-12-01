#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include "myframe.h"
#include "myvideo.h"
#include "myplaylist.h"

#include <QVector>
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
    explicit MyGraphicsView(MyPlaylist myPlaylist, int videoId, QWidget *parent = nullptr);
    void updateBoundary(int frame);
    void clearBoundary();
    void showBoundary(QRect boundary);

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
       MyVideo myVideo;
       MyPlaylist myPlaylist;

       // Lets us know where to store the rectangle info
       int currentFrame = 0;
       int currentLinkId = 0;
       int currentVideoId = 0;
};

#endif // MYGRAPHICSVIEW_H
