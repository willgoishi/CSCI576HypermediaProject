#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>

class DrawArea : public QWidget
{
    Q_OBJECT

public:
    DrawArea(QWidget *parent = nullptr);
    bool isModified() const { return modified; }
    QColor penColor() const { return myPenColor; }
    int penWidth() const { return myPenWidth; }

public slots:
    void clearImage();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;


protected:

private:
    void drawLineTo(const QPoint &endPoint);
    void drawRect(const QPoint &startPoint, const QPoint &endPoint);
    void resizeImage(QImage *image, const QSize &newSize);

    bool modified;
    bool scribbling;
    int myPenWidth;
    QColor myPenColor;
    QImage image;
    QPoint initPoint;
    QPoint lastPoint;
};

#endif
