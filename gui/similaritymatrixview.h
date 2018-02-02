#ifndef SIMILARITYMATRIXVIEW_H
#define SIMILARITYMATRIXVIEW_H

#include <QObject>
#include <QColor>
#include <QList>
#include <QString>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QGraphicsDropShadowEffect>
#include <vector>
#include <functional>

#include "constants.h"
#include "dendrogramitem.h"


struct ColorScheme3 {
    QString title;

    QColor class01;
    QColor class02;
    QColor class03;
};

class SimilarityMatrixView : public QGraphicsView
{
    Q_OBJECT
public:
    SimilarityMatrixView(QWidget * parent = 0);
    void build(const QList<SimilarityMatrix> &matrices, const ClusteringResult &result);
    void build(SimilarityMatrix &matrix, const ClusteringResult &result);

    QPixmap getImageOfColorSchemeSample(int width, int height, int sampleSize);

private:
    void buildCmpBlock(const QList<SimilarityMatrix> &matrices, int i, int j, const QRectF &bounds);
    void buildKeyBlock(const QList<SimilarityMatrix> &matrices, const QRectF &bounds);
    std::function<QColor(double)> createColorTransformation(const QList<SimilarityMatrix> &matrices, const QColor &baseColor);
    std::function<QColor(double)> createColorTransformation(const SimilarityMatrix &matrix, const ColorScheme3 &colorScheme);
    std::function<QColor(double)> createOrderedTransformation(std::vector<double> &values, const ColorScheme3 &colorScheme);
    std::function<int(double)> createQuantileTransformation(const SimilarityMatrix &matrix);

    void updateBoxes();
protected:
    virtual void wheelEvent (QWheelEvent * event ) override;
private:
    int baseColorIndex;
    QGraphicsScene *scene;

    std::vector<std::function<QColor(double)>> toColorBaseRed;
    std::vector<std::function<QColor(double)>> toColorBaseGreen;
    std::vector<std::function<QColor(double)>> toColorBaseBlue;

    std::vector<std::function<int(double)>> withinQuantile;
    QList<QString> labelNames;
    QList<QGraphicsRectItem*> boxes;
    int _numScheduledScalings = 0;

    int currLowerQuantile = 0;
    int currUpperQuantile = 100;

    std::vector<QColor> baseColors = {QColor::fromRgb(255, 126, 126), // red
                                      QColor::fromRgb(122, 255, 159), // green
                                      QColor::fromRgb(122, 177, 255) // blue
                                     };

    QList<ColorScheme3> colorSchemes = {
                                        {"YlOrRd", QColor::fromRgb(255, 237, 160), QColor::fromRgb(254, 178, 76), QColor::fromRgb(240, 59, 32)},
                                        {"GnBu", QColor::fromRgb(224, 243, 219), QColor::fromRgb(168, 221, 181), QColor::fromRgb(67, 162, 202)},
                                        {"BuGn", QColor::fromRgb(229, 245, 249), QColor::fromRgb(153, 216, 201), QColor::fromRgb(44, 162, 95)},
                                       };

private:
    class BoxItem : public QGraphicsRectItem {
      public:
        BoxItem(const QRectF &rect, QGraphicsItem * parent = 0) : QGraphicsRectItem(rect, parent)
        {
            setAcceptHoverEvents(true);
        }
      protected:
        virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override
        {
            auto color = brush().color().lighter(150);
            setPen(QPen(color, 10));

            update();
        }
        virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) override
        {
            auto color = brush().color();
            setPen(QPen(color, 0));

            update();
        }
    };

signals:
    void boxSelected(QPair<QString, QString> participants, QString method, QString value);
public slots:
    void boxSelectionChanged();
    void scalingTime(qreal x);
    void animFinished();
    void lowerQuantileChanged(int lowerQuantile);
    void upperQuantileChanged(int upperQuantile);
    void baseColorChanged(int baseColorIndex);

};

#endif // SIMILARITYMATRIXVIEW_H
