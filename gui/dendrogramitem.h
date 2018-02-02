#ifndef DENDROGRAMITEM_H
#define DENDROGRAMITEM_H


#include <string>
#include <QObject>
#include <QBrush>
#include <QPainter>
#include <QPen>
#include <QGraphicsItem>
#include <QGraphicsLineItem>

#include <stdexcept>
#include <cmath>
#include <functional>

#include "interactivelineitem.h"
#include "hoverablelineitem.h"
#include "../core/clustering/include/hac.h"

struct TreeInfo {
    std::pair<double, double> pos;
    InteractiveLineItem *item;
};

class DendrogramItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
signals:
    void externSubtreeActivated(QStringList leafIds);
public:
    DendrogramItem(const ClusteringResult &result, int width, int verticalMargin, const QColor &color=QColor::fromRgb(180, 180, 180), bool tooltip=true, bool showLabels=true, int fontSize=9);
    DendrogramItem(int width, int verticalMargin, const QColor &color=QColor::fromRgb(180,180,180), bool tooltip=true, bool showLabels=true, int fontSize=9, int penWidth=3);

    void setDefaultState(void);
    void setInterativeModeEnabled(bool enabled);
    void setVerticalMargin(int verticalMargin);
    void build(const ClusteringResult &result);

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
           QWidget *widget);
public slots:
    void internSubtreeActivated(InteractiveLineItem *item);
private:


    TreeInfo postorderBuildStep(const clu::node *p);
    double getTreeWidth(const node *tree, double width=0.);

    std::function<int(double)> xtrans;
    std::function<int(double)> ytrans;

    std::vector<int> leafsOrder;
    bool showLabels;


    TreeInfo rootInfo;

    int width;
    int height;
    int leafPosY = 0;
    int leafIndex = 0;
    int verticalMargin = 10;
    int fontSize;
    double max;

    QPen pen1;
    QPen pen2;
    QPen pen3;
};

#endif // DENDROGRAMITEM_H
