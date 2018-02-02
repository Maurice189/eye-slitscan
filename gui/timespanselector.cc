#include "timespanselector.h"


TimeSpanSelector::TimeSpanSelector(int width, int height) : QGraphicsItem()
{
    this->width = width;
    this->height = height;

    box = new QGraphicsRectItem(0, 1, width, height, this);

    box->setBrush(QColor::fromRgb(245, 245, 245));
    box->setPen(QPen(QColor::fromRgb(245, 245, 245), 2));
    first = new TimespanHandle(0, height - 2, box);
    first->setZValue(100);
    first->setPen(QColor::fromRgb(94, 144, 250));
    first->setBrush(QColor::fromRgb(94, 144, 250));
    second = new TimespanHandle(0, height - 2, box);
    second->setZValue(100);
    second->setPen(QColor::fromRgb(94, 144, 250));
    second->setBrush(QColor::fromRgb(94, 144, 250));

    currentPosIndicator = new QGraphicsPolygonItem(box);
    currentPosIndicator->setBrush(Qt::blue);
    currentPosIndicator->setPen(QPen(Qt::blue));


    QFont font("Noto", 10);

    for(int x = 0; x < width; x += 10) {
        /*
        if (x % 110 == 0) {
            QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(box);
            text->setText(QString::number(x));
            text->setBrush(QColor::fromRgb(135, 135, 135));
            text->setFont(font);
            text->setPos(x, 2);
        }
        */
        if (x % 50 == 0) {
            QGraphicsLineItem *line = new QGraphicsLineItem(box);
            line->setLine(QLineF(x, 18, x, height-2));
            line->setPen(QPen(QColor::fromRgb(40, 40, 40), 1));

            QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(box);
            text->setText(QString::number(x));
            text->setBrush(QColor::fromRgb(40, 40, 40));
            text->setFont(font);
            text->setPos(x-10, 1);
        } else {
            QGraphicsLineItem *line = new QGraphicsLineItem(box);
            line->setLine(QLineF(x, 25, x, height-2));
            line->setPen(QPen(QColor::fromRgb(40, 40, 40), 1));
        }
    }

    QObject::connect(first, &TimespanHandle::handleMoved, this, &TimeSpanSelector::handleMoved);
    QObject::connect(second, &TimespanHandle::handleMoved, this, &TimeSpanSelector::handleMoved);

    first->setOtherHandle(second);
}


void TimeSpanSelector::setHandlesPosition(QPair<int, int> handlesPosition)
{
    first->setX(handlesPosition.first);
    second->setX(handlesPosition.second);
}

void TimeSpanSelector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    for(auto &child : childItems()) {
        child->paint(painter, option, widget);
    }
}

QRectF TimeSpanSelector::boundingRect() const
{
    return childrenBoundingRect();
}
