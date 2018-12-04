#include "videowidget.h"
#include "drawarea.h"
#include "mywidget.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QPoint>
#include <QRubberBand>
#include <QVBoxLayout>
#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent)
    : QVideoWidget(parent)
{
//    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

//    QPalette p = palette();
//    p.setColor(QPalette::Window, Qt::black);
//    setPalette(p);

//    setAttribute(Qt::WA_OpaquePaintEvent);

    // Add drawarea


//    drawArea = new DrawArea(this);
//    drawArea->setGeometry(10, 180, 10 + 352, 180 + 288);
//    drawArea->setStyleSheet("border:1px solid rgb(0, 255, 0);background-color: rgba(0, 0, 0, 0);");
//    drawArea->resize(352, 288/2);
//    this->layout()->addWidget(drawArea);

//    QWidget* myWidget = new MyWidget;
//    QGridLayout *layout = new QGridLayout;
//    layout->addWidget(myWidget, 0, 0);

//    parent->layout()->addWidget(m_myWidget);

//    auto layout = new QVBoxLayout;
//    layout->setSpacing(0);
//    layout->setMargin(0);
//    layout->addWidget(m_myWidget);
//    this->setLayout(layout);

    setAttribute(Qt::WA_StaticContents);

    modified = false;
    scribbling = false;
    myPenWidth = 2;
    myPenColor = Qt::blue;

}


void VideoWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "DrawArea::mousePressEvent()";

    if (event->button() == Qt::LeftButton) {
        initPoint = event->pos();
        lastPoint = event->pos();
        scribbling = true;
    }
}

void VideoWidget::mouseMoveEvent(QMouseEvent *event)
{
//    qDebug() << "mouseMoveEvent";

    if ((event->buttons() & Qt::LeftButton) && scribbling)
        clearImage();
        drawRect(initPoint, event->pos());
}

void VideoWidget::mouseReleaseEvent(QMouseEvent *event)
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

void VideoWidget::paintEvent(QPaintEvent *event)
{
    qDebug() << "paintEvent";

    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);
}


void VideoWidget::resizeEvent(QResizeEvent *event)
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

void VideoWidget::clearImage()
{
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
}

void VideoWidget::drawLineTo(const QPoint &endPoint)
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


void VideoWidget::drawRect(const QPoint &startPoint, const QPoint &endPoint)
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

void VideoWidget::resizeImage(QImage *image, const QSize &newSize)
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
