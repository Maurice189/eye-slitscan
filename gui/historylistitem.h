#ifndef HISTORYLISTITEM_H
#define HISTORYLISTITEM_H

#include <QListWidgetItem>
#include <QPainter>
#include <QObject>
#include <QPixmap>
#include <QList>
#include <QLineF>
#include <QMap>

#include <stdexcept>
#include <cmath>
#include <functional>
#include <algorithm>
#include <cstdlib>

#include "../core/clustering/include/hac.h"
#include "constants.h"



class HistoryListItem : public QListWidgetItem
{
public:
    HistoryListItem(ClusteringStateData state, int width, int height) : QListWidgetItem()
    {
        this->state = state;
        this->width = width;
        this->height = height;
        this->horizontalMargin = 25;
        this->horizontalPadding = (width-2*horizontalMargin) / (state.result.leafsOrder.size()-1);

        this->setSizeHint(QSize(width, height+50));
        this->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
        this->setForeground(QBrush(QColor::fromRgb(46, 153, 131)));


        //QFont font = font();
        //font.setPixelSize(13);

        //this->setFont(QFont("Noto", 11));

        this->setText(state.info.info.title + "\n" + QString::number(state.info.from) + " - " + QString::number(state.info.to));
    }

    ClusteringStateData& getClusteringState()
    {
        return state;
    }

    void setActivated(bool active)
    {
        this->active = active;

    }

    void prepareStepsDendrogramDrawing(const ClusteringResult &result)
    {
        auto minmax = std::minmax_element(result.distances.begin(), result.distances.end());

        int indexMin = std::distance(result.distances.begin(), minmax.first);
        int indexMax = std::distance(result.distances.begin(), minmax.second);

        double distanceDiff = result.distances[indexMin];

        if (indexMax != indexMin) {
            distanceDiff = std::fabs(result.distances[indexMin] - result.distances[indexMax]);
        }

        double m = (30 + 15 - height) / (result.distances[indexMax] - result.distances[indexMin]);
        double b = height - 30 - (m * result.distances[indexMin]);

        this->max = (height - 15 - b) / m;

        xtrans = [](double x) {
            return x;
        };

        if (std::fabs(result.sumOfDistances - 0.0) < 1e-9) {
            throw std::invalid_argument("dendrogramitem ::: sum of distances in dendrogram tree must not be zero");
        }

        this->leafPosX = horizontalMargin;

        ytrans = [m, b](double y) {
            return (y * m) + b;
        };
    }

    void draw(const ClusteringResult &result)
    {

        QPixmap img01(width, height);
        QPixmap img02(width, height);

        img01.fill(Qt::transparent);
        img02.fill(Qt::transparent);

        QPainter *painter01 = new QPainter(&img01);
        QPainter *painter02 = new QPainter(&img02);

        painter01->setPen(QPen(Qt::black, 2));
        painter02->setPen(QPen(Qt::black, 2));

        int size = std::min((width / (state.result.leafs.size() * 4.)), 10.);


        painter01->setFont(QFont("Noto", size));
        painter02->setFont(QFont("Noto", size));

        prepareStepsDendrogramDrawing(result);
        postorderBuildStep(result.tree, painter01);

        prepareStepsDendrogramDrawing(result);
        postorderBuildStep(result.tree, painter02);

        //painter02->setRenderHint(QPainter::Antialiasing);
        painter02->setBrush(Qt::blue);
        painter02->drawEllipse(3, 3, 7, 7);

        delete painter01;
        delete painter02;

        dendrogramIconDeselected = img01;
        dendrogramIconSelected = img02;

        if (active)
            this->setIcon(QIcon(dendrogramIconSelected));
        else
            this->setIcon(QIcon(dendrogramIconDeselected));
    }

    void setHideHint(bool hide)
    {
        if(rangeActive && typeActive && !hide) {
            this->setHidden(false);
        } else {
            this->setHidden(true);
        }
    }

    void enableHighlight(double factor)
    {
        assert(factor >= -0.009 && factor <= 1.009);

        QFont itemFont = font();

        QPixmap img(width, height);
        img.fill(Qt::transparent);

        QPainter *painter = new QPainter(&img);
        painter->setOpacity(0.2);

        painter->drawPixmap(0, 0, dendrogramIconDeselected);

        painter->setOpacity(1.0);
        painter->setPen(QPen(QColor::fromRgb(94, 144, 250), 4));

        itemFont.setPixelSize(20);
        painter->setFont(itemFont);

        auto text = QString::number(factor).left(4);

        if (text.length() == 1) {
            text += ".";
        }
        while (text.length() < 4) {
            text += "0";
        }

        painter->drawText((width / 2) - 25, (height / 2) + 10, text);

        delete painter;
        highlightIcon = img;

        this->setIcon(QIcon(highlightIcon));
    }

    void redraw()
    {
        if (active)
            this->setIcon(QIcon(dendrogramIconSelected));
        else
            this->setIcon(QIcon(dendrogramIconDeselected));

    }

    void disableHighlight()
    {
        this->setBackground(QColor::fromRgb(0, 0, 0, 0));

        if (active)
            this->setIcon(QIcon(dendrogramIconSelected));
        else
            this->setIcon(QIcon(dendrogramIconDeselected));
    }

    void filterByRange(int start, int end)
    {
        rangeActive = state.info.from >= start && state.info.to <= end;
        filter();
    }

    void filterByMeasureInfo(MeasureInformation activeMeasureInfo)
    {
        if (state.info.info.title == activeMeasureInfo.title) {
            typeActive = !typeActive;
        }
        filter();
    }

private:
    void filter()
    {
        if(rangeActive && typeActive) {
            this->setHidden(false);
        } else {
            this->setHidden(true);
        }
    }

    std::pair<double, double> postorderBuildStep(const clu::node *p, QPainter *painter)
    {
        if(p) {
            double leftPosX;
            double rightPosX;

            double leftPosY;
            double rightPosY;


            bool isLeaf = true;

            if(p->left) {
                auto pos = postorderBuildStep(p->left, painter);
                leftPosX = pos.first;
                leftPosY = pos.second;
                isLeaf = false;
            }
            if(p->right) {
                auto pos = postorderBuildStep(p->right, painter);
                rightPosX = pos.first;
                rightPosY = pos.second;
                isLeaf = false;
            }
            if (isLeaf) {
                int tmpLeafPosX = leafPosX;
                leafPosX += horizontalPadding;
                return std::pair<double, double>(tmpLeafPosX, max);
            }

            assert(rightPosX > leftPosX);

            double centerPosX = (rightPosX + leftPosX) / 2;
            double centerPosY = p->distance;

            QLineF leftLine(QPointF(xtrans(leftPosX), ytrans(leftPosY)), QPointF(xtrans(leftPosX), ytrans(centerPosY)));
            QLineF rightLine(QPointF(xtrans(rightPosX), ytrans(rightPosY)), QPointF(xtrans(rightPosX), ytrans(centerPosY)));
            QLineF centerLine(QPointF(xtrans(leftPosX), ytrans(centerPosY)), QPoint(xtrans(rightPosX), ytrans(centerPosY)));

            if (leftPosY == max && showLabels) {
                painter->drawText(leftPosX-7, height, QString::fromStdString(p->left->key));
            }
            if (rightPosY == max && showLabels) {
                painter->drawText(rightPosX-7, height, QString::fromStdString(p->right->key));
            }

            painter->drawLine(leftLine);
            painter->drawLine(rightLine);
            painter->drawLine(centerLine);

            return std::pair<double, double>(centerPosX, centerPosY);
        }
    }

private:
    std::function<int(double)> xtrans;
    std::function<int(double)> ytrans;

    ClusteringStateData state;

    QPixmap dendrogramIconSelected;
    QPixmap dendrogramIconDeselected;
    QPixmap highlightIcon;

    int height;
    int width;

    int leafPosX = 0;
    int horizontalPadding;
    int horizontalMargin;

    double max;

    bool typeActive = true;
    bool rangeActive = true;
    bool showLabels = true;
    bool active = false;

};

#endif // HISTORYLISTITEM_H
