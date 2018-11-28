#include "mywidget.h"
#include <QWidget>

mywidget::mywidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
//    setAttribute(Qt::WA_TranslucentBackground, true);
    this->setStyleSheet("background-color: rgba(0,0,0,0);");
}
