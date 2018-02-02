#ifndef SLTISCANVIDEO_H
#define SLTISCANVIDEO_H

#include "constants.h"

#include <QObject>
#include <QList>
#include <QLineF>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtMultimediaWidgets/QGraphicsVideoItem>


struct DataPoint {
    int posX;
    int posY;
    int radius;

    QString label;
    QColor color;
};

class SlitscanVideo : public QGraphicsVideoItem
{
    Q_OBJECT
public:
    SlitscanVideo(int width, int height, QGraphicsItem *parent=0);
    void setSlits(QList<int> slitPosX);
    void setShowLabelsEnabled(bool labelsEnabled);
    void drawGazeData(QList<ParticipantData> participants, int pos, int drawMode);
    void setSourceInfo(const VideoInfo &srcInfo);
    void redraw(void);
protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
private:
    QFont font;

    QList<DataPoint> data;

    QList<QLineF> vLines;
    QList<QColor> colors;
    QList<QPair<int, int>> points;
    QList<QString> labels;

    bool labelsEnabled = true;

    double srcWidth;
    double srcHeight;

    int width;
    int height;

};

#endif // SLTISCANVIDEO_H
