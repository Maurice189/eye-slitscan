#include "videooverlaywidget.h"

#include <QPainter>

VideoOverlayWidget::VideoOverlayWidget(QWidget *parent) : QWidget(parent)
{

    this->setPalette(Qt::transparent);
}

void VideoOverlayWidget::setGazePoints(QList<QPair<int, int>> points)
{
    this->points.clear();
    for(auto pos : points) {
        this->points.append(QPair<int, int>(pos.first * (width() / 1920.), pos.second * (height() / 1200.)));
    }
    this->update();
}

#include <iostream>


void VideoOverlayWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    QColor backgroundColor = palette().light().color();
    backgroundColor.setAlpha(50);
    painter.begin(this);
    painter.fillRect(rect(),backgroundColor);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    for (const auto &pos : points) {
        painter.drawEllipse(pos.first, pos.second, 5, 5);
        std::cout << "DRAW ELLIPSE : " << pos.first << " " << pos.second << std::endl;
    }
    std::cout << ":::::::::::: PAINT EVENT ::::::::::::::::" << std::endl;
    QWidget::paintEvent(event);
    painter.end();
}
