#include "hoverablelineitem.h"

HoverableLineItem::HoverableLineItem(QLineF line, QPen standardPen, QPen hoverPen, QPen selectedPen, QGraphicsItem *parent) : QGraphicsLineItem(line, parent)
{
    this->mode = VisualMode::STANDARD;
    this->setPen(standardPen);

    this->standardPen = standardPen;
    this->hoverPen = hoverPen;
    this->selectedPen = selectedPen;
}



void HoverableLineItem::setVisualMode(VisualMode mode)
{
    switch (mode) {
    case VisualMode::STANDARD:
        this->setPen(standardPen);
        selected = false;
        break;
    case VisualMode::SELECTED:
        this->setPen(selectedPen);
        selected = true;
        break;
    case VisualMode::HOVERED:
        this->setPen(hoverPen);
        this->mode = mode;
        break;
    case VisualMode::UNHOVERED:
        if(selected) {
            this->setPen(selectedPen);
        } else {
            this->setPen(standardPen);
        }
        break;
    default:
        break;
    }
    this->mode = mode;
    update();
}
