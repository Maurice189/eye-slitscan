#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QDesktopWidget>
#include <clustering/include/hac.h>
#include <slitscan-generator/include/genslitscan.h>
#include <QMap>
#include <QVector>
#include <QSet>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QtMultimediaWidgets/QGraphicsVideoItem>
#include <QInputDialog>
#include <QProgressDialog>

#include "slitscanview.h"
#include "similaritymatrixview.h"
#include "videooverlaywidget.h"
#include "screenshot.h"

#include "../core/clustering/copheneticcorrelation.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        csvExport(CSVExport(clusteringsData, sourceInfo)),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("SLITSCAN-CL");

    clusteringMethodGroup = new QActionGroup(this);

    clusteringMethodGroup->addAction(ui->actionSingleLinkage);
    clusteringMethodGroup->addAction(ui->actionCompleteLinkage);
    clusteringMethodGroup->addAction(ui->actionAvgGroupLinkage);
    ui->actionAvgGroupLinkage->setChecked(true);

    ui->slitscanView->setScene(&scene);
    ui->slitscanView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->slitscanView->setInteractive(true);
    ui->slitscanView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->slitscanView->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);
    ui->slitscanView->show();

    ui->miniMapView->setScene(&miniMapScene);
    ui->miniMapView->setInteractive(true);
    ui->miniMapView->show();

    ui->timespanView->setScene(&timespanScene);
    ui->timespanView->setInteractive(true);
    ui->timespanView->show();

    ui->timespanView->setVisible(false);
    ui->measuresTree->setIconSize(QSize(22, 22));


    ui->miniMapView->setVisible(false);
    updateBaseColorScheme(0);

    ui->dendrogramView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->dendrogramView->setStyleSheet( "QGraphicsView { border-style: none; }" );
    ui->slitscanView->setStyleSheet( "QGraphicsView { border-style: none; }" );
    ui->dendrogramView->setScene(&dendrogramScene);
    ui->videoView->setVisible(false);

    ui->dendrogramView->setAlignment( Qt::AlignTop | Qt::AlignLeft );
    ui->dendrogramView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    QGraphicsDropShadowEffect *dseLeft = new QGraphicsDropShadowEffect();
    QGraphicsDropShadowEffect *dseRight = new QGraphicsDropShadowEffect();

    dseLeft->setColor(Qt::black);
    dseRight->setColor(Qt::black);

    dseLeft->setOffset(0);
    dseRight->setOffset(0);
    dseLeft->setBlurRadius(20);
    dseRight->setBlurRadius(20);

    //ui->leftSlitImg->setGraphicsEffect(dseLeft);
    //ui->rightSlitImg->setGraphicsEffect(dseRight);

    player = new QMediaPlayer;
    player->setNotifyInterval(50);

    ui->measuresTree->addTopLevelItems(measureItems);
    ui->participantsTree->addTopLevelItems(pariticpantItems);

    slider = new FrameSliderItem(0, 10);
    slider->setFlags(QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemIgnoresTransformations);
    slider->setVisible(false);

    scene.addItem(slider);

    ditem = new DendrogramItem(150, 10 + slitscanHeight, QColor::fromRgb(50, 50, 50), true, false, 2);
    ditem->setFlags(QGraphicsItem::ItemIgnoresTransformations);
    dendrogramScene.addItem(ditem);

    selector = new TimeSpanSelector(10000, 30);
    timespanScene.addItem(selector);

    ui->dendrogramView->verticalScrollBar()->setSliderPosition(ui->slitscanView->verticalScrollBar()->sliderPosition());
    ui->historyList->setIconSize(QSize(320, 120));
    ui->historyList->setAlternatingRowColors(false);

    QObject::connect(player, &QMediaPlayer::mediaStatusChanged,
                     [&](QMediaPlayer::MediaStatus status){
        if(status == QMediaPlayer::LoadedMedia) //when loading is finished
        {
            slider->setMediaLengthMs(player->duration());
            player->play();
            player->pause();
        }
    });

    rvoP1 = new RepresentationViewerObject(ui->leftP);
    rvoP2 = new RepresentationViewerObject(ui->rightP);

    QObject::connect(ditem, &DendrogramItem::externSubtreeActivated, this, &MainWindow::dendrogramSubtreeActivated);
    QObject::connect(ui->radioSingle, &QRadioButton::toggled, this, &MainWindow::matrixOptionChanged);
    QObject::connect(ui->radioSlit, &QRadioButton::toggled, rvoP1, &RepresentationViewerObject::radioSlitButtonToggled);
    QObject::connect(ui->radioSlit, &QRadioButton::toggled, rvoP2, &RepresentationViewerObject::radioSlitButtonToggled);
    QObject::connect(ui->radioGazeX, &QRadioButton::toggled, rvoP1, &RepresentationViewerObject::radioGazeXButtonToggled);
    QObject::connect(ui->radioGazeX, &QRadioButton::toggled, rvoP2, &RepresentationViewerObject::radioGazeXButtonToggled);
    QObject::connect(ui->radioGazeY, &QRadioButton::toggled, rvoP1, &RepresentationViewerObject::radioGazeYButtonToggled);
    QObject::connect(ui->radioGazeY, &QRadioButton::toggled, rvoP2, &RepresentationViewerObject::radioGazeYButtonToggled);

    QObject::connect(ui->selectAll, &QPushButton::clicked, this, &MainWindow::selectAllViewers);
    QObject::connect(ui->participantsTree, &QTreeWidget::itemChanged, this, &MainWindow::participantsTreeItemChanged);
    QObject::connect(ui->matrixView, &SimilarityMatrixView::boxSelected, this, &MainWindow::boxInMatrixSelected);
    QObject::connect(slider, &FrameSliderItem::sliderMoved, player, &QMediaPlayer::setPosition);
    QObject::connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::videoPositionChanged);
    QObject::connect(ui->slitscanView, &SlitscanView::mouseDragged, slider, &FrameSliderItem::setSliderPos);
    QObject::connect(ui->slitscanView->verticalScrollBar(), &QScrollBar::valueChanged, ui->dendrogramView->verticalScrollBar(), &QScrollBar::setValue);
    QObject::connect(ui->slitscanView->horizontalScrollBar(), &QScrollBar::valueChanged, ui->timespanView->horizontalScrollBar(), &QScrollBar::setValue);
    QObject::connect(selector, &TimeSpanSelector::handlesMoved, this, &MainWindow::handlesMoved);
    QObject::connect(ui->historyList, &HistoryListWidget::historyListItemSelected, this, &MainWindow::historyListSelectionChanged);
    QObject::connect(ui->historyList, &HistoryListWidget::deselected, this, &MainWindow::historyListDeselected);
    QObject::connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::clearHistoryList);
    QObject::connect(ui->selectVisibleArea, &QPushButton::clicked, this, &MainWindow::selectVisibleArea);

    QObject::connect(ui->spinLowerQuantile, QOverload<int>::of(&QSpinBox::valueChanged), ui->matrixView, &SimilarityMatrixView::lowerQuantileChanged);
    QObject::connect(ui->spinUpperQuantile, QOverload<int>::of(&QSpinBox::valueChanged), ui->matrixView, &SimilarityMatrixView::upperQuantileChanged);
    QObject::connect(ui->comboBaseColor, &QComboBox::currentTextChanged, this, &MainWindow::baseColorChanged);
    QObject::connect(ui->spinLowerQuantile, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateBaseColorScheme);
    QObject::connect(ui->spinUpperQuantile, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateBaseColorScheme);

}


void MainWindow::showLicenseDialog()
{
    QMessageBox::about(this, "3-Clause BSD License",
 "Copyright: (2017) Maurice Koch\n"
 "License: BSD-3\n\n"

 "All rights reserved.\n\n"

 "Redistribution and use in source and binary forms, with or without "
 "modification, are permitted provided that the following conditions are met:\n\n"

 "1. Redistributions of source code must retain the above copyright notice,"
    "this list of conditions and the following disclaimer.\n\n"

 "2. Redistributions in binary form must reproduce the above copyright notice,"
   "this list of conditions and the following disclaimer in the documentation"
   "and/or other materials provided with the distribution.\n\n"

 "3. Neither the name of the copyright holder nor the names of its contributors"
   "may be used to endorse or promote products derived from this software"
   "without specific prior written permission.\n\n"

 "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS "
 "AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE "
 "IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE "
 "ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE "
 "LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR "
 "CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF "
 "SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS "
 "INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN "
 "CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) "
 "ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE "
 "POSSIBILITY OF SUCH DAMAGE.");
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(this, "About & Acknowledgement", "This tool was developed in the context "
                                                     "of an Bachelor Thesis at the University of Stuttgart - "
                                                     "<a href='https://www.visus.uni-stuttgart.de/en/institute.html'>Visualisation Research Centre</a>. "
                                                     "We hope that it illustrates the usefullness of the "
                                                     "Slit-Scan-Visualisation technique "
                                                     "in the context of scanpath comparision.\n\n"
                                                     "Many thanks goes to my supervisor <a href='http://www.vis.uni-stuttgart.de/institut/mitarbeiter/kuno-kurzhals.html'>Kuno Kurzhals</a> for his great support.");
}


void MainWindow::dendrogramSubtreeActivated(QStringList leafIds)
{
    for(auto &p : checkedParticipants) {
        p.active = leafIds.contains(p.id);
        //std::cout << p.id.toStdString() << " is " << (p.active ? "" : " not ") << " active " << std::endl;
    }
    videoItem->redraw();
}


void MainWindow::updateBaseColorScheme(int i)
{
    ui->colorSchemeLabel->setPixmap(ui->matrixView->getImageOfColorSchemeSample(70, 20, 5));
}

void MainWindow::baseColorChanged(const QString &baseColorText)
{
    int baseColorIndex = 0;

    if (baseColorText.toLower() == "ylorbd") {
       baseColorIndex = 0;
    } else if (baseColorText.toLower() == "ylgnbu") {
       baseColorIndex = 1;
    } else if (baseColorText.toLower() == "bugn") {
       baseColorIndex = 2;
    }

    ui->matrixView->baseColorChanged(baseColorIndex);
    updateBaseColorScheme(0);
}

void MainWindow::setBeeSwarmEnabled(bool enabled)
{
    ui->bee_2->setVisible(enabled);
    slider->setVisible(enabled);
}

void MainWindow::zoomingFactorChanged(int value)
{
    ui->slitscanView->setUpdatesEnabled(false);

    slitscanHeight = 130 + (5 * value);

    int visibleIndex = 0;
    for(int i = 0; i < participants.size(); ++i) {
        slitScanItems[i]->setHeightOfSlitscan(slitscanHeight);
        if(pariticpantItems[i]->checkState(0) == Qt::Checked) {
            slitScanItems[i]->show();
            slitScanItems[i]->setY(visibleIndex * (slitscanHeight + 10));
            visibleIndex++;
        } else {
            slitScanItems[i]->hide();
        }
    }
    scene.setSceneRect(0, 0, slitscanWidth, (visibleIndex)* (slitscanHeight + 10));
    slider->setHeight(scene.height());

    delete ditem;
    ditem = new DendrogramItem(150, 10 + slitscanHeight, QColor::fromRgb(50, 50, 50), true, false);
    ditem->setFlags(QGraphicsItem::ItemIgnoresTransformations);
    QObject::connect(ditem, &DendrogramItem::externSubtreeActivated, this, &MainWindow::dendrogramSubtreeActivated);
    dendrogramScene.addItem(ditem);
    dendrogramScene.setSceneRect(scene.sceneRect());

    if (stateActive) {
        updateVisualisation(currentClusteringState);
    }

    ui->slitscanView->setUpdatesEnabled(true);
}

void MainWindow::slitscanModeSelection(int mode)
{
    if (mode == 0) {
        ui->slitscanView->resetMatrix();
        ui->timespanView->show();

        for(const auto &item : slitScanItems) {
            item->setVisibleAreaToMaximum(false);
        }

        ui->slitscanView->setZoomingEnabled(false);
        ui->measuresTree->setEnabled(true);
        ui->historyList->setEnabled(true);
        ui->clusteringFrame->setEnabled(true);
        ui->dendrogramView->setHidden(false);
        ui->history->setHidden(false);

        auto horizontalBoundaryX = getHandleSpanXRange();
        slider->setHorizontalBoundaries(horizontalBoundaryX.first, horizontalBoundaryX.second);
        //ui->bee->setHidden(false);

    } else if (mode == 1) {
        ui->timespanView->hide();

        for(const auto &item : slitScanItems) {
            item->setVisibleAreaToMaximum(true);
        }

        ui->slitscanView->setZoomingEnabled(true);
        ui->measuresTree->setEnabled(false);
        ui->historyList->setEnabled(false);
        ui->clusteringFrame->setEnabled(false);
        ui->dendrogramView->setHidden(true);
        ui->history->setHidden(true);
        ui->slitscanView->update();
        slider->setHorizontalBoundaries(0, slitscanWidth);
        //ui->bee->setHidden(true);
    }
}



void MainWindow::selectVisibleArea()
{
    QRect viewport_rect(0, 0, ui->slitscanView->viewport()->width(), ui->slitscanView->viewport()->height());
    QRectF visible_scene_rect = ui->slitscanView->mapToScene(viewport_rect).boundingRect();

    int from = visible_scene_rect.x();
    int to = from + visible_scene_rect.width();

    selector->setHandlesPosition(QPair<int, int>(from, to));
    handlesMoved(QPointF(from, to));
    ui->timespanView->viewport()->repaint();
}


void MainWindow::selectAllArea()
{
    int from = 0;
    int to = slitscanWidth;

    selector->setHandlesPosition(QPair<int, int>(from, to));
    handlesMoved(QPointF(from, to));
    ui->timespanView->viewport()->repaint();
}




void MainWindow::clearHistoryList()
{
    for(auto &data : clusteringsData) {
        if (data.result.tree) {
            delete data.result.tree;
        }
    }
    clusteringsData.clear();
    ui->historyList->clear();
    ditem->setVisible(false);

    stateActive = false;
}

void MainWindow::historyListDeselected(HistoryListItem *item)
{
    for(int i = 0; i < ui->historyList->count(); i++) {
        auto other = dynamic_cast<HistoryListItem*>(ui->historyList->item(i));
        if (other != item) {
            other->disableHighlight();
            other->setHideHint(false);
        } else {
            std::cout << " ----- > Deselected item at : " << i << std::endl;
        }
    }
}


bool MainWindow::equalClusteringStates(const ClusteringStateData &d1, const ClusteringStateData &d2)
{
    bool compatible = correlationCompatibleClusteringStates(d1, d2);

    bool spanEqual = (d1.info.from == d2.info.from && d1.info.to == d2.info.to) ||
                     (d1.info.to == d2.info.from && d1.info.from == d2.info.to);

    bool titleEqual = d1.info.info.title == d2.info.info.title;

    return compatible && spanEqual && titleEqual;
}

bool MainWindow::correlationCompatibleClusteringStates(const ClusteringStateData &d1, const ClusteringStateData &d2)
{
    QSet<QString> d1Set;
    QSet<QString> d2Set;

    for(const auto &s : d1.result.leafs) {
        d1Set.insert(QString::fromStdString(s));
    }
    for(const auto &s : d2.result.leafs) {
        d2Set.insert(QString::fromStdString(s));
    }

    return d1Set == d2Set;
}

void MainWindow::historyListSelectionChanged(HistoryListItem *item)
{
    bool uniform = (ui->uniformScaleCheck->checkState() == Qt::Checked);
    auto itemState = item->getClusteringState();

    std::cout << "History selection changed" << std::endl;

    for(int i = 0; i < ui->historyList->count(); i++) {
        auto other = dynamic_cast<HistoryListItem*>(ui->historyList->item(i));
        auto otherState = other->getClusteringState();

        if (other != item) {
            // The correlation between two HACs can be only calculated
            // if both share the same elements.
            other->setActivated(false);
            bool isCompatible = correlationCompatibleClusteringStates(itemState, other->getClusteringState());
            if (isCompatible) {
                if (uniform) {
                    CopheneticCorrelation corr(Clustering::createUniformClustering(itemState.result), Clustering::createUniformClustering(otherState.result), itemState.result.leafs, 100);
                    other->enableHighlight(corr.correlationCoefficient(true));
                } else {
                    CopheneticCorrelation corr(itemState.result, otherState.result, itemState.result.leafs, 100);
                    other->enableHighlight(corr.correlationCoefficient(true));
                }
            } else {
                other->setHidden(true);
            }
        } else {
            other->setHidden(false);
            other->setActivated(true);
            other->disableHighlight();
        }
    }
    currentClusteringState = item->getClusteringState();
    // Show the selected clustering state
    updateVisualisation(item->getClusteringState());
    std::cout << " ------------------------------------- " << std::endl;
}


void MainWindow::createSlitScanMiniMap()
{
    int n = checkedParticipants.size();
    double spectrogramHeight = 40. / n;

    QPixmap minimap(1000, 40);
    minimap.fill(Qt::transparent);
    QPainter *painter = new QPainter(&minimap);

    for(int i = 0; i < checkedParticipants.size(); ++i) {
        const auto &data = checkedParticipants.at(i);
        QPixmap spectrogram = QPixmap::fromImage(data.imgSpectrogram.scaled(1000, spectrogramHeight, Qt::IgnoreAspectRatio));
        painter->drawPixmap(0, int(i * spectrogramHeight), spectrogram);
    }

    QGraphicsPixmapItem *miniMapItem = new QGraphicsPixmapItem(minimap);
    miniMapScene.addItem(miniMapItem);

    delete painter;
}

void MainWindow::playPauseToggled(bool checked)
{
    if(checked) {
        QObject::disconnect(slider, &FrameSliderItem::sliderMoved, player, &QMediaPlayer::setPosition);
        QObject::connect(player, &QMediaPlayer::positionChanged, slider, &FrameSliderItem::setSliderPosByDuration);
        ui->playPauseButton->setText("Pause");
        player->play();
    } else {
        QObject::connect(slider, &FrameSliderItem::sliderMoved, player, &QMediaPlayer::setPosition);
        QObject::disconnect(player, &QMediaPlayer::positionChanged, slider, &FrameSliderItem::setSliderPosByDuration);
        ui->playPauseButton->setText("Play");
        player->pause();
    }
}

void MainWindow::videoPositionChanged(qint64 duration)
{
    int pos = slitscanWidth * (double(duration) / player->duration());

    auto horizontalBoundaryX = getHandleSpanXRange();

    if (ui->modeSelector->currentIndex() == 1) {
        if(pos >= slitscanWidth) {
            player->setPosition(0.);
        }
    }

    else if (pos > horizontalBoundaryX.second) {
        double newDuration = double(horizontalBoundaryX.first * player->duration()) / slitscanWidth;
        player->setPosition(newDuration);
    }

    int hours = duration / (1000 * 3600);
    int min = (duration % (1000 * 3600)) / (1000 * 60);
    int sec = (duration % (1000 * 60)) / 1000;

    QString time("");

    if (hours < 10) {
        time += "0";
    }

    time += QString::number(hours) + ":";

    if (min < 10) {
        time += "0";
    }

    time += QString::number(min) + ":";

    if(sec < 10) {
        time += "0";
    }

    time += QString::number(sec);

    ui->videoTimeLabel->setText(time);
    //videoItem->setGazePointX(checkedParticipants, pos);

    int videoMode = ui->radioGaze->isChecked() ? 0 : 1;
    videoItem->drawGazeData(checkedParticipants, pos, videoMode);
}

void MainWindow::handlesMoved(QPointF newPos)
{
    for(const auto& item : slitScanItems) {
        item->setVisibleArea(QRectF(newPos.x(), 0, newPos.y() - newPos.x(), item->pixmap().height()));
        item->update();
    }

    slider->setHorizontalBoundaries(newPos.x(), newPos.y());
    ui->visibleAreaLabel->setText(QString::number(newPos.x()) + " - " + QString::number(newPos.y()));
    ui->slitscanView->viewport()->update();
}

void MainWindow::selectAllViewers()
{
    for(int i = 0; i < participants.size(); ++i) {
        pariticpantItems[i]->setCheckState(0, Qt::Checked);
    }
}

void MainWindow::deselectAllViewers()
{
    for(int i = 0; i < participants.size(); ++i) {
        pariticpantItems[i]->setCheckState(0, Qt::Unchecked);
    }
}

void MainWindow::participantsTreeItemChanged(QTreeWidgetItem *item, int column)
{
    int visibleIndex = 0;
    int old = checkedParticipants.size();

    checkedParticipants.clear();
    for(int i = 0; i < participants.size(); ++i) {
        if(pariticpantItems[i]->checkState(0) == Qt::Checked) {
            checkedParticipants.append(participants[i]);
            slitScanItems[i]->show();
            slitScanItems[i]->setY(visibleIndex * (slitscanHeight + 10));
            visibleIndex++;
        } else {
            slitScanItems[i]->hide();
        }
    }

    ui->nofViewersLabel->setText(QString::number(checkedParticipants.size()) + " of " + QString::number(participants.size()) + " viewers selected.");
    scene.setSceneRect(0, 0, slitscanWidth, (visibleIndex)* (slitscanHeight + 10));
    slider->setHeight(scene.height() - 10);

    if (checkedParticipants.size() != old) {
        clearHistoryList();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Minus) {
        //emit ui->zoomFactor->stepDown();
        if (zoomFactor > -19) {
            zoomFactor--;
            zoomingFactorChanged(zoomFactor);
        }
    }
    if (event->key() == Qt::Key_Plus) {
        //emit ui->zoomFactor->stepUp();
        if (zoomFactor < 19) {
            zoomFactor++;
            zoomingFactorChanged(zoomFactor);
        }
    }
    /*
    if(event->key() == Qt::Key_Escape) {
        slitscanModeSelection(0);
        ui->modeSelector->setCurrentIndex(0);
    }
    */
    if (event->key() == Qt::Key_Delete) {
        auto selectedItems = ui->measuresTree->selectedItems();
        for(auto item : selectedItems) {
            int found = 0;
            int index = -1;
            for(int i = 0; i < addedMeasures.size(); i++) {
                if (addedMeasures[i].title == item->text(0)) {
                    found++;
                    index = i;
                }
            }
            assert(found == 1);
            std::cout << "REMOVE MEASURE :: " << addedMeasures[index].title.toStdString() << std::endl;
            addedMeasures.removeAt(index);
            measureItems.removeAt(index);
            ui->clusteringCombo->removeItem(index);
            delete item;
        }
    }
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::boxInMatrixSelected(QPair<QString, QString> ids, QString method, QString value)
{
    for(int i = 0; i < participants.size(); ++i) {
        ParticipantData &data = participants[i];

        int from = currentClusteringState.info.from;
        int to = currentClusteringState.info.to;

        auto begin = data.exportData.begin() + from;
        auto end = data.exportData.begin() + to;
        auto exportData = std::vector<TobiiExportData>(begin, end);

        QImage image;
        cv::Mat mat;
        if (data.id == ids.first || data.id == ids.second) {
            mat = data.matSlitscan(cv::Rect(from,0,to-from, data.matSlitscan.rows)).clone();
            image = QImage((unsigned char*) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA8888).rgbSwapped();
            image = image.scaled(QSize(350, 100), Qt::IgnoreAspectRatio);
        }

        if (data.id == ids.first) {
            TimeSpanData dataP1;
            dataP1.trajectoryOfGazes = createTrajectoryFromGazeData(exportData);
            dataP1.aoiStringWTB = data.aoiStringWTB.substr(from, to - from);
            dataP1.aoiString = mergeByFixationIndex(dataP1.aoiStringWTB, exportData);
            dataP1.slitscan = mat;
            rvoP1->update(dataP1);
            //ui->leftSlitImg->setPixmap(QPixmap::fromImage(image));
        }
        else if (data.id == ids.second) {
            TimeSpanData dataP2;
            dataP2.slitscan = mat;
            dataP2.trajectoryOfGazes = createTrajectoryFromGazeData(exportData);
            dataP2.aoiStringWTB = data.aoiStringWTB.substr(from, to - from);
            dataP2.aoiString = mergeByFixationIndex(dataP2.aoiStringWTB, exportData);
            rvoP2->update(dataP2);
            //ui->rightSlitImg->setPixmap(QPixmap::fromImage(image));
        }
    }

    ui->matrixValue->setText(value);
    ui->matrixMethod->setText(method);
    ui->leftParticipant->setText(ids.first);
    ui->rightParticipant->setText(ids.second);
}

void MainWindow::setVideoSource(const std::string &videoSourcePath)
{
    videoItem = new SlitscanVideo(500, 250);
    videoScene = new QGraphicsScene(ui->videoView);

    videoItem->setSourceInfo(videoInfo);
    videoItem->setAspectRatioMode(Qt::IgnoreAspectRatio);
    ui->videoView->setScene(videoScene);

    player->setVideoOutput(videoItem);

    videoScene->addItem(videoItem);
    ui->videoView->show();

    player->setMedia(QUrl::fromLocalFile(QString::fromStdString(videoSourcePath)));
    ui->videoView->setVisible(true);
}

void MainWindow::addSlitScan(QPixmap slitscan, QString label, int positionIndex)
{
    SlitscanItem *item = new SlitscanItem(label);
    auto pos = selector->getHandlePositions();

    QObject::connect(ui->slitscanView->horizontalScrollBar(), &QScrollBar::valueChanged, item, &SlitscanItem::scrollbarValueChanged);

    slitScanItems.push_back(item);
    item->setPixmap(slitscan);
    item->setX(0);
    item->setY(positionIndex * (slitscanHeight + 10));

    item->setVisibleArea(QRectF(pos.x(), 0, pos.y() - pos.x(), item->pixmap().height()));
    if(ui->modeSelector->currentIndex() == 0) {
        item->setVisibleArea(QRectF(pos.x(), 0, pos.y() - pos.x(), item->pixmap().height()));
    } else {
        item->setVisibleAreaToMaximum(true);
    }

    slitscanWidth = slitscan.width();
    slider->setMaximumY(slitscan.width());
    scene.addItem(item);

    scene.setSceneRect(0, 0, slitscan.width(), (positionIndex+1)* (slitscanHeight + 10));
    dendrogramScene.setSceneRect(scene.sceneRect());
    timespanScene.setSceneRect(scene.sceneRect());

    ui->timespanView->setVisible(true);
    slider->setVisible(true);
    slider->setHeight(scene.height());
    slider->setHorizontalBoundaries(0, slitscanWidth);

}

bool MainWindow::loadAOIData(const std::string aoiAbsPath)
{
    AOIImporter aoiImporter;
    if(!aoiImporter.parseViperFormattedXML(aoiAbsPath, dynamicAOIs, sourceInfo)) {
        return false;
    }

    //QFileInfo info(QString::fromStdString(aoiAbsPath));
    //auto videoAbsPath = info.absolutePath() + "/" + QString::fromStdString(sourceInfo.filename);

    //stimuliSource = videoAbsPath.toStdString();
    //stimuliSet = true;

    //videoInfo = SlitscanGenerator::readVideoInfo(stimuliSource);
    //setVideoSource(stimuliSource);

    return true;
}
bool MainWindow::loadStimulus(const std::string stimulusPath)
{

    try {
        videoInfo = SlitscanGenerator::readVideoInfo(stimulusPath);
        setVideoSource(stimulusPath);

        stimuliSource = stimulusPath;
        stimuliSet = true;

        return true;
    } catch (SlitScanVideoError &error) {
        QMessageBox::critical(this, ("Loading video not possible:"), QString("Error while loading : ") + QString::fromStdString(stimulusPath));
    }
    return false;
}

void MainWindow::addParticipant()
{
    QStringList filePathList = QFileDialog::getOpenFileNames(this, tr("Add Participant"), "/home/maurice/Downloads", tr("Tab-Seperated Files (*.tsv)"));

    QProgressDialog progress("Load Participant Data ... ", QString(), 0, filePathList.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setValue(0);

    int atFileIndex = 0;

    try {
        for(auto filePath : filePathList) {
            QFileInfo info(filePath);

            progress.setValue(atFileIndex);
            progress.setLabelText("Load : " + filePath);
            QCoreApplication::processEvents();

            QString label = info.baseName().split("-").first();

            if (label.length() > 5) {
                label = label.left(5);
            }

            ParticipantData data = createDataOfParticipant(info, label);
            std::cout << " Add Participant - " << participants.size() << " : " << label.toStdString() << std::endl;

            participants.push_back(data);
            checkedParticipants.append(data);

            addSlitScan(data.imgSlitscan, label, checkedParticipants.size() - 1);

            QTreeWidgetItem *participant = new QTreeWidgetItem();
            int percantageOfValidSamples = int(100 * getPercantageOfValidSamples(data.exportData));

            QPixmap pix(20, 20);
            QPainter painter(&pix);

            painter.setBrush(participants.back().color);
            painter.drawRect(0, 0, 20, 20);
            painter.setRenderHint(QPainter::Antialiasing);

            participant->setIcon(0, QIcon(pix));
            participant->setCheckState(0, Qt::Checked);
            participant->setText(0, info.baseName() + " (" + QString::number(percantageOfValidSamples) + "%)");
            pariticpantItems.append(participant);
            ui->participantsTree->addTopLevelItems(pariticpantItems);

            atFileIndex++;
        }

        progress.setValue(atFileIndex+1);

        ui->nofViewersLabel->setText(QString::number(pariticpantItems.size()) + " Viewers");
        ui->slitscanView->viewport()->update();
    } catch(std::invalid_argument ex) {
        QMessageBox::critical(this, ("Import of viewer not possible"), ex.what());
    }
    clearHistoryList();
    createSlitScanMiniMap();
}

QPair<int, int> MainWindow::getHandleSpanXRange()
{
    auto span = selector->getHandlePositions();

    int x = span.x();
    int y = span.y();

    if (x < 0) {
        x = 0;
    }

    if (y < 0) {
        y = 0;
    }

    if (x > y) {
        int tmp = y;
        y = x;
        x = tmp;
    }

    return QPair<int, int>(x, y);
}


void MainWindow::matrixOptionChanged()
{
    if(!clusteringsData.isEmpty()) {
        updateVisualisation(currentClusteringState);
    }
}

void MainWindow::updateVisualisation(const ClusteringStateData &state)
{
    auto uniformResult = Clustering::createUniformClustering(state.result);

    try {
        if (uniformScaling) {
            ditem->build(uniformResult);
        } else {
            ditem->build(state.result);
        }
    } catch(std::invalid_argument &ex) {
        QMessageBox::critical(this, tr("Illegal Clustering Results"),
                    tr("The calculated similarity values are near to zero, thus clustering is not executed !"));
        return;
    }

    ditem->setVisible(true);
    ditem->update();
    ditem->setY((slitscanHeight / 2));
    ditem->setDefaultState();
    ui->dendrogramView->viewport()->repaint();

    // Reordering of slitscans according to clustering information
    for(int i = 0; i < checkedParticipants.size(); ++i) {
        int slitscanIndex = -1;
        for(int j = 0; j < checkedParticipants.size(); j++) {
            if (checkedParticipants[j].id == QString::fromStdString(state.result.leafs[i])) {
                slitscanIndex = checkedParticipants[j].insertIndex;
            }
        }
        assert(slitscanIndex != -1);
        slitScanItems[slitscanIndex]->show();
        slitScanItems[slitscanIndex]->setY(i * (slitscanHeight + 10));
    }

    selector->setHandlesPosition(QPair<int, int>(state.info.from, state.info.to));
    handlesMoved(QPointF(state.info.from, state.info.to));
    ui->timespanView->viewport()->repaint();


    QList<SimilarityMatrix> matrices;
    SimilarityMatrix currMatrix;

    for(auto &stateData : clusteringsData) {
        if (state.info.info.title == stateData.info.info.title) {
           currMatrix = stateData.matrix;
        }
        matrices.append(stateData.matrix);
    }

    if (ui->radioSingle->isChecked()) {
        ui->matrixView->build(currMatrix, uniformResult);
    } else if (ui->radioMulti->isChecked()) {
        ui->matrixView->build(matrices, uniformResult);
    }
}

void MainWindow::executeClustering()
{
    if (addedMeasures.empty()) {
        std::cout << "NO MEASURE ADDED YET - CLUSTERING NOT POSSIBLE" << std::endl;
        return;
    }

    if (checkedParticipants.size() < 3) {
        QMessageBox::critical(this, tr("Clustering not possible"),
                    tr("For clustering at least three items are needed."));
        return;
    }

    QPair<int, int> xRange = getHandleSpanXRange();

    if (xRange.second - xRange.first < 5) {
        QMessageBox::critical(this, tr("Invalid range selction"),
                    tr("Only ranges of at least 5 pixels are allowed."));
        return;
    }

    ClusteringMethod clusteringMethod = ClusteringMethod::SINGLE_LINKAGE;
    bool clusteringMethodChecked = false;
    auto checkedAction = clusteringMethodGroup->checkedAction();

    if (checkedAction == ui->actionSingleLinkage) {
        clusteringMethod = ClusteringMethod::SINGLE_LINKAGE;
        clusteringMethodChecked = true;
    }
    if (checkedAction == ui->actionCompleteLinkage) {
        clusteringMethod = ClusteringMethod::COMPLETE_LINKAGE;
        assert(!clusteringMethodChecked);
        clusteringMethodChecked = true;
    }
    if (checkedAction == ui->actionAvgGroupLinkage) {
        assert(!clusteringMethodChecked);
        clusteringMethod = ClusteringMethod::GROUP_AVERAGE_LINKAGE;
        clusteringMethodChecked = true;
    }
    assert(clusteringMethodChecked);

    ClusterInfo info;
    info.info = currentMeasure;
    info.from = xRange.first;
    info.to = xRange.second;

    auto similarityMatrix = createSimilarityMatrix(currentMeasure, xRange);

    clu::CSimilarityMatrix matrix;
    matrix.elements = similarityMatrix.elements;

    for (auto label : similarityMatrix.labels) {
        matrix.labels.push_back(label.toStdString());
    }

    Clustering clustering(matrix, clusteringMethod);
    ClusteringResult result = clustering.executeHAC();

    bool uniform = (ui->uniformScaleCheck->checkState() == Qt::Checked);

    ClusteringStateData clusteringState;

    clusteringState.matrix = createSimilarityMatrix(currentMeasure, xRange);
    clusteringState.matrix.name = currentMeasure.title;
    clusteringState.info = info;
    clusteringState.result = result;

    clusteringsData.append(clusteringState);

    HistoryListItem *item = new HistoryListItem(clusteringState, 320, 100);
    ui->historyList->insertItem(0, item);

    if (uniform) {
        item->draw(Clustering::createUniformClustering(result));
    } else {
        item->draw(result);
    }

    stateActive = true;
    currentClusteringState = clusteringState;
    updateVisualisation(clusteringState);

    for(int i = 0; i < ui->historyList->count(); i++) {
        auto other = dynamic_cast<HistoryListItem*>(ui->historyList->item(i));
        if (other != item) {
            other->setActivated(false);
        } else {
            other->setActivated(true);
        }
        other->redraw();
    }
}

void MainWindow::setAOI()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Set AOI"), "/home/maurice/Downloads", tr("XML(*.xml)"));
    QFileInfo info(filePath);
    loadAOIData(info.absoluteFilePath().toStdString());
}
void MainWindow::setStimulus()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Set Stimulus"), "/home/maurice/Downloads", tr("Videos(*.mp4 *.avi)"));
    QFileInfo info(filePath);
    loadStimulus(info.absoluteFilePath().toStdString());
}

ParticipantData MainWindow::createDataOfParticipant(const QFileInfo &tsvFile, const QString &label)
{
    if (!stimuliSet) {
        throw std::invalid_argument("Please load first : AOI+Stimulus - XML File");
    }
    ParticipantData data;
    TobiiImporter importer;

    if (!importer.read(tsvFile.absoluteFilePath().toStdString(), data.exportData, false)) {
        throw std::invalid_argument("Error - While parsing " + tsvFile.baseName().toStdString() + " - Please check your file !");
    }

    SlitscanGenerator slitscanGenerator;
    slitscanGenerator.createVisualization(stimuliSource, data.exportData);

    cv::Mat mat = slitscanGenerator.getVC123();
    QImage image = QImage((unsigned char*) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA8888).rgbSwapped();

    mat = slitscanGenerator.getVC1();
    QImage imgSpectrogram = QImage((unsigned char*) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA8888).rgbSwapped();

    data.color = COLORS[participants.size() % COLORS.size()];
    data.id = label;

    data.insertIndex = participants.size();
    //data.aoiStringWTB = fixationToSequenceDynamic(data.exportData, dynamicAOIs, videoInfo.fpms, false);
    data.aoiStringWTB = gazeSequenceToString(data.exportData, dynamicAOIs, videoInfo.fpms);

    data.matVC123 = slitscanGenerator.getVC123();
    data.matSlitscan = slitscanGenerator.getVC123();
    data.imgSlitscan = QPixmap::fromImage(image.scaled(image.width(), slitscanHeight, Qt::IgnoreAspectRatio));
    data.imgSpectrogram = imgSpectrogram;

    return data;
}

SimilarityMatrix MainWindow::createSimilarityMatrix(MeasureInformation measure, QPair<int, int> xRange)
{
    SimilarityMatrix similarityMatrix;
    std::vector<TimeSpanData> spansFromParticipants;

    for(int i = 0; i < checkedParticipants.size(); ++i) {
        ParticipantData &data = checkedParticipants[i];

        auto begin = data.exportData.begin() + xRange.first;
        auto end = data.exportData.begin() + xRange.second;
        auto exportData = std::vector<TobiiExportData>(begin, end);

        TimeSpanData timeSpanData;
        timeSpanData.aoiStringWTB = data.aoiStringWTB.substr(xRange.first, xRange.second - xRange.first);
        timeSpanData.aoiString = mergeByFixationIndex(timeSpanData.aoiStringWTB, exportData);
        timeSpanData.trajectoryOfGazes = createTrajectoryFromGazeData(exportData);
        timeSpanData.trajectoryOfFixations = createTrajectoryFromFixationData(exportData);
        timeSpanData.slitscan = data.matSlitscan(cv::Rect(xRange.first, 0, xRange.second-xRange.first, data.matSlitscan.rows)).clone();

        spansFromParticipants.push_back(timeSpanData);

        std::cout << data.id.toStdString() << " binning : " << timeSpanData.aoiStringWTB << std::endl;
        std::cout << data.id.toStdString() << " merged : " << timeSpanData.aoiString << std::endl;
    }

    for(int i = 0; i < checkedParticipants.size(); ++i) {
        std::vector<double> column;
        column.resize(checkedParticipants.size());
        similarityMatrix.elements.push_back(column);
    }

    for(int i = 0; i < checkedParticipants.size(); ++i) {
        similarityMatrix.labels.append(checkedParticipants[i].id);
        for(int j = i+1; j < checkedParticipants.size(); ++j) {
            double score = calculateSimilarityScore(spansFromParticipants[i], spansFromParticipants[j], measure);
            similarityMatrix.elements[i][j] = score;
            similarityMatrix.elements[j][i] = score;
        }
    }

    createPositiveMatrix(similarityMatrix.elements);

    // Invert values, if necessary
    if (measure.invertValues) {
        invertMatrix(similarityMatrix.elements);
    }

    normalizeMatrix(similarityMatrix.elements);

    similarityMatrix.info = measure;
    return similarityMatrix;
}

double MainWindow::calculateSimilarityScore(const TimeSpanData &p1Data, const TimeSpanData &p2Data, MeasureInformation measure)
{
    switch (measure.measure) {
    case SimilarityMeasure::DTW:
    {
        assert(measure.properties.find("REPRESENTATION") != measure.properties.end());
        auto propRepresentation = measure.properties["REPRESENTATION"];

        if (propRepresentation == "FIXATIONS") {
            return distanceDTW(p1Data.trajectoryOfFixations, p2Data.trajectoryOfFixations, false);
        } else if (propRepresentation == "GAZE-POINTS") {
            return distanceDTW(p1Data.trajectoryOfGazes, p2Data.trajectoryOfGazes, false);
        } else {
            assert(false);
        }
    }
        break;
    case SimilarityMeasure::FRECHET:
    {
        assert(measure.properties.find("REPRESENTATION") != measure.properties.end());
        auto propRepresentation = measure.properties["REPRESENTATION"];

        if (propRepresentation == "FIXATIONS") {
            return discreteFrechetDistance(p1Data.trajectoryOfFixations, p2Data.trajectoryOfFixations);
        } else if (propRepresentation == "GAZE-POINTS") {
            return discreteFrechetDistance(p1Data.trajectoryOfGazes, p2Data.trajectoryOfGazes);
        } else {
            assert(false);
        }
    }
        break;
    case SimilarityMeasure::LEVENSHTEIN:
    {
        assert(measure.properties.find("TEMPORAL-BINNING") != measure.properties.end());
        auto propTemporalBinning = measure.properties["TEMPORAL-BINNING"];

        if (propTemporalBinning == "YES") {
            return levenshteinDistance(p1Data.aoiStringWTB, p2Data.aoiStringWTB);
        } else if (propTemporalBinning == "NO"){
            return levenshteinDistance(p1Data.aoiString, p2Data.aoiString);
        } else {
            assert(false);
        }
    }
        break;
    case SimilarityMeasure::NEEDLEMAN_WUNSCH:
    {
        assert(measure.properties.find("TEMPORAL-BINNING") != measure.properties.end());
        auto propTemporalBinning = measure.properties["TEMPORAL-BINNING"];

        assert(measure.properties.find("SCORING") != measure.properties.end());
        auto propSubstMt = measure.properties["SCORING"];

        std::function<int(char, char)> subWeights;

        if (propSubstMt.left(8) == "STANDARD") {
            std::cout << "Use Standard Substituiton Matrix" << std::endl;
            subWeights = [](char a, char b) {
                if(a == b)
                    return 1;
                return -1;
            };
        } else if (propSubstMt.left(9) == "BENCHMARK") {
            int benchmarkID = propSubstMt.right(2).toInt();
            assert(benchmarkID >= 1 && benchmarkID <= 11);
            std::cout << "Use Substituiton Matrix of benchmark data : " << benchmarkID << std::endl;
            subWeights = SUBSTITUTION_MATRICES[benchmarkID-1];
        } else {
            assert(false);
        }

        if (propTemporalBinning == "YES") {
            return needlemanWunschScore(p1Data.aoiStringWTB, p2Data.aoiStringWTB, subWeights, -1);
        } else if (propTemporalBinning == "NO"){
            return needlemanWunschScore(p1Data.aoiString, p2Data.aoiString, subWeights, -1);
        } else {
            assert(false);
        }
    }
        break;
    case SimilarityMeasure::HISTOGRAM:
    {
        assert(measure.properties.find("COLOR-SPACE") != measure.properties.end());
        assert(measure.properties.find("METRIC") != measure.properties.end());

        auto propMetric = measure.properties["METRIC"];
        auto propColorSpace = measure.properties["COLOR-SPACE"];

        HistogramMetric metric;
        ColorSpace colorSpace;

        if (propMetric == "BHATTACHARYYA") {
            metric = HistogramMetric::BHATTACHARYYA;
        } else if (propMetric == "INTERSECTION") {
            metric = HistogramMetric::INTERSECTION;
        } else if (propMetric == "CORRELATION") {
            metric = HistogramMetric::CORRELATION;
        } else if (propMetric == "CHI_SQUARE") {
            metric = HistogramMetric::CHI_SQUARE;
        } else if (propMetric == "EMD") {
            metric = HistogramMetric::EMD;
        } else {
            assert(false);
        }

        if (propColorSpace == "RGB") {
            colorSpace = ColorSpace::RGB;
        } else if (propColorSpace == "HSV") {
            colorSpace = ColorSpace::HSV;
        } else if (propColorSpace == "LAB") {
            colorSpace = ColorSpace::CIE_LAB;
        } else {
            assert(false);
        }

        return histogramScore(p1Data.slitscan, p2Data.slitscan, metric, colorSpace);
    }
        break;
    case SimilarityMeasure::IMAGE2D:
    {
        assert(measure.properties.find("METRIC") != measure.properties.end());
        auto propMetric = measure.properties["METRIC"];

        ImageMetric metric;

        if (propMetric == "SQUARED_DIFFERENCES") {
            metric = ImageMetric::SQUARED_DIFFERENCES;
        } else if (propMetric == "COSINE_COEFFICIENT") {
            metric = ImageMetric::COSINE_COEFFICIENT;
        } else if (propMetric == "CROSS_CORRELATION") {
            metric = ImageMetric::CROSS_CORRELATION;
        } else {
            assert(false);
        }
        return imageScore(p1Data.slitscan, p2Data.slitscan, metric);
    }
        break;
    default:
        assert(false);
        break;
    }
}

void MainWindow::addMeasure(const MeasureInformation &info)
{
    QTreeWidgetItem *mItem = new MeasureTreeItem(info, true);
    measureItems.append(mItem);
    ui->measuresTree->addTopLevelItems(measureItems);
    ui->clusteringCombo->addItem(info.title);
    addedMeasures.push_back(info);

    // We always set the added measure as the new clustering option for convienience
    activeMeasureChanged(measureItems.size() - 1);

    QTreeWidgetItem *item = new MeasureTreeItem(info, false, false);
    item->setCheckState(0, Qt::Checked);
    ui->clusteringCombo->setCurrentIndex(measureItems.size() - 1);
}


QString MainWindow::getMeasureTitle(SimilarityMeasure measure)
{
    int counter = 0;
    for (const auto &item : addedMeasures) {
        counter += (item.measure == measure) ? 1 : 0;
    }

    QString suffix = counter > 0 ? " (" + QString('0' + counter) + ")" : QString("");

    switch (measure) {
    case SimilarityMeasure::DTW:
        return QString("Dynamic Time Warping") + suffix;
    case SimilarityMeasure::EUCLIDEAN:
        return QString("Euclidean Distance") + suffix;
    case SimilarityMeasure::FRECHET:
        return QString("Frechet Distance") + suffix;
    case SimilarityMeasure::LEVENSHTEIN:
        return QString("Levenshtein") + suffix;
    case SimilarityMeasure::NEEDLEMAN_WUNSCH:
        return QString("Needleman-Wunsch") + suffix;
    case SimilarityMeasure::HISTOGRAM:
        return QString("Histogram") + suffix;
    case SimilarityMeasure::IMAGE2D:
        return QString("Image 2d") + suffix;
    case SimilarityMeasure::MULTIMATCH:
        return QString("MultiMatch") + suffix;
    default:
        assert(false);
        break;
    }
    return QString("");
}

bool MainWindow::chooseColorSpaceDialog(MeasureInformation &info)
{
    QStringList items;
    items << tr("HSV") << tr("LAB") << tr("RGB");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Choose Color Space"),
                         tr("Color Space :"), items, 0, false, &ok);
    if (ok && !item.isEmpty()) {
        info.properties["COLOR-SPACE"] = item;
        return true;
    }
    return false;
}

bool MainWindow::chooseTemporalBinningDialog(MeasureInformation &info)
{
    auto answer = QMessageBox::question(this, "String-Edit Options", "Use Temporal Binning");

    if (answer == QMessageBox::Yes) {
        bool ok;
        /*
        int binSize = QInputDialog::getInt(this, tr("Choose Temporal Binning Size"),
                     tr("Milliseconds"), 50, 5, 100, 1, &ok);
        */
        info.properties["TEMPORAL-BINNING"] = "YES";
        //info.properties["BIN-SIZE"] = QString::number(binSize);
        return true;
    } else if (answer == QMessageBox::No){
        info.properties["TEMPORAL-BINNING"] = "NO";
        return true;
    }
    return false;
}

bool MainWindow::chooseTrajectoryRepresentation(MeasureInformation &info)
{
    QStringList items;
    items << tr("GAZE-POINTS")
          << tr("FIXATIONS");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Choose trajectory representation"),
                         tr("Trajectory Representation"), items, 0, false, &ok);
    if (ok && !item.isEmpty()) {
        info.properties["REPRESENTATION"] = item;
        return true;
    }
    return false;
}

bool MainWindow::chooseSubstitutionMatrix(MeasureInformation &info)
{
    QStringList items;
    items << tr("STANDARD")
          << tr("BENCHMARK-01")
          << tr("BENCHMARK-02")
          << tr("BENCHMARK-03")
          << tr("BENCHMARK-04")
          << tr("BENCHMARK-05")
          << tr("BENCHMARK-06")
          << tr("BENCHMARK-07")
          << tr("BENCHMARK-08")
          << tr("BENCHMARK-09")
          << tr("BENCHMARK-010")
          << tr("BENCHMARK-011");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Choose substitution matrix"),
                         tr("Substitution Matrix"), items, 0, false, &ok);
    if (ok && !item.isEmpty()) {
        info.properties["SCORING"] = item;
        return true;
    }
    return false;
}

void MainWindow::enableVideoLabels(bool enable)
{
    videoItem->setShowLabelsEnabled(enable);
}

void MainWindow::enableUniformScaling(bool enable)
{
    for(int i = 0; i < ui->historyList->count(); ++i) {
        HistoryListItem *castedItem = dynamic_cast<HistoryListItem*>(ui->historyList->item(i));
        auto result = castedItem->getClusteringState().result;
        if (enable) {
            castedItem->draw(Clustering::createUniformClustering(result));
        } else {
            castedItem->draw(result);
        }
    }
}

void MainWindow::enableUniformScalingDendrogram(bool enable)
{
    uniformScaling = enable;
}


void MainWindow::exportSlitscans()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose directory"),
                                                "/home",
                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        QPair<int, int> xRange = getHandleSpanXRange();
        //bool full = ui->modeSelector->currentIndex() == 1;
        bool full = false;


        for(const auto &item : checkedParticipants) {
            auto matSlitscan = full ? item.matVC123.clone() : item.matVC123(cv::Rect(xRange.first,0,xRange.second-xRange.first, item.matVC123.rows)).clone();
            auto imgSlitscan = QImage((unsigned char*) matSlitscan.data, matSlitscan.cols, matSlitscan.rows, matSlitscan.step, QImage::Format_RGBA8888).rgbSwapped();

            imgSlitscan = imgSlitscan.convertToFormat(QImage::Format_RGB888);
            imgSlitscan = imgSlitscan.scaled(imgSlitscan.width(), 400, Qt::IgnoreAspectRatio);
            auto filename = item.id + (full ? "-full" : "[" + QString::number(xRange.first) + ":" + QString::number(xRange.second) + "]") + ".png";
            imgSlitscan.save(dir + "/" + filename);
        }
    }
}

void MainWindow::exportCSVOfClusterData()
{
    QString filename = QFileDialog::getSaveFileName(this,
            tr("Export CSV-File from Cluster Data"), "",
            tr("Comma-Seperated-Values (*.csv);;All Files (*)"));

    if (!filename.isEmpty()) {
        csvExport.writeDistanceMatricesToCSV(filename.toStdString());
    }
}

void MainWindow::exportCopheneticCorrelationData()
{
    QString filename = QFileDialog::getSaveFileName(this,
            tr("Export CSV-File from cophentic correlation values between HACs in history."), "",
            tr("Comma-Seperated-Values (*.csv);;All Files (*)"));

    // ATTENTION : WE DO NOT CHECK IF THOSE STATES ARE COMPATIBLE - SO TAKE CARE WHEN EXPORTING !!
    if (!filename.isEmpty()) {
        bool uniform = (ui->uniformScaleCheck->checkState() == Qt::Checked);
        csvExport.writeCophenticCorrelationToFile(uniform, filename.toStdString());
    }
}

void MainWindow::takeScreenshot()
{
    Screenshot *screenshot = new Screenshot;
    screenshot->move(QApplication::desktop()->availableGeometry(screenshot).topLeft() + QPoint(20, 20));
    screenshot->show();
}

void MainWindow::addLevenshtein()
{
    MeasureInformation info;
    info.invertValues = false;
    info.measure = SimilarityMeasure::LEVENSHTEIN;
    info.title = getMeasureTitle(info.measure);

    if (chooseTemporalBinningDialog(info)) {
        addMeasure(info);
    }
}

void MainWindow::addNeedlemanWunsch()
{
    MeasureInformation info;
    info.invertValues = true;
    info.measure = SimilarityMeasure::NEEDLEMAN_WUNSCH;
    info.title = getMeasureTitle(info.measure);

    if (chooseTemporalBinningDialog(info) && chooseSubstitutionMatrix(info)) {
        addMeasure(info);
    }
}

void MainWindow::addDTW()
{
    MeasureInformation info;
    info.invertValues = false;
    info.measure = SimilarityMeasure::DTW;
    info.title = getMeasureTitle(info.measure);
    if (chooseTrajectoryRepresentation(info)) {
        addMeasure(info);
    }
}

void MainWindow::addFrechet()
{
    MeasureInformation info;
    info.invertValues = false;
    info.measure = SimilarityMeasure::FRECHET;
    info.title = getMeasureTitle(info.measure);
    if (chooseTrajectoryRepresentation(info)) {
        addMeasure(info);
    }
}

void MainWindow::addEUCL()
{
    MeasureInformation info;
    info.invertValues = false;
    info.measure = SimilarityMeasure::EUCLIDEAN;
    info.title = getMeasureTitle(info.measure);
    addMeasure(info);
    /*
    if (chooseTrajectoryNormalization(info)) {
        addMeasure(info);
    }
    */
}

void MainWindow::addHistogramBhattacharyya()
{
    MeasureInformation info;
    info.invertValues = false;
    info.measure = SimilarityMeasure::HISTOGRAM;
    info.title = getMeasureTitle(info.measure);
    info.properties["METRIC"] = "BHATTACHARYYA";
    if (chooseColorSpaceDialog(info)) {
        addMeasure(info);
    }

}

void MainWindow::addHistogramCorrelation()
{
    MeasureInformation info;
    info.invertValues = true;
    info.measure = SimilarityMeasure::HISTOGRAM;
    info.title = getMeasureTitle(info.measure);
    info.properties["METRIC"] = "CORRELATION";
    if (chooseColorSpaceDialog(info)) {
        addMeasure(info);
    }
}

void MainWindow::addHistogramIntersection()
{
    MeasureInformation info;
    info.invertValues = true;
    info.measure = SimilarityMeasure::HISTOGRAM;
    info.title = getMeasureTitle(info.measure);
    info.properties["METRIC"] = "INTERSECTION";
    if (chooseColorSpaceDialog(info)) {
        addMeasure(info);
    }
}


void MainWindow::addHistogramChiSquare()
{
    MeasureInformation info;
    info.invertValues = false;
    info.measure = SimilarityMeasure::HISTOGRAM;
    info.title = getMeasureTitle(info.measure);
    info.properties["METRIC"] = "CHI_SQUARE";
    if (chooseColorSpaceDialog(info)) {
        addMeasure(info);
    }
}

void MainWindow::addHistogramEarthMoverDistance()
{
    MeasureInformation info;
    info.invertValues = false;
    info.measure = SimilarityMeasure::HISTOGRAM;
    info.title = getMeasureTitle(info.measure);
    info.properties["METRIC"] = "EMD";
    if (chooseColorSpaceDialog(info)) {
        addMeasure(info);
    }

}

void MainWindow::addImageCosineCoefficient()
{
    MeasureInformation info;
    info.invertValues = true;
    info.measure = SimilarityMeasure::IMAGE2D;
    info.title = getMeasureTitle(info.measure);
    info.properties["METRIC"] = "COSINE_COEFFICIENT";
    addMeasure(info);
}

void MainWindow::addImageCrossCorrelation()
{
    MeasureInformation info;
    info.invertValues = true;
    info.measure = SimilarityMeasure::IMAGE2D;
    info.title = getMeasureTitle(info.measure);
    info.properties["METRIC"] = "CROSS_CORRELATION";
    addMeasure(info);
}

void MainWindow::addImageSquaredDifferences()
{
    MeasureInformation info;
    info.invertValues = false;
    info.measure = SimilarityMeasure::IMAGE2D;
    info.title = getMeasureTitle(info.measure);
    info.properties["METRIC"] = "SQUARED_DIFFERENCES";
    addMeasure(info);
}

void MainWindow::activeMeasureChanged(int index)
{
    for(int i = 0; i < measureItems.size(); i++) {
        MeasureTreeItem *castedItem = dynamic_cast<MeasureTreeItem*>(measureItems[i]);
        if (i == index) {
            castedItem->showClusteringIcon(true);
        } else {
            castedItem->showClusteringIcon(false);
        }
    }
    currentMeasure = addedMeasures[index];
}

MainWindow::~MainWindow()
{
    delete ui;
}
