#ifndef INTERACTIVELINEITEM_H
#define INTERACTIVELINEITEM_H

#include <QObject>
#include <QGraphicsLineItem>
#include <QStringList>
#include <QLineF>
#include <QPen>

#include "../core/clustering/include/hac.h"
#include "hoverablelineitem.h"

class InteractiveLineItem : public QObject, public HoverableLineItem
{
    Q_OBJECT
signals:
    void activatedLeafs(InteractiveLineItem *item);
public:
    InteractiveLineItem(const clu::node *subtree, QLineF line, QPen standardPen, QPen hoverPen, QPen selectedPen, QGraphicsItem *parent=nullptr);
    void addChild(HoverableLineItem *child);
    QStringList getLeafIds() { return leafIds;}
   virtual void setVisualMode(VisualMode mode) override;
protected:
   virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
   virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;
   virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) override;
private:
    void calcLeafIds(const clu::node *p, QStringList *leafIds);
private:
    clu::node *subtree;
    QList<HoverableLineItem*> children;
    QStringList leafIds;

};

#endif // INTERACTIVELINEITEM_H
