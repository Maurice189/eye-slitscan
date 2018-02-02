#include "interactivelineitem.h"

InteractiveLineItem::InteractiveLineItem(const clu::node *subtree, QLineF line, QPen standardPen, QPen hoverPen, QPen selectedPen, QGraphicsItem *parent) : HoverableLineItem(line, standardPen, hoverPen, selectedPen, parent)
{
    calcLeafIds(subtree, &leafIds);
}

void InteractiveLineItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HoverableLineItem::mousePressEvent(event);
    emit activatedLeafs(this);
}


void InteractiveLineItem::setVisualMode(VisualMode mode)
{
    HoverableLineItem::setVisualMode(mode);

    for(HoverableLineItem *child : children) {
        child->setVisualMode(mode);
    }
}

void InteractiveLineItem::calcLeafIds(const node *p, QStringList *leafIds)
{
    if(p) {
        leafIds->append(QString::fromStdString(p->key));
        calcLeafIds(p->left, leafIds);
        calcLeafIds(p->right, leafIds);
    }
}

void InteractiveLineItem::addChild(HoverableLineItem *child)
{
    if (child) {
        children.append(child);
    }
}

void InteractiveLineItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsLineItem::hoverEnterEvent(event);
    setVisualMode(VisualMode::HOVERED);
}

void InteractiveLineItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsLineItem::hoverLeaveEvent(event);
    setVisualMode(VisualMode::UNHOVERED);
}
