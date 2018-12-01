#ifndef MYVIDEO_H
#define MYVIDEO_H

#include "myframe.h"
#include "QVector"
#include "QRect"
#include "QRectF"

class MyVideo
{
public:
    MyVideo();
    void addFrame(MyFrame* frame);
    void addBoundary(int frameCount, int linkId, QGraphicsRectItem* boundary);
    MyFrame* getFrame(int frameCount);

private:
    QVector<MyFrame*> myVideo;
};

#endif // MYVIDEO_H
