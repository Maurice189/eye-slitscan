#include "representationview.h"

RepresentationViewerObject::RepresentationViewerObject(QGraphicsView *view)
{
    this->view = view;
    scene = new QGraphicsScene();
    view->setScene(scene);

    slitscanItem = new QGraphicsPixmapItem();
    aoiStringItem = new QGraphicsTextItem();
    gazeX = new QChart();
    gazeY = new QChart();

    scene->addItem(slitscanItem);
}

void RepresentationViewerObject::update(const TimeSpanData &data)
{
    scene->clear();

    auto mat = data.slitscan;
    auto image = QImage((unsigned char*) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA8888).rgbSwapped();
    image = image.scaled(QSize(450, 170), Qt::IgnoreAspectRatio);

    slitscanItem = new QGraphicsPixmapItem(QPixmap::fromImage(image));

    int i = 0;

    QLineSeries *xSeries = new QLineSeries();
    QLineSeries *ySeries = new QLineSeries();

    for(auto p : data.trajectoryOfGazes.points) {
        xSeries->append(i, p.first);
        ySeries->append(i, p.second);
        i++;
    }


    aoiStringItem = new QGraphicsTextItem(QString::fromStdString(data.aoiString));
    aoiStringItem->setTextInteractionFlags(Qt::TextSelectableByMouse);
    aoiStringItem->setTextWidth(440);

    gazeX = new QChart();
    gazeY = new QChart();

    gazeX->legend()->hide();
    gazeX->addSeries(xSeries);
    gazeX->createDefaultAxes();
    gazeX->resize(450, 170);

    gazeY->legend()->hide();
    gazeY->addSeries(ySeries);
    gazeY->createDefaultAxes();
    gazeY->resize(450, 170);

    switch (currState) {
    case RadioState::SLITSCAN:
        scene->addItem(slitscanItem);
        break;
    case RadioState::GAZEX:
        scene->addItem(gazeX);
        break;
    case RadioState::GAZEY:
        scene->addItem(gazeY);
        break;
    default:
        break;
    }
}

void RepresentationViewerObject::radioSlitButtonToggled(bool toggled)
{
    if(toggled) {
        currState = RadioState::SLITSCAN;
        scene->addItem(slitscanItem);
    } else {
        scene->removeItem(slitscanItem);
    }
}

void RepresentationViewerObject::radioGazeXButtonToggled(bool toggled)
{
    if(toggled) {
        currState = RadioState::GAZEX;
        scene->addItem(gazeX);
    } else {
        scene->removeItem(gazeX);
    }
}

void RepresentationViewerObject::radioGazeYButtonToggled(bool toggled)
{
    if(toggled) {
        currState = RadioState::GAZEY;
        scene->addItem(gazeY);
    } else {
        scene->removeItem(gazeY);
    }
}

void RepresentationViewerObject::radioGazeAOIStringButtonToggled(bool toggled)
{
}
