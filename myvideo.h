#ifndef MYVIDEO_H
#define MYVIDEO_H

#include "myframe.h"
#include "QVector"
#include "QRect"

class MyVideo
{
public:
    MyVideo();
    void addFrame(MyFrame* frame);
    void addBoundary(int frameCount, int linkId, QRect boundary);
    MyFrame* getFrame(int frameCount);

private:
    QVector<MyFrame*> myVideo;
};

#endif // MYVIDEO_H
