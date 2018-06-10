#include "similaritymatrixview.h"

#include <cassert>
#include <utility>
#include <map>
#include <QTimeLine>
#include <QGraphicsTextItem>

SimilarityMatrixView::SimilarityMatrixView(QWidget *parent) : QGraphicsView(parent)
{
    scene = new QGraphicsScene();
    this->setScene(scene);

    QObject::connect(scene, &QGraphicsScene::selectionChanged, this, &SimilarityMatrixView::boxSelectionChanged);

    baseColorIndex = 0;
}

void SimilarityMatrixView::lowerQuantileChanged(int lowerQuantile)
{
    currLowerQuantile = lowerQuantile;
    updateBoxes();
}

void SimilarityMatrixView::updateBoxes()
{
    for(const auto &box : boxes) {
        double value = box->data(3).toString().toDouble();
        double index = box->data(4).toInt();

        if (withinQuantile[index](value) < currLowerQuantile || withinQuantile[index](value) > currUpperQuantile) {
            box->setPen(QColor::fromRgb(180, 180, 180));
            box->setBrush(QColor::fromRgb(0, 0, 0, 0));
            box->setAcceptHoverEvents(false);
        } else {
            QColor fillColor;
            if (baseColorIndex == 0) {
                fillColor = toColorBaseRed[index](value);
            } else if (baseColorIndex == 1) {
                fillColor = toColorBaseGreen[index](value);
            } else if (baseColorIndex == 2) {
                fillColor = toColorBaseBlue[index](value);
            }

            box->setPen(fillColor);
            box->setBrush(fillColor);
            box->setAcceptHoverEvents(true);
        }
    }

}

void SimilarityMatrixView::upperQuantileChanged(int upperQuantile)
{
    currUpperQuantile = upperQuantile;
    updateBoxes();
}


void SimilarityMatrixView::wheelEvent ( QWheelEvent * event )
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15; // see QWheelEvent documentation
    _numScheduledScalings += numSteps;
    if (_numScheduledScalings * numSteps < 0) // if user moved the wheel in another direction, we reset previously scheduled scalings
        _numScheduledScalings = numSteps;

    QTimeLine *anim = new QTimeLine(350, this);
    anim->setUpdateInterval(20);

    connect(anim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
    connect(anim, SIGNAL (finished()), SLOT (animFinished()));
    anim->start();
}


void SimilarityMatrixView::keyPressEvent(QKeyEvent* event)
{
   int numSteps;
   switch (event->key())
   {
   case Qt::Key_Plus:
       numSteps = 1;
       break;
   case Qt::Key_Minus:
       numSteps = -1;
       break;
   }
    std::cout << "numsteps:" << numSteps << std::endl;
   _numScheduledScalings += numSteps;
   if (_numScheduledScalings * numSteps < 0) // if user moved the wheel in another direction, we reset previously scheduled scalings
       _numScheduledScalings = numSteps;

   QTimeLine *anim = new QTimeLine(350, this);
   anim->setUpdateInterval(20);

   connect(anim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
   connect(anim, SIGNAL (finished()), SLOT (animFinished()));
   anim->start();
   // Otherwise pass to QGraphicsView.
   QGraphicsView::keyPressEvent(event);
}

void SimilarityMatrixView::scalingTime(qreal x)
{
    qreal factor = 1.0+ qreal(_numScheduledScalings) / 300.0;
    scale(factor, factor);
}

void SimilarityMatrixView::animFinished()
{
    if (_numScheduledScalings > 0)
        _numScheduledScalings--;
    else
        _numScheduledScalings++;
    sender()->~QObject();
}

void SimilarityMatrixView::boxSelectionChanged()
{
    for(const auto &item : scene->selectedItems()) {
        emit boxSelected(QPair<QString, QString>(item->data(0).toString(), item->data(1).toString()), item->data(2).toString(), item->data(3).toString());
    }
}

void SimilarityMatrixView::baseColorChanged(int baseColorIndex)
{
    this->baseColorIndex = baseColorIndex;
    updateBoxes();
}

void SimilarityMatrixView::build(SimilarityMatrix &matrix, const ClusteringResult &result)
{
    QList<SimilarityMatrix> matrices;
    matrices += matrix;
    build(matrices, result);
}

void SimilarityMatrixView::build(const QList<SimilarityMatrix> &matrices, const ClusteringResult &result)
{
    scene->clear();
    scene->setSceneRect(0,0,2600,2600);
    auto size = this->sceneRect();

    if (matrices.empty()) {
        return;
    }

    boxes.clear();
    toColorBaseRed.clear();
    toColorBaseGreen.clear();
    toColorBaseBlue.clear();
    withinQuantile.clear();

    for(int i = 0; i < matrices.size(); i++) {
        toColorBaseRed.push_back(createColorTransformation(matrices[i], colorSchemes[0]));
        toColorBaseGreen.push_back(createColorTransformation(matrices[i], colorSchemes[1]));
        toColorBaseBlue.push_back(createColorTransformation(matrices[i], colorSchemes[2]));
        withinQuantile.push_back(createQuantileTransformation(matrices[i]));
    }

    int nofParticpants = matrices[0].labels.size();
    labelNames = matrices[0].labels;



    int marginX = 400;
    int marginY = 400;

    double sceneWidth = size.width() - 2 * marginX;
    double sceneHeight = size.height() - 2 * marginY;

    int dendroSize = 350;
    int dendroMargin = 85;

    std::cout << sceneHeight << std::endl;

    double cmpBlockWidth = (sceneWidth-dendroSize-dendroMargin) / (nofParticpants);
    double cmpBlockHeigth = (sceneHeight-dendroSize-dendroMargin) / (nofParticpants);

    double blockPaddingFraction = 0.1;

    double paddingBlockWidth = blockPaddingFraction * cmpBlockWidth;
    double paddingBlockHeight = blockPaddingFraction * cmpBlockHeigth;

    //buildKeyBlock(matrices, QRectF((size.width() - 300) / 2 , 0, 300, 300));
    //buildKeyBlock(matrices, QRectF(0, sceneHeight / 2, 100, 100));

    DendrogramItem *verticalDendrogram = new DendrogramItem(dendroSize, cmpBlockHeigth, QColor::fromRgb(50, 50, 50), true, true, 30, 6);
    DendrogramItem *horizontalDendrogram = new DendrogramItem(dendroSize, cmpBlockWidth, QColor::fromRgb(50, 50, 50), true, true, 30, 6);


    verticalDendrogram->build(result);
    verticalDendrogram->setInterativeModeEnabled(false);
    horizontalDendrogram->build(result);
    horizontalDendrogram->setInterativeModeEnabled(false);

    QTransform transform;
    transform.rotate(-90);
    horizontalDendrogram->setTransform(transform);

    verticalDendrogram->setPos(marginX, (cmpBlockHeigth/2) + marginY);
    horizontalDendrogram->setPos(marginX + dendroSize + dendroMargin + (cmpBlockWidth / 2), sceneHeight + marginY);

    this->scene->addItem(verticalDendrogram);
    this->scene->addItem(horizontalDendrogram);

    for(int y = 0; y < nofParticpants; ++y) {
        for(int x = 0; x < nofParticpants; x++) {
            auto rect = QRectF(marginX + dendroMargin + dendroSize + (x * cmpBlockWidth), marginY + (y * cmpBlockHeigth) + 20, cmpBlockWidth-paddingBlockWidth, cmpBlockHeigth-paddingBlockHeight);
            buildCmpBlock(matrices, result.leafsOrder[x], result.leafsOrder[y], rect);
        }
    }

    updateBoxes();
}


QPixmap SimilarityMatrixView::getImageOfColorSchemeSample(int width, int height, int sampleSize)
{
    assert(sampleSize > 1);
    assert(width > 0 && height > 0);

    QPixmap csSample(width, height);
    csSample.fill(Qt::transparent);

    QPainter *painter = new QPainter(&csSample);

    std::vector<double> values;

    for(int i = 0; i < 100; i++) {
        values.push_back(i);
    }

    auto toColor = createOrderedTransformation(values, colorSchemes[baseColorIndex]);

    if(currLowerQuantile < currUpperQuantile) {
        double blockWidth = width / double(sampleSize);
        double valueWidth = (currUpperQuantile - currLowerQuantile) / double(sampleSize);

        double x = 0.;
        double value = currLowerQuantile;

        for(int i = 0; i < sampleSize; i++) {
            QBrush brush(toColor(100 - value));
            painter->fillRect(QRect(x, 0, blockWidth, height), brush);

            x += blockWidth;
            value += valueWidth;
        }
    }

    delete painter;
    return csSample;
}

void SimilarityMatrixView::buildKeyBlock(const QList<SimilarityMatrix> &matrices, const QRectF &bounds)
{
    int nrows = matrices.size() > 1 ?  2 : 1;
    int ncolumns = int((double(matrices.size())/nrows)+0.5);

    double blockWidth = bounds.width() / ncolumns;
    double blockHeight = bounds.height() / nrows;

    blockWidth -= 10;
    blockHeight -= 10;


    for(int row = 0; row < nrows; ++row) {
        for(int column = 0; column < ncolumns; ++column) {

            auto rect = QRectF(bounds.x() + (column * (blockWidth+10)), bounds.y() + (row * (blockHeight+10)), blockWidth, blockHeight);

            int matrixIndex = column + (row * ncolumns);

            if(matrixIndex >= matrices.size())
                continue;

            QGraphicsRectItem *rectItem = new QGraphicsRectItem(rect);
            QGraphicsTextItem *textItem = new QGraphicsTextItem(matrices[matrixIndex].name);

            textItem->setPos(rect.x() + 3, rect.y() + 3);
            textItem->setTextWidth(rect.width() - 5);
            textItem->setFont(QFont("Noto", 16));
            textItem->setDefaultTextColor(QColor::fromRgb(130, 130, 130, 100));

            rectItem->setPen(QColor::fromRgb(180, 180, 180, 100));
            rectItem->setBrush(QColor::fromRgb(200, 200, 200, 100));

            scene->addItem(rectItem);
            scene->addItem(textItem);
        }
    }
}

void SimilarityMatrixView::buildCmpBlock(const QList<SimilarityMatrix> &matrices, int i, int j, const QRectF &bounds)
{
    int nrows = matrices.size() > 1 ?  2 : 1;
    int ncolumns = int((double(matrices.size())/nrows)+0.5);

    double blockWidth = bounds.width() / ncolumns;
    double blockHeight = bounds.height() / nrows;

    blockWidth -= 10;
    blockHeight -= 10;


    for(int row = 0; row < nrows; ++row) {
        for(int column = 0; column < ncolumns; ++column) {

            auto rect = QRectF(bounds.x() + (column * (blockWidth+10)), bounds.y() + (row * (blockHeight+10)), blockWidth, blockHeight);

            int matrixIndex = column + (row * ncolumns);

            QGraphicsRectItem *item = i == j ? new QGraphicsRectItem(rect) : new BoxItem(rect);

            if(matrixIndex >= matrices.size())
                continue;

            if (i == j) {
                item->setPen(QColor::fromRgb(180, 180, 180));
                item->setBrush(QColor::fromRgb(0, 0, 0, 0));
            } else {
                item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
                item->setData(0, labelNames[i]);
                item->setData(1, labelNames[j]);
                item->setData(2, matrices[matrixIndex].name);
                item->setData(3, QString::number(matrices[matrixIndex].elements[i][j]));
                item->setData(4, matrixIndex);

                QColor fillColor;

                if (baseColorIndex == 0) {
                    fillColor = toColorBaseRed[matrixIndex](matrices[matrixIndex].elements[i][j]);
                } else if (baseColorIndex == 1) {
                    fillColor = toColorBaseGreen[matrixIndex](matrices[matrixIndex].elements[i][j]);
                } else if (baseColorIndex == 2) {
                    fillColor = toColorBaseBlue[matrixIndex](matrices[matrixIndex].elements[i][j]);
                }

                item->setPen(fillColor);
                item->setBrush(fillColor);

                auto strNum = QString::number(matrices[matrixIndex].elements[i][j]).left(4);
                auto strName = matrices[matrixIndex].name;

                item->setToolTip("<b>Score   :</b> " + strNum + "<br></br><b>Method: </b>" + strName);
                boxes.append(item);
            }

            QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();

            effect->setColor(QColor::fromRgb(10, 10, 10));
            effect->setOffset(0);
            effect->setBlurRadius(9);

            item->setGraphicsEffect(effect);

            scene->addItem(item);
        }
    }
}

std::function<QColor(double)> SimilarityMatrixView::createOrderedTransformation(std::vector<double> &values, const ColorScheme3 &colorScheme)
{
    std::sort(values.begin(), values.end());

    return [colorScheme, values](double x) {
        auto itLower = std::upper_bound(values.begin(), values.end(), x);

        int indexLower = itLower - values.begin();

        double value = double(indexLower) / (values.size() - 1.0);
        QColor fromColor, toColor;

        if (value < 0.5) {
            fromColor = colorScheme.class03;
            toColor = colorScheme.class02;
        } else {
            fromColor = colorScheme.class01;
            toColor = colorScheme.class02;
            value = 1. - value;
        }

        int r1,g1,b1;
        int r2,g2,b2;
        int r,g,b;

        fromColor.getRgb(&r1, &g1, &b1);
        toColor.getRgb(&r2, &g2, &b2);

        double alpha = value / 0.5;

        r = r1 * (1. - alpha) + r2 * alpha;
        g = g1 * (1. - alpha) + g2 * alpha;
        b = b1 * (1. - alpha) + b2 * alpha;

        return QColor::fromRgb(r, g, b);
    };
}

std::function<QColor(double)> SimilarityMatrixView::createColorTransformation(const SimilarityMatrix &matrix, const ColorScheme3 &colorScheme)
{
    int n = matrix.elements.size();

    std::vector<double> values;

    for(int i = 0; i < n; ++i) {
        for(int j = i+1; j < n; ++j) {
            values.push_back(matrix.elements[i][j]);
        }
    }
    return createOrderedTransformation(values, colorScheme);
}


std::function<int(double)> SimilarityMatrixView::createQuantileTransformation(const SimilarityMatrix &matrix)
{
    int n = matrix.elements.size();

    std::vector<double> values;

    for(int i = 0; i < n; ++i) {
        for(int j = i+1; j < n; ++j) {
            values.push_back(matrix.elements[i][j]);
        }
    }
    std::sort(values.begin(), values.end());

    return [values, n](double x) {

        auto it = std::lower_bound(values.begin(), values.end(), x);
        int index = it - values.begin();

        index = values.size() - 1 - index;

        double value = double(index) / (values.size() - 1.0);
        std::cout << int(value * 100) << std::endl;
        return int(value * 100);
    };

}

std::function<QColor(double)> SimilarityMatrixView::createColorTransformation(const QList<SimilarityMatrix> &matrices, const QColor &baseColor)
{
    double max = 0.0;
    double min = 1e10;

    for(auto matrix : matrices) {
        for(int i = 0; i < matrix.elements[0].size(); ++i) {
            for(int j = i+1; j < matrix.elements[i].size(); ++j) {
                if (max < matrix.elements[i][j]) {
                    max = matrix.elements[i][j];
                }
                if (min > matrix.elements[i][j]) {
                    min = matrix.elements[i][j];
                }
            }
        }
    }

    assert(max != min);

    return [max, min, baseColor](double x) {
        double q = (x - min) / (max - min);
        q = std::log(x / max);
        return baseColor.darker(100. + ((1.-q) * 100));
    };
}
