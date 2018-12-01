#include "myframe.h"

#include <QtDebug>
#include <QRectF>
#include <QGraphicsRectItem>

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

void MyFrame::addHyperlinkTarget(int linkId, MyFrame *frame)
{
    qDebug() << "Add hyperlink target";

    hyperlinks.insert(linkId, frame);
}

MyFrame *MyFrame::getHyperlinkTarget(int linkId)
{
    return hyperlinks[linkId];
}

int MyFrame::getVideoId()
{
    return videoId;
}

QMap<int, QGraphicsRectItem*> MyFrame::getLinks()
{
    return links;
}

