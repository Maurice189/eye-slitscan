#ifndef TIMELINEITEM_H
#define TIMELINEITEM_H

#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QLineF>
#include <QPen>


enum class TimeUnit {MILLI_SECONDS, SECONDS, MINUTES};

class TimelineItem : public QGraphicsItem
{
public:
    TimelineItem(int initialTime, int endTime, TimeUnit unit, int ticks, int widgetWidth);
    void updateScale(int initialTime, int endTime, TimeUnit unit, int ticks, int widgetWidth);
private:
    QPen pen;
};

#endif // TIMELINEITEM_H
