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
    MyFrame(int frameCount, int videoId, int linkId, QGraphicsRectItem* boundary);
    MyFrame(int frameCount, int videoId);

    void addBoundary(int linkId, QGraphicsRectItem* boundary);
    bool hasBoundary(int linkId);
    void removeBoundary(int linkId);
    int getFrameCount();
    QGraphicsRectItem* getBoundary(int linkId);

    void addHyperlinkTarget(int linkId, MyFrame* frame);
    MyFrame *getHyperlinkTarget(int linkId);

    int getVideoId();
    QMap<int, QGraphicsRectItem*> getLinks();
    QJsonObject toJson();

private:
    int frameCount;
    int videoId;
    int linkId;
    QMap<int, QGraphicsRectItem*> links;
    QMap<int, MyFrame*> hyperlinks;
    MyFrame* hyperlinkTarget;
};

#endif // MYFRAME_H
