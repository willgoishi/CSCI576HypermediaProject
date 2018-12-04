#include "myvideo.h"

#include "QtDebug"
#include "QVectorIterator"

MyVideo::MyVideo()
{

}

MyVideo::MyVideo(int id)
{
    videoId = id;
}

void MyVideo::addFrame(MyFrame *frame)
{
    myVideo.append(frame);
}

void MyVideo::addBoundary(int frameCount, int linkId, QGraphicsRectItem* boundary)
{
    qDebug() << "MyVideo::addBoundary";

    MyFrame* frame = myVideo[frameCount];

    qDebug() << "Got frame";

    frame->addBoundary(linkId, boundary);

    // Also interpolate frames from current frame to previous non-null frame
    if (frameCount > 0) {

        bool hasBoundary = false;
        int startFrame = 0;
        int endFrame = frameCount;

        for (int i = frameCount - 1; i >= 0; i--) {
            frame = myVideo[i];

            if (frame->hasBoundary(linkId)) {
                hasBoundary = true;
                startFrame = i;
                break;
            }
        }

        qDebug() << "startFrame: " << startFrame;
        qDebug() << "endFrame: " << endFrame;

        // If we found the frame, interpolate frame count
        if (hasBoundary) {

            QGraphicsRectItem* startBoundary = myVideo[startFrame]->getBoundary(linkId);
            QGraphicsRectItem* endBoundary = myVideo[endFrame]->getBoundary(linkId);

            QRectF startRect = startBoundary->rect();
            QRectF endRect = endBoundary->rect();

            qreal startX1;
            qreal startY1;
            qreal startX2;
            qreal startY2;

            startRect.getCoords(&startX1, &startY1, &startX2, &startY2);

            qreal endX1;
            qreal endY1;
            qreal endX2;
            qreal endY2;

            endRect.getCoords(&endX1, &endY1, &endX2, &endY2);

            qDebug() << "startX1: " << startX1;
            qDebug() << "startY1: " << startY1;
            qDebug() << "startX2: " << startX2;
            qDebug() << "startY2: " << startY2;


            qreal dX1 = (endX1 - startX1) / (endFrame - startFrame);
            qreal dY1 = (endY1 - startY1) / (endFrame - startFrame);
            qreal dX2 = (endX2 - startX2) / (endFrame - startFrame);
            qreal dY2 = (endY2 - startY2) / (endFrame - startFrame);


            for (int i = startFrame + 1; i < endFrame; i++) {

                qreal interX1 = startX1 + (i - startFrame) * dX1;
                qreal interY1 = startY1 + (i - startFrame) * dY1;
                qreal interX2 = startX2 + (i - startFrame) * dX2;
                qreal interY2 = startY2 + (i - startFrame) * dY2;

                QRectF interRect = QRectF(
                            QPointF(interX1, interY1),
                            QPointF(interX2, interY2)
                        );
                QGraphicsRectItem* boundary = new QGraphicsRectItem(interRect);
                frame = myVideo[i];
                frame->addBoundary(linkId, boundary);
            }
        }
    }

}

MyFrame* MyVideo::getFrame(int frameCount)
{
    return myVideo[frameCount];
}

int MyVideo::getFirstFrameWithBoundaryFromLinkId(int linkId)
{
    QVectorIterator<MyFrame*> i(myVideo);
    while(i.hasNext()) {
        MyFrame* frame = i.next();
        if (frame->hasBoundary(linkId)) {
            return frame->getFrameCount();
        }
    }
    return 0;
}

bool MyVideo::hasFirstFrameWithBoundaryFromLinkId(int linkId)
{
    QVectorIterator<MyFrame*> i(myVideo);
    while(i.hasNext()) {
        MyFrame* frame = i.next();
        if (frame->hasBoundary(linkId)) {
            return true;
        }
    }

    return false;
}

int MyVideo::getVideoId()
{
    return videoId;
}

QJsonObject MyVideo::toJson()
{
    return {
        { "videoId", videoId }
    };
}

















