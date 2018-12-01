#include "myframe.h"

#include <QtDebug>
#include <QRectF>
#include <QGraphicsRectItem>

MyFrame::MyFrame(int frameCount)
{
    this->frameCount = frameCount;
    qDebug() << "frameCount: " << frameCount;
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

QGraphicsRectItem* MyFrame::getBoundary(int linkId)
{
    return links[linkId];
}

QMap<int, QGraphicsRectItem*> MyFrame::getLinks()
{
    return links;
}


MyFrame::MyFrame(int frameCount, int videoId, int linkId, QGraphicsRectItem* boundary)
{
    qDebug() << "frameCount: " << frameCount;
    qDebug() << "videoId: " << videoId;
    qDebug() << "linkId: " << linkId;
}
