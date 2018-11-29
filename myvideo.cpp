#include "myvideo.h"

#include "QtDebug"

MyVideo::MyVideo()
{

}

void MyVideo::addFrame(MyFrame *frame)
{
    myVideo.append(frame);
}

void MyVideo::addBoundary(int frameCount, int linkId, QRect boundary)
{
    qDebug() << "MyVideo::addBoundary";

    MyFrame* frame = myVideo[frameCount];

    qDebug() << "Got frame";

    frame->addBoundary(linkId, boundary);
}

MyFrame* MyVideo::getFrame(int frameCount)
{
    return myVideo[frameCount];
}
