#ifndef MYVIDEO_H
#define MYVIDEO_H

#include "myframe.h"

#include "QVector"
#include "QRect"
#include "QRectF"
#include "QJsonArray"
#include "QJsonObject"
#include "QtCore"


class MyVideo
{
public:
    MyVideo();
    MyVideo(QString videoTitle);
    void addFrame(MyFrame* frame);
    void addBoundary(int frameCount, int linkId, QGraphicsRectItem* boundary);
    MyFrame* getFrame(int frameCount);
    int getFirstFrameWithBoundaryFromLinkId(int linkId);
    bool hasFirstFrameWithBoundaryFromLinkId(int linkId);

    void addHyperlinkTarget(int linkId, MyFrame* frame);
    MyFrame* getHyperlinkTarget(int linkId);

    int getVideoId();
    QVector<MyFrame*> myVideo;
    QString videoTitle;

    // LinkId maps to frame
    QMap<int, MyFrame*> hyperlinks;

    QJsonObject toJson();

};

#endif // MYVIDEO_H
