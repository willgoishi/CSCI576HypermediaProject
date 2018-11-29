#ifndef MYPLAYLIST_H
#define MYPLAYLIST_H

#include "myvideo.h"
#include "QVector"

class MyPlaylist
{
public:
    MyPlaylist();
    void addVideo(MyVideo video);
    // When we click to draw boundary, get the active video
    MyVideo getActiveVideo(int videoId);

private:
    QVector<MyVideo> playlist;

};

#endif // MYPLAYLIST_H
