#ifndef MYFRAME_H
#define MYFRAME_H

#include <QRect>
#include <QRectF>
#include <QVector>
#include <QMap>
#include <QGraphicsRectItem>

class MyFrame
{

public:
    MyFrame(int frameCount, int videoId, int linkId, QGraphicsRectItem* boundary);
    MyFrame(int frameCount);

    void addBoundary(int linkId, QGraphicsRectItem* boundary);
    bool hasBoundary(int linkId);
    void removeBoundary(int linkId);
    QGraphicsRectItem* getBoundary(int linkId);

    QMap<int, QGraphicsRectItem*> getLinks();

private:
    int frameCount;
    int videoId;
    int linkId;
    QMap<int, QGraphicsRectItem*> links;
};

#endif // MYFRAME_H
