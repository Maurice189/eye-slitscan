#include "dendrogramitem.h"

#include <clustering/include/hac.h>
#include <algorithm>
#include <QMap>
#include <QGraphicsSimpleTextItem>

DendrogramItem::DendrogramItem(const ClusteringResult &result, int width, int verticalMargin, const QColor &color, bool tooltip, bool showLabels, int fontSize) : DendrogramItem(width, verticalMargin, color, tooltip, showLabels, fontSize)
{
    build(result);
}

DendrogramItem::DendrogramItem(int width, int verticalMargin, const QColor &color, bool tooltip, bool showLabels, int fontSize, int penWidth) : QGraphicsItem()
{
    this->showLabels = showLabels;
    this->fontSize = fontSize;

    pen1.setStyle(Qt::SolidLine);
    pen1.setWidth(penWidth);
    pen1.setBrush(color);

    pen2.setStyle(Qt::SolidLine);
    pen2.setWidth(penWidth);
    pen2.setBrush(Qt::green);

    pen3.setStyle(Qt::SolidLine);
    pen3.setWidth(penWidth);
    pen3.setBrush(QColor::fromRgb(220, 220, 220));

    if (tooltip) {
        setAcceptHoverEvents(true);
    } else {
        setAcceptHoverEvents(false);
    }

    if (width < 10) {
        throw std::invalid_argument("dendrogramitem ::: width must not be less than 10px");
    }

    this->verticalMargin = verticalMargin;
    this->width = width;
}

void DendrogramItem::build(const ClusteringResult &result)
{
    this->height = verticalMargin * (result.leafsOrder.size()-1);
    this->leafsOrder = result.leafsOrder;

    auto minmax = std::minmax_element(result.distances.begin(), result.distances.end());

    int indexMin = std::distance(result.distances.begin(), minmax.first);
    int indexMax = std::distance(result.distances.begin(), minmax.second);

    double distanceDiff = result.distances[indexMin];

    if (indexMax != indexMin) {
        distanceDiff = std::fabs(result.distances[indexMin] - result.distances[indexMax]);
    }

    int rightSpaceX = 40;
    int leftSpaceX = 10;

    if (!showLabels) {
        rightSpaceX = 0;
    }

    double m = (leftSpaceX + rightSpaceX + 10 - width) / (result.distances[indexMax] - result.distances[indexMin]);
    double b = width - (leftSpaceX + rightSpaceX) - (m * result.distances[indexMin]);

    max = (width - rightSpaceX - b) / m;

    xtrans = [m, b](double x) {
        return (x * m) + b;
    };

    ytrans = [](double y) {
        return y;
    };

    leafPosY = 0;
    leafIndex = 0;

    for(auto *child : childItems()) {
        child->setParentItem(nullptr);
        delete child;
    }

    if (std::fabs(result.sumOfDistances - 0.0) < 1e-9) {
        throw std::invalid_argument("dendrogramitem ::: sum of distances in dendrogram tree must not be zero");
    }

    rootInfo = postorderBuildStep(result.tree);
}


void DendrogramItem::setVerticalMargin(int verticalMargin)
{
    this->verticalMargin = verticalMargin;
}

TreeInfo DendrogramItem::postorderBuildStep(const clu::node *p)
{
    if(p) {
        double leftPosX;
        double rightPosX;

        double leftPosY;
        double rightPosY;

        bool isLeaf = true;

        TreeInfo leftTreeInfo;
        TreeInfo rightTreeInfo;

        if(p->left) {
            leftTreeInfo = postorderBuildStep(p->left);
            leftPosX = leftTreeInfo.pos.first;
            leftPosY = leftTreeInfo.pos.second;
            isLeaf = false;
        }
        if(p->right) {
            rightTreeInfo = postorderBuildStep(p->right);
            rightPosX = rightTreeInfo.pos.first;
            rightPosY = rightTreeInfo.pos.second;
            isLeaf = false;
        }
        if (isLeaf) {
            int tmpLeafPosY = leafPosY;
            leafPosY += verticalMargin;
            return TreeInfo{std::pair<double, double>(max, tmpLeafPosY), nullptr};
        }

        assert(rightPosY > leftPosY);

        double centerPosX = p->distance;
        double centerPosY = (rightPosY + leftPosY) / 2;

        QLineF leftLine(QPointF(xtrans(leftPosX), ytrans(leftPosY)), QPointF(xtrans(centerPosX), ytrans(leftPosY)));
        QLineF rightLine(QPointF(xtrans(rightPosX), ytrans(rightPosY)), QPointF(xtrans(centerPosX), ytrans(rightPosY)));
        QLineF centerLine(QPointF(xtrans(centerPosX), ytrans(leftPosY)), QPoint(xtrans(centerPosX), ytrans(rightPosY)));

        auto *leftItem = new HoverableLineItem(leftLine, pen3, pen2, pen1, this);
        auto *rightItem = new HoverableLineItem(rightLine, pen3, pen2, pen1, this);
        InteractiveLineItem *centerItem = new InteractiveLineItem(p, centerLine, pen3, pen2, pen1, this);

        centerItem->addChild(leftItem);
        centerItem->addChild(rightItem);
        centerItem->addChild(leftTreeInfo.item);
        centerItem->addChild(rightTreeInfo.item);

        QObject::connect(centerItem, &InteractiveLineItem::activatedLeafs, this, &DendrogramItem::internSubtreeActivated);

        if (leftPosX == max && showLabels) {
            QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(leftItem);
            text->setText(QString::fromStdString(p->left->key));
            text->setFont(QFont("Noto", fontSize));
            text->setBrush(QColor::fromRgb(50, 50, 50));
            text->setPos(width-32, leftPosY-14);
            leafIndex++;
        }
        if (rightPosX == max && showLabels) {
            QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(rightItem);
            text->setText(QString::fromStdString(p->right->key));
            text->setFont(QFont("Noto", fontSize));
            text->setBrush(QColor::fromRgb(50, 50, 50));
            text->setPos(width-32, rightPosY-14);
            leafIndex++;
        }

        centerItem->setToolTip(QString::fromStdString(p->key));
        centerItem->setAcceptHoverEvents(true);

        return TreeInfo{std::pair<double, double>(centerPosX, centerPosY), centerItem};
    }
}


void DendrogramItem::setDefaultState()
{
    rootInfo.item->setVisualMode(VisualMode::STANDARD);
    rootInfo.item->setVisualMode(VisualMode::SELECTED);
}

void DendrogramItem::setInterativeModeEnabled(bool enabled)
{
    this->setAcceptHoverEvents(enabled);
    rootInfo.item->setVisualMode(VisualMode::STANDARD);

    if (!enabled) {
        rootInfo.item->setVisualMode(VisualMode::SELECTED);
    }
}

QRectF DendrogramItem::boundingRect() const
{
    return childrenBoundingRect();
}

void DendrogramItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
   QWidget *widget)
{
    QBrush brush(Qt::blue);
    painter->setBrush(brush);

    for(auto &child : childItems()) {
        child->paint(painter, option, widget);
    }
}

void DendrogramItem::internSubtreeActivated(InteractiveLineItem *item)
{
    rootInfo.item->setVisualMode(VisualMode::STANDARD);
    item->setVisualMode(VisualMode::SELECTED);

    emit externSubtreeActivated(item->getLeafIds());
}
