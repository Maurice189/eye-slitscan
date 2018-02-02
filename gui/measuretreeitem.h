#ifndef MEASURETREEITEM_H
#define MEASURETREEITEM_H

#include <QTreeWidgetItem>
#include <QIcon>
#include <QPair>
#include <QList>
#include <QString>
#include <QPixmap>
#include <QFont>
#include <QStringList>

#include "constants.h"


class MeasureTreeItem : public QTreeWidgetItem
{
public:
    MeasureTreeItem(const MeasureInformation &info, bool spacing=false, bool showProperties=true);
    MeasureInformation measureInfo();
    void showClusteringIcon(bool show);
private:
    QStringList formatProperties(const QMap<QString, QString> &map);
    QString formatString(const QString &in);
private:
    MeasureInformation info;
    QIcon normalIcon;
    QIcon clusteringIcon;
};

#endif // MEASURETREEITEM_H
