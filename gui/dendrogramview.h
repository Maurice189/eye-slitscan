#ifndef DENDROGRAMVIEW_H
#define DENDROGRAMVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>

class DendrogramView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DendrogramView(QWidget *parent = 0);
protected:
    virtual void wheelEvent (QWheelEvent * event)
    {
    }
};

#endif // DENDROGRAMVIEW_H
