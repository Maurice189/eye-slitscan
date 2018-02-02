#include "historylistwidget.h"

#include "../core/clustering/copheneticcorrelation.h"

HistoryListWidget::HistoryListWidget(QWidget *parent) : QListWidget(parent)
{

}

void HistoryListWidget::addHistoryListItem(HistoryListItem *item)
{
    this->addItem(item);
}
