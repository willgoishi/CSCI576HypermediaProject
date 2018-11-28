#ifndef MYNODE_H
#define MYNODE_H

#include <QRect>

class MyNode
{

public:
    MyNode(int frameCount, int videoCount, int linkCount, QRect boundary);

private:
    int frameCount;
    int videoCount;
    int linkCount;
    QRect boundary;

};

#endif // MYNODE_H
