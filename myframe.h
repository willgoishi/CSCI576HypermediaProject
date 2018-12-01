#ifndef MYFRAME_H
#define MYFRAME_H

#include <QRect>
#include <QVector>
#include <QMap>

class MyFrame
{

public:
    MyFrame(int frameCount, int videoId, int linkId, QRect boundary);
    MyFrame(int frameCount);

    void addBoundary(int linkId, QRect boundary);
    bool hasBoundary(int linkId);
    QRect getBoundary(int linkId);

private:
    int frameCount;
    int videoId;
    int linkId;
    QMap<int, QRect> links;
};

#endif // MYFRAME_H
