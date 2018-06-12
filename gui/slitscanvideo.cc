#include "slitscanvideo.h"

#include <QPainter>
#include <QGraphicsRectItem>
#include <QLinearGradient>
#include <QGraphicsDropShadowEffect>
#include <iostream>

SlitscanVideo::SlitscanVideo(int width, int height, QGraphicsItem *parent) : QGraphicsVideoItem(parent)
{

    this->setSize(QSizeF(width, height));
    this->width = width;
    this->height = height;

    this->srcWidth = 0;
    this->srcHeight = 0;

    this->font = QFont("Noto");
    this->font.setPixelSize(12);
    this->font.setBold(true);

    int halfWidth = width / 2;

    auto leftVideoHint = new QGraphicsRectItem(0, height, halfWidth, 6, this);
    auto rightVideoHint = new QGraphicsRectItem(halfWidth, height, halfWidth, 6, this);

    leftVideoHint->setPen(Qt::NoPen);
    rightVideoHint->setPen(Qt::NoPen);

    auto leftGradient = QLinearGradient(0, 5, halfWidth, 3);
    auto rightGradient = QLinearGradient(halfWidth, 5, width, 3);

    leftGradient.setColorAt(0, QColor::fromRgb(150, 0, 0));
    leftGradient.setColorAt(1, QColor::fromRgb(255, 255, 255));

    rightGradient.setColorAt(0, QColor::fromRgb(255, 255, 255));
    rightGradient.setColorAt(1, QColor::fromRgb(0, 0, 150));

    leftVideoHint->setBrush(leftGradient);
    rightVideoHint->setBrush(rightGradient);

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();

    effect->setColor(QColor::fromRgb(10, 10, 10));
    effect->setOffset(0);
    effect->setBlurRadius(50);

    //this->setGraphicsEffect(effect);

}

void SlitscanVideo::setSlits(QList<int> slitPosX)
{
    vLines.clear();
    for(auto posX : slitPosX) {
        QLineF line(QPointF(posX, 0), QPointF(posX, height));
        vLines.append(line);
    }
    redraw();
}


void SlitscanVideo::redraw()
{
    this->update(this->boundingRect());
}

void SlitscanVideo::setSourceInfo(const VideoInfo &srcInfo)
{
    this->srcWidth = srcInfo.frameWidth;
    this->srcHeight = srcInfo.frameHeight;
}

void SlitscanVideo::setShowLabelsEnabled(bool labelsEnabled)
{
    this->labelsEnabled = labelsEnabled;
    redraw();
}


void SlitscanVideo::drawGazeData(std::vector<Participant> participants, int pos, int drawMode)
{
    this->data.clear();

    for(int i = 0; i < participants.size(); i++) {
        const auto &p = participants[i];

        if (p.active) {
           DataPoint d;

           if (drawMode == 0) {
               d.posX = p.exportData[pos].mappedGazeDataPointX * (width / srcWidth);
               d.posY = p.exportData[pos].mappedGazeDataPointY * (height / srcHeight);
               d.radius = 8;
           } else if (drawMode == 1) {
               d.posX = p.exportData[pos].mappedFixationPointX * (width / srcWidth);
               d.posY = p.exportData[pos].mappedFixationPointY * (height / srcHeight);
               d.radius = ((p.exportData[pos].timestamp - p.exportData[pos].fixationTimestamp) / double(p.exportData[pos].fixationDuration)) * p.exportData[pos].fixationDuration * 0.02;
               d.radius += 8;
           }

           d.color = p.color;
           d.color.setAlpha(210);
           d.label = p.id;

           data.append(d);
        }
    }
    redraw();
}

void SlitscanVideo::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsVideoItem::paint(painter, option, widget);
    painter->setFont(font);

    for (const auto &p : data) {
        painter->setBrush(p.color);
        painter->setPen(QPen(QColor::fromRgb(0, 0, 0, 0), 0));
        painter->drawEllipse(p.posX-p.radius, p.posY-p.radius, p.radius, p.radius);

        if (labelsEnabled) {
            painter->setPen(QPen(QColor::fromRgb(244, 152, 66, 180), 1));
            painter->drawText(p.posX-p.radius-15, p.posY-p.radius-15, p.label);
        }
    }
}
