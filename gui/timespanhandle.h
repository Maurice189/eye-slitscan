#ifndef TIMESPANHANDLE_H
#define TIMESPANHANDLE_H

#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QPainter>
#include <iostream>

class TimespanHandle : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
signals:
    void handleMoved();
private:
    int maxPosX = 0;
    TimespanHandle *other;
    bool hasOtherHandle = false;
public:
    TimespanHandle(qreal pos, qreal height, QGraphicsItem *parent) : QGraphicsPolygonItem(parent)
    {
        setFlags(ItemIsMovable | ItemSendsGeometryChanges);
        QPolygonF polygon = QPolygonF({QPointF(pos-10, 10), QPointF(pos+10, 10), QPointF(pos+10, 20), QPointF(pos, height), QPointF(pos-10, 20)});
        this->setPolygon(polygon);
        this->setPos(100, 0);
    }

    void setOtherHandle(TimespanHandle *other)
    {
        this->other = other;
        hasOtherHandle = true;
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
        update( boundingRect().x(), QRectF().y(), boundingRect().width(), boundingRect().height() );
        QGraphicsItem::mouseMoveEvent( event );
    }

    /*
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
        update( boundingRect().x(), QRectF().y(), boundingRect().width(), boundingRect().height() );
        QGraphicsItem::mouseReleaseEvent( event );
    }
    */
protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        QGraphicsPolygonItem::paint(painter, option, widget);
        painter->setRenderHint(QPainter::Antialiasing, false);
    }
    QVariant itemChange(GraphicsItemChange change, const QVariant &value)
    {
        if (change == ItemPositionChange) {
            double x = value.toPointF().x();
            if (x < 0) {
                return QPointF(0, 0);
            }

            emit handleMoved();
            // value is the new position.
            QPointF newPos = value.toPointF();
            newPos.setY(QRectF().y());
            //std::cout << newPos.x() << std::endl;
            return newPos;
        }
        return QGraphicsItem::itemChange(change, value);
    }
};

#endif // TIMESPANHANDLE_H
