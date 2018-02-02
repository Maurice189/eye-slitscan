#ifndef TIMESPANSELECTOR_H
#define TIMESPANSELECTOR_H

#include <QObject>
#include <QMouseEvent>
#include <QLabel>
#include <QSlider>
#include <QMimeData>
#include <QDrag>
#include <QWidgetAction>
#include <QApplication>
#include <QPixmap>
#include <QCursor>
#include <QGuiApplication>
#include <QDir>
#include <QProxyStyle>
#include <QStyleOptionSlider>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsPolygonItem>

/*
*
*/

#include "timespanhandle.h"


class TimeSpanSelector: public QObject, public QGraphicsItem
{
    Q_OBJECT

signals:
    void handlesMoved(QPointF newPos);
public slots:
    void handleMoved()
    {
        if (first->pos().x() < second->pos().x()) {
            emit handlesMoved(QPointF(first->pos().x(), second->pos().x()));
        } else {
            emit handlesMoved(QPointF(second->pos().x(), first->pos().x()));
        }
    }

    void currentPositionChanged(qreal pos)
    {
        //QPolygonF polygon = QPolygonF({QPointF(pos-10, 10), QPointF(pos+10, 10), QPointF(pos+10, 20), QPointF(pos, height), QPointF(pos-10, 20)});
        //currentPosIndicator->setPolygon(polygon);
    }

public:
    /** Constructor */
    TimeSpanSelector(int width, int height);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;

    QPointF getHandlePositions()
    {
        if (first->pos().x() < second->pos().x()) {
            return QPointF(first->x(), second->x());
        }
        return QPointF(second->x(), first->x());
    }

    void setHandlesPosition(QPair<int, int> handlesPosition);

private:
    QGraphicsRectItem *box;
    QGraphicsPolygonItem *currentPosIndicator;
    TimespanHandle *first;
    TimespanHandle *second;
    int width;
    int height;
};

#endif // TIMESPANSELECTOR_H
