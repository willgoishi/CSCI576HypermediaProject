#ifndef MYPLAYLIST_H
#define MYPLAYLIST_H

#include "QVector"
#include "myvideo.h"
#include <QMediaPlaylist>

#define PRIMARY_LOCATION 0
#define SECONDARY_LOCATION 1
#define PLAYER_LOCATION 2

class MyPlaylist {
public:
  MyPlaylist();
  void addVideo(MyVideo *video);
  // When we click to draw boundary, get the active video
  MyVideo *getVideo(int videoId);
  QVector<MyVideo *> playlist;

private:
};

#endif // MYPLAYLIST_H
