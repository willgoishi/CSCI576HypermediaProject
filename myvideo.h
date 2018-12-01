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
    MyVideo(int id);
    void addFrame(MyFrame* frame);
    void addBoundary(int frameCount, int linkId, QGraphicsRectItem* boundary);
    MyFrame* getFrame(int frameCount);
    int getFirstFrameWithBoundaryFromLinkId(int linkId);
    bool hasFirstFrameWithBoundaryFromLinkId(int linkId);


    int getVideoId();
    QVector<MyFrame*> myVideo;


private:
    int videoId;
};

#endif // MYVIDEO_H
