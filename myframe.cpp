#include "myframe.h"

#include <QtDebug>
#include <QRect>

MyFrame::MyFrame(int frameCount)
{
    this->frameCount = frameCount;
    qDebug() << "frameCount: " << frameCount;
}

void MyFrame::addBoundary(int linkId, QRect boundary)
{
    links.insert(linkId, boundary);

    qDebug() << "MyFrame::addBoundary";
}

bool MyFrame::hasBoundary(int linkId)
{
    return links.contains(linkId);
}

QRect MyFrame::getBoundary(int linkId)
{
    return links[linkId];
}


MyFrame::MyFrame(int frameCount, int videoId, int linkId, QRect boundary)
{
    qDebug() << "frameCount: " << frameCount;
    qDebug() << "videoId: " << videoId;
    qDebug() << "linkId: " << linkId;
}
