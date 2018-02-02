#ifndef FRAMESLIDERITEM_H
#define FRAMESLIDERITEM_H

#include <QGraphicsLineItem>
#include <QObject>
#include <QPen>
#include <iostream>

class FrameSliderItem : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    FrameSliderItem(int offsetX, int height);
    void setMediaLengthMs(int mediaLengthMs)
    {
        m_mediaLengthMs = mediaLengthMs;
    }

    void setMaximumY(int maximumY)
    {
        m_maximumY = maximumY;
    }
    void setHeight(int height)
    {
        QLineF line(QPointF(0, 0), QPointF(0, height));
        setLine(line);
    }

public slots:
    void setSliderPos(qint64 posX)
    {
        setPos(posX, y());
    }
    void setSliderPosByDuration(qint64 posX)
    {
        setPos(m_maximumY * (double(posX) / m_mediaLengthMs), y());
    }

    void setHorizontalBoundaries(int fromX, int toX)
    {
        this->fromBoundaryX = fromX - offsetX;
        this->toBoundaryX = toX - offsetX;
    }

signals:
    void sliderMoved(qint64 pos);
    void sliderPosMoved(qint64 pos);
protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant  &value )
    {
      if (change == ItemPositionChange){

        double x = value.toPointF().x() - offsetX;

        if (x < fromBoundaryX) {
            return QPointF(offsetX + fromBoundaryX, 0);
        }
        if (x > toBoundaryX) {
            return QPointF(offsetX + toBoundaryX, 0);
        }
        double currMs = ((value.toPointF().x()-offsetX) / double(m_maximumY)) * m_mediaLengthMs;
        emit sliderMoved((qint64) currMs);
        emit sliderPosMoved(value.toPointF().x());
      }
      return QGraphicsItem::itemChange(change, value);
    }
private:
    int m_maximumY = 1;
    int m_mediaLengthMs = 1;
    int offsetX = 0;
    int fromBoundaryX;
    int toBoundaryX;
};

#endif // FRAMESLIDERITEM_H
