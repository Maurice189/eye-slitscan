#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QAction>
#include <QActionGroup>
#include <QGraphicsScene>
#include <QRectF>
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QString>
#include <QtGlobal>
#include <QFileDialog>
#include <QPixmap>
#include <QImage>
#include <QThread>
#include <QMenu>
#include <QGraphicsLinearLayout>

#include "slitscanitem.h"
#include "frameslideritem.h"
#include "measuretreeitem.h"
#include "dendrogramitem.h"
#include "timespanselector.h"
#include "historylistitem.h"
#include "videooverlaywidget.h"
#include "slitscanvideo.h"
#include "csvexport.h"
#include "constants.h"
#include "representationview.h"
#include "participant.h"
#include "participantmanager.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void addSlitScan(QPixmap slitscan, QString label, int positionIndex);
    void setVideoSource(const std::string &videoSourcePath);

public slots:


    void asyncParticipantsLoadedFinished();
    void dendrogramSubtreeActivated(QStringList leafIds);

    void matrixOptionChanged();
    void baseColorChanged(const QString &baseColorText);
    void updateBaseColorScheme(int i);

    void selectAllArea();
    void selectAllViewers();
    void deselectAllViewers();
    void showAboutDialog();
    void showLicenseDialog();
    void slitscanModeSelection(int mode);
    void clearHistoryList();
    void selectVisibleArea();
    void keyReleaseEvent(QKeyEvent*);

    void historyListSelectionChanged(HistoryListItem *item);
    void historyListDeselected(HistoryListItem * item);

    void addParticipant();
    void setAOI();
    void setStimulus();
    void executeClustering();
    void updateVisualisation(const ClusteringStateData &state);
    void handlesMoved(QPointF newPos);

    void addLevenshtein();
    void addNeedlemanWunsch();

    void addDTW();
    void addEUCL();
    void addFrechet();

    void addHistogramBhattacharyya();
    void addHistogramCorrelation();
    void addHistogramIntersection();
    void addHistogramChiSquare();
    void addHistogramEarthMoverDistance();

    void addImageCosineCoefficient();
    void addImageCrossCorrelation();
    void addImageSquaredDifferences();

    void activeMeasureChanged(int index);
    void videoPositionChanged(qint64 duration);

    void boxInMatrixSelected(QPair<QString,QString> ids, QString method, QString value);
    void playPauseToggled(bool);
    void zoomingFactorChanged(int value);
    void setBeeSwarmEnabled(bool enabled);

    void participantsTreeItemChanged(QTreeWidgetItem *item, int column);

    void takeScreenshot();
    void enableUniformScaling(bool enable);
    void exportCSVOfClusterData();
    void exportSlitscans();
    void exportCopheneticCorrelationData();
    void enableUniformScalingDendrogram(bool enable);

    void enableVideoLabels(bool enable);

private:
    ParticipantData createDataOfParticipant(const QFileInfo &tsvFile, const QString &label);
    double calculateSimilarityScore(const TimeSpanData &p1Data, const TimeSpanData &p2Data, MeasureInformation measure);
    bool loadAOIData(const std::string aoiAbsPath);
    bool loadStimulus(const std::string stimulusPath);
    SimilarityMatrix createSimilarityMatrix(MeasureInformation measure, QPair<int, int> xRange);
    void preloadDefault();
    QString measureEnumToString(SimilarityMeasure measure);
    void addMeasure(const MeasureInformation &info);
    bool chooseColorSpaceDialog(MeasureInformation &info);
    bool chooseTrajectoryRepresentation(MeasureInformation &info);
    bool chooseTemporalBinningDialog(MeasureInformation &info);
    bool chooseSubstitutionMatrix(MeasureInformation &info);
    QPair<int, int> getHandleSpanXRange();

    int numberOfActiveParticipants();
    bool correlationCompatibleClusteringStates(const ClusteringStateData &d1, const ClusteringStateData &d2);
    bool equalClusteringStates(const ClusteringStateData &d1, const ClusteringStateData &d2);

    void createPositiveSimilarityMatrix(SimilarityMatrix &matrix, bool invertedValues);

    void createSlitScanMiniMap();

    QString getMeasureTitle(SimilarityMeasure measure);
    void filterStates();
private:
    QThread workerThread;
    ParticipantManager *pManager;

    Ui::MainWindow *ui;
    QMenu *contextMenu;

    QGraphicsScene *videoScene;
    QGraphicsScene scene;
    QGraphicsScene dendrogramScene;
    QGraphicsScene timespanScene;
    QGraphicsScene miniMapScene;

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QVideoWidget *videoWidget;
    QTreeWidgetItem *participantRoot;

    QGraphicsLinearLayout *slitScanItemLayout;
    QGraphicsWidget *slitScanItemGroup;


    TimeSpanSelector *selector;
    SlitscanVideo *videoItem;
    FrameSliderItem *slider;
    DendrogramItem *ditem;

    std::vector<SlitscanItem*> slitScanItems;
    std::vector<struct DynamicAOI> dynamicAOIs;

    std::vector<Participant> participants;
    std::vector<Participant> checkedParticipants;

    QList<ClusteringStateData> clusteringsData;
    QList<QTreeWidgetItem *> measureItems;
    QList<QTreeWidgetItem *> pariticpantItems;
    QList<MeasureInformation> addedMeasures;

    RepresentationViewerObject* rvoP1;
    RepresentationViewerObject* rvoP2;

    int slitscanHeight = 130;
    int slitscanWidth = 200;
    int nofParticipants = 0;
    int zoomFactor = 0;

    bool uniformScaling = true;
    bool showZoomingModeHintMsg = true;
    bool durationSet = false;
    bool stimuliSet = false;
    bool stateActive = false;

    struct GenericSourceInfo sourceInfo;
    VideoInfo videoInfo;
    CSVExport csvExport;
    MeasureInformation currentMeasure;
    ClusteringStateData currentClusteringState;
    std::string stimuliSource;

    QActionGroup *clusteringMethodGroup;

};

#endif // MAINWINDOW_H
