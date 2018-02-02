#include "frameslideritem.h"

FrameSliderItem::FrameSliderItem(int offsetX, int height) : offsetX(offsetX)
{
    QLineF line(QPointF(0, 0), QPointF(0, height));
    QPen pen(Qt::black, 3, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);

    setZValue(1000); // TODO : This is not clean
    setLine(line);
    setPen(pen);
    setX(offsetX);
}
