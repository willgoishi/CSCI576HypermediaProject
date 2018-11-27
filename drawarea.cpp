#include "drawArea.h"

#include <QtWidgets>
#include <QDebug>

DrawArea::DrawArea(QWidget *parent)
    : QWidget(parent)
{
    qDebug() << "drawarea.cpp";
    setAttribute(Qt::WA_StaticContents);
//    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
//    setAttribute(Qt::WA_NoSystemBackground);
//    setAttribute(Qt::WA_TranslucentBackground);
//    setAttribute(Qt::WA_PaintOnScreen);
//    setAttribute(Qt::WA_TransparentForMouseEvents);
//    setStyleSheet("background-color: rgba(22,22,22,1)");
//    setStyleSheet("border:1px solid rgb(0, 255, 0);");

    modified = false;
    scribbling = false;
    myPenWidth = 2;
    myPenColor = Qt::blue;
}

void DrawArea::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "DrawArea::mousePressEvent()";

    if (event->button() == Qt::LeftButton) {
        initPoint = event->pos();
        lastPoint = event->pos();
        scribbling = true;
    }
}

void DrawArea::mouseMoveEvent(QMouseEvent *event)
{
//    qDebug() << "mouseMoveEvent";

    if ((event->buttons() & Qt::LeftButton) && scribbling)
        clearImage();
        drawRect(initPoint, event->pos());
}

void DrawArea::mouseReleaseEvent(QMouseEvent *event)
{
//    qDebug() << "mouseReleaseEvent";

    if (event->button() == Qt::LeftButton && scribbling) {

        QPoint startPoint = initPoint;
        QPoint endPoint = event->pos();

        int startX = startPoint.x();
        int startY = startPoint.y();
        int endX = endPoint.x();
        int endY = endPoint.y();

        qDebug() << "Draw Final Rect:";
        qDebug() << "Start point: (" << startX << ", " << startY << ")";
        qDebug() << "End point: (" << endX << ", " << endY << ")";

        drawRect(initPoint, event->pos());
        scribbling = false;
    }
}

void DrawArea::paintEvent(QPaintEvent *event)
{
//    qDebug() << "paintEvent";

    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);
}


void DrawArea::resizeEvent(QResizeEvent *event)
{
    qDebug() << "resizeEvent";

    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void DrawArea::clearImage()
{
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
}

void DrawArea::drawLineTo(const QPoint &endPoint)
{
    qDebug() << "drawLineTo";


    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(lastPoint, endPoint);
    modified = true;

    int rad = (myPenWidth / 2) + 2;
    update(QRect(lastPoint, endPoint).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
}


void DrawArea::drawRect(const QPoint &startPoint, const QPoint &endPoint)
{
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));

    // Draw rectangle
    QRect rect = QRect(endPoint, startPoint);
    painter.drawRect(rect);
    painter.end();
    update();

    // Set vars
    modified = true;
    lastPoint = endPoint;
}

void DrawArea::resizeImage(QImage *image, const QSize &newSize)
{
    qDebug() << "resizeImage";

    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}
