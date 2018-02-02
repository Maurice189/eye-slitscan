#ifndef REPRESENTATIONVIEW_H
#define REPRESENTATIONVIEW_H

#include "constants.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

//QT_CHARTS_USE_NAMESPACE

using namespace QtCharts;

enum class RadioState {SLITSCAN, GAZEX, GAZEY};

class RepresentationViewerObject : public QObject
{
    Q_OBJECT
public:
    RepresentationViewerObject(QGraphicsView *view);
    void update(const TimeSpanData &data);

private:
    QGraphicsView* view;
    QGraphicsScene* scene;
    QGraphicsPixmapItem* slitscanItem;
    QGraphicsTextItem* aoiStringItem;
    QChart* gazeX;
    QChart* gazeY;

    RadioState currState = RadioState::SLITSCAN;

public slots:
    void radioSlitButtonToggled(bool toggled);
    void radioGazeXButtonToggled(bool toggled);
    void radioGazeYButtonToggled(bool toggled);
    void radioGazeAOIStringButtonToggled(bool toggled);
};

#endif // REPRESENTATIONVIEW_H
