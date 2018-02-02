#ifndef HOVERABLELINEITEM_H
#define HOVERABLELINEITEM_H

#include <QGraphicsLineItem>
#include <QPen>

enum class VisualMode {STANDARD, SELECTED, HOVERED, UNHOVERED};

class HoverableLineItem : public QGraphicsLineItem
{
public:
    HoverableLineItem(QLineF line, QPen standardPen, QPen hoverPen, QPen selectedPen, QGraphicsItem *parent=nullptr);
    virtual void setVisualMode(VisualMode mode);
private:
    QPen standardPen;
    QPen hoverPen;
    QPen selectedPen;


    bool selected = false;
    VisualMode mode;
};

#endif // HOVERABLELINEITEM_H
