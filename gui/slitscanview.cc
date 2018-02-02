#include "slitscanview.h"
#include <QPointF>
#include <iostream>

SlitscanView::SlitscanView(QWidget *parent) :
    QGraphicsView(parent)
{

}


void SlitscanView::setZoomingEnabled(bool zoomingEnabled)
{
    this->zoomingEnabled = zoomingEnabled;
}

void SlitscanView::setVisibleArea(int xStart, int xEnd)
{
    this->xStart = xStart;
    this->xEnd = xEnd;
}

void SlitscanView::setVisibleAreaToMaximum(bool maxVisibleArea)
{
    this->maxVisibleArea = maxVisibleArea;
}

/*
void SlitscanView::wheelEvent(QWheelEvent *event)
    {
        if (zoomingEnabled && (event->modifiers() & Qt::ControlModifier)) {
            // zoom
            const ViewportAnchor anchor = transformationAnchor();
            setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
            int angle = event->angleDelta().y();
            qreal factor;
            if (angle > 0) {
                factor = 1.1;
            } else {
                factor = 0.9;
            }
            scale(factor, factor);
            setTransformationAnchor(anchor);
        } else {
            QGraphicsView::wheelEvent(event);
        }
    }
*/
void SlitscanView::mouseMoveEvent(QMouseEvent * event)
{
    if(event->buttons() & Qt::LeftButton) {
        QPointF pt = mapToScene(event->pos());
        emit mouseDragged(pt.x());
    }
}
