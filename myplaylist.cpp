#include "myplaylist.h"
#include "qmediaplaylist.h"

MyPlaylist::MyPlaylist()
{

}

void MyPlaylist::addVideo(MyVideo* video)
{
    playlist.append(video);
}

MyVideo* MyPlaylist::getVideo(int videoId)
{
    return playlist[videoId];
}
