#ifndef VIDEOOVERLAYWIDGET_H
#define VIDEOOVERLAYWIDGET_H

#include <QWidget>
#include <QList>
#include <QPair>

class VideoOverlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoOverlayWidget(QWidget *parent = 0);
    void setGazePoints(QList<QPair<int, int>> points);
protected:
    virtual void paintEvent(QPaintEvent *event) override;
private:
    QList<QLineF> vLines;
    QList<QPair<int, int>> points;

signals:

public slots:
};

#endif // VIDEOOVERLAYWIDGET_H
