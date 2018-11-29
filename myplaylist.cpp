#include "myplaylist.h"

MyPlaylist::MyPlaylist()
{

}

void MyPlaylist::addVideo(MyVideo video)
{
    playlist.append(video);
}

MyVideo MyPlaylist::getActiveVideo(int videoId)
{
    return playlist[videoId];
}
