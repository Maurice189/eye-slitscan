#ifndef SLITSCANITEM_H
#define SLITSCANITEM_H

#include <iostream>
#include <QPixmap>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QGraphicsLayoutItem>
#include <QObject>
#include <cmath>

class SlitscanItem : public QObject, public QGraphicsPixmapItem, public QGraphicsLayoutItem
{
    Q_OBJECT
public:
    SlitscanItem(QString label)
    {
        this->label = label;
    }

    virtual QSizeF sizeHint(Qt::SizeHint which,
                            const QSizeF& constraint = QSizeF()) const {
        Q_UNUSED(which);
        Q_UNUSED(constraint);
        return boundingRect().size();
    }

    virtual void setGeometry(const QRectF& rect) {
        setPos(rect.topLeft());
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        painter->setPen(QColor::fromRgb(245, 245, 245));
        painter->setBrush(QBrush(QColor::fromRgb(245, 245, 245)));
        painter->drawRect(0, 0, pixmap().width(), pixmap().height());


        int leftX = visibleArea.x();
        int rightX = visibleArea.width() + leftX;

        if (!maxVisibleArea) {
            painter->setBrush(QColor::fromRgb(220, 220, 220, 255));
            painter->setPen(QColor::fromRgb(220, 220, 220, 255));

            QRectF left(0, 0, leftX, pixmap().height());
            QRectF right(rightX, 0, pixmap().width() - rightX, pixmap().height());

            painter->drawRect(left);
            painter->drawRect(right);
        }
        QGraphicsPixmapItem::paint(painter,option,widget);

        int h = pixmap().height();

        QFont font("Noto", 13);

        static const QPointF points[3] = {
                QPointF(0 + labelPosX, h-25),
                QPointF(10 + labelPosX, h-15),
                QPointF(0 + labelPosX, h-5)
        };

        painter->setOpacity(1);
        painter->setBrush(QColor::fromRgb(245, 245, 245));
        painter->setPen(QColor::fromRgb(0, 0, 0, 0));
        painter->drawPolygon(points, 3);

        painter->setPen(QColor::fromRgb(20, 20, 20));
        painter->setFont(font);
        painter->drawText(labelPosX + 15, h - 10, label);
    }

    void setVisibleArea(QRectF visibleArea)
    {
        this->visibleArea = visibleArea;
    }

    void setVisibleAreaToMaximum(bool maxVisibleArea)
    {
        this->maxVisibleArea = maxVisibleArea;
    }

    virtual void setPixmap(const QPixmap &pixmap)
    {
        QGraphicsPixmapItem::setPixmap(pixmap);
        this->original = pixmap;
        visibleArea = pixmap.rect();
    }

    void setHeightOfSlitscan(int height)
    {
        QGraphicsPixmapItem::setPixmap(this->original.scaled(this->original.width(), height, Qt::IgnoreAspectRatio));
    }

    void scaleHorizontalByFactor(double factor)
    {
        int newWidth = int(this->original.width() * factor);
        QGraphicsPixmapItem::setPixmap(this->original.scaled(newWidth, this->original.height()));
    }


public slots:
    void horizontalScalingChanged(int newValue)
    {
        scaleHorizontalByFactor(newValue / 100.);
    }

    void scrollbarValueChanged(int value)
    {
        labelPosX = value;
        update();
    }

protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {

        double snapHeight = pixmap().height() + 10.;
        double xx=x();
        double yy=y();
        if((int)yy / (int)snapHeight != yy/snapHeight) yy=snapHeight*std::round(yy/snapHeight);
        setPos(xx,yy);
        QGraphicsItem::mouseReleaseEvent(event);
    }
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value)
    {
        if (change == ItemPositionChange)
            return QPointF(pos().x(), value.toPointF().y());
        return QGraphicsItem::itemChange( change, value );
    }
private:
    bool maxVisibleArea = false;
    QRectF visibleArea;
    QPixmap original;
    QString label;

    int labelPosX = 0;
};

#endif // SLITSCANITEM_H
