#include "myframe.h"

#include <QtDebug>
#include <QRectF>
#include <QGraphicsRectItem>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVariantMap>

MyFrame::MyFrame(int frameCount, int videoId, int linkId, QGraphicsRectItem* boundary)
{
    qDebug() << "frameCount: " << frameCount;
    qDebug() << "videoId: " << videoId;
    qDebug() << "linkId: " << linkId;
}

MyFrame::MyFrame(int frameCount, int videoId)
{
    this->frameCount = frameCount;
    this->videoId = videoId;
//    qDebug() << "frameCount: " << frameCount;
}

void MyFrame::addBoundary(int linkId, QGraphicsRectItem* boundary)
{
    links.insert(linkId, boundary);

    qDebug() << "MyFrame::addBoundary";
}

bool MyFrame::hasBoundary(int linkId)
{
    return links.contains(linkId);
}

void MyFrame::removeBoundary(int linkId)
{
    links.remove(linkId);
}

int MyFrame::getFrameCount()
{
   return frameCount;
}

QGraphicsRectItem* MyFrame::getBoundary(int linkId)
{
    return links[linkId];
}

//void MyFrame::addHyperlinkTarget(int linkId, MyFrame *frame)
//{
//    qDebug() << "Add hyperlink target";

//    hyperlinks.insert(linkId, frame);
//}

//MyFrame *MyFrame::getHyperlinkTarget(int linkId)
//{
//    return hyperlinks[linkId];
//}

int MyFrame::getVideoId()
{
    return videoId;
}

QMap<int, QGraphicsRectItem*> MyFrame::getLinks()
{
    return links;
}

QJsonObject MyFrame::toJson()
{
    QJsonObject obj;
    obj.insert("frameCount", getFrameCount());

    QJsonArray boundaries;

    QMapIterator<int, QGraphicsRectItem*> i(links);

    while (i.hasNext()) {
        i.next();

        QJsonObject boundaryObj;

        QGraphicsRectItem* rect = i.value();
        QJsonArray rectCoords;

        qreal x;
        qreal y;
        qreal w;
        qreal h;

        rect->rect().getCoords(&x, &y, &w, &h);
        rectCoords.push_back(x);
        rectCoords.push_back(y);
        rectCoords.push_back(w);
        rectCoords.push_back(h);

        boundaryObj.insert("linkId", i.key());
        boundaryObj.insert("coords", rectCoords);
//        boundaryObj.insert("targ")


        boundaries.push_back(boundaryObj);
    }

//    QJsonDocument json = QJsonDocument::fromVariant(linksMap);

    obj.insert("boundaries", boundaries);
    obj.insert("videoId", videoId);

    return obj;
}

