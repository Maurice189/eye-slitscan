#ifndef SLITSCANVIEW_H
#define SLITSCANVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>

#include "slitscanitem.h"

class SlitscanView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit SlitscanView(QWidget *parent = 0);
    void setZoomingEnabled(bool zoomingEnabled);
    void setVisibleArea(int xStart, int xEnd);
    void setVisibleAreaToMaximum(bool maxVisibleArea);
protected:
       // void mousePressEvent(QMouseEvent * e);
       // void mouseReleaseEvent(QMouseEvent * e);
       // void mouseDoubleClickEvent(QMouseEvent * e);
       //virtual void wheelEvent(QWheelEvent *event) override;
public slots:
       void mouseMoveEvent(QMouseEvent * event) override;
signals:
       void mouseDragged(qint64 x);
private:
       bool zoomingEnabled = false;
       bool maxVisibleArea = false;
       QRectF visibleArea;
       int xStart;
       int xEnd;
};

#endif // SLITSCANVIEW_H
