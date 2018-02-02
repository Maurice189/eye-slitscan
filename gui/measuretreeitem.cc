#include "measuretreeitem.h"

MeasureInformation MeasureTreeItem::measureInfo()
{
    return info;
}

void MeasureTreeItem::showClusteringIcon(bool show)
{
    if (show) {
        this->setIcon(0, clusteringIcon);
        this->setForeground(0, Qt::black);
    } else {
        this->setIcon(0, normalIcon);
        this->setForeground(0, Qt::black);
    }
}

QStringList MeasureTreeItem::formatProperties(const QMap<QString, QString> &map)
{
    QStringList entries;
    int maxKeyLength = 0;

    for(const auto &key : map.keys()) {
        if (key.length() > maxKeyLength) {
            maxKeyLength = key.length();
        }
    }

    for(auto key : map.keys()) {
        int zeroPaddingLength = maxKeyLength - key.length();

        QString keyText;
        if (key == "COLOR_SPACE") {
            keyText = "Color Space" + QString(zeroPaddingLength, QChar(' '));
        } else {
            keyText = formatString(key) + QString(zeroPaddingLength, QChar(' '));
        }

        QString valueText = formatString(map[key]);

        entries.append(keyText + ": " + valueText);
    }
    return entries;
}

QString MeasureTreeItem::formatString(const QString &in)
{
    if (in.isEmpty()) {
        return QString("[]");
    }
    return in[0].toUpper() + in.toLower().right(in.length()-1);
}

MeasureTreeItem::MeasureTreeItem(const MeasureInformation &info, bool spacing, bool showProperties)
{

    QFont itemFont = font(0);
    itemFont.setStyleHint(QFont::Monospace);

    normalIcon = QIcon(":/icons/method.png");
    clusteringIcon = QIcon(":/icons/clustering.png");

    this->info = info;
    this->setIcon(0, normalIcon);

    if (spacing) {
        this->setSizeHint(0, QSize(270, 30));
    }

    this->setFlags(this->flags() | Qt::ItemIsSelectable);
    this->setText(0, info.title);

    if (showProperties) {
        for(const auto& text : formatProperties(info.properties)) {
            QTreeWidgetItem *item = new QTreeWidgetItem(this);
            item->setFont(0, itemFont);
            item->setText(0, text);
        }
    }
}
