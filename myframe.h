#ifndef MYFRAME_H
#define MYFRAME_H

#include <QRect>
#include <QRectF>
#include <QVector>
#include <QMap>
#include <QGraphicsRectItem>
#include <QJsonObject>

class MyFrame
{

public:
    MyFrame(int frameCount, QString videoTitle);
    void addBoundary(int linkId, QGraphicsRectItem* boundary);
    bool hasBoundary(int linkId);
    void removeBoundary(int linkId);
    int getFrameCount();
    QGraphicsRectItem* getBoundary(int linkId);
    QMap<int, QGraphicsRectItem*> getLinks();
    QJsonObject toJson();
    QString videoTitle;

private:
    int frameCount;
    int linkId;
    QMap<int, QGraphicsRectItem*> links;
};

#endif // MYFRAME_H
