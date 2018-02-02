#ifndef HISTORYLISTWIDGET_H
#define HISTORYLISTWIDGET_H


#include "historylistitem.h"

#include <QObject>
#include <QListWidget>
#include <QMouseEvent>
#include <QModelIndex>

class HistoryListWidget : public QListWidget
{
    Q_OBJECT
public:
    HistoryListWidget(QWidget *parent=0);

    void addHistoryListItem(HistoryListItem *item);

signals:
    void historyListItemSelected(HistoryListItem *item);
    void deselected(HistoryListItem *item);

private:
    virtual void mousePressEvent(QMouseEvent *event)
    {
        QModelIndex index = indexAt(event->pos());
        bool selected = selectionModel()->isSelected(indexAt(event->pos()));
        QListWidget::mousePressEvent(event);

        /*
        if (index.row() == -1) {
            return;
        }*/

        HistoryListItem *item = dynamic_cast<HistoryListItem*>(this->item(index.row()));

        if (selected) {
            clearSelection();
            const QModelIndex mindex;
            selectionModel()->setCurrentIndex(mindex, QItemSelectionModel::Select);
            emit deselected(item);
        } else {
            emit historyListItemSelected(item);
        }
    }
};

#endif // HISTORYLISTWIDGET_H
