#ifndef PARTICIPANTMANAGER_H
#define PARTICIPANTMANAGER_H

#include <QObject>
#include <QStringList>
#include <QFileInfo>
#include <vector>

#include "participant.h"
#include "constants.h"

class ParticipantManager : public QObject
{
    Q_OBJECT
public:
    ParticipantManager(){}

    void setDynamicAOIs(std::vector<struct DynamicAOI> &dynamicAOIs)
    {
        this->dynamicAOIs = dynamicAOIs;
    }

    void setVideoInfo(VideoInfo videoInfo)
    {
        this->videoInfo = videoInfo;
    }

    void setStimuliSource(std::string stimuliSource)
    {
        this->stimuliSource = stimuliSource;
    }

    void setSlitscanHeight(int slitscanHeight)
    {
        this->slitscanHeight = slitscanHeight;
    }

    void setFilePathList(const QStringList &filePathList)
    {
        this->filePathList = filePathList;
    }
    std::vector<Participant> getParticipants()
    {
        return participants;
    }
public slots:
    void load()
    {
        for(auto filePath : filePathList) {
            QFileInfo info(filePath);
            QString label = info.baseName().split("-").first();

            if (label.length() > 5) {
                label = label.left(5);
            }
            Participant data = Participant(dynamicAOIs, videoInfo, stimuliSource);
            data.load(info.absoluteFilePath().toStdString(), label.toStdString(), participants.size(), slitscanHeight);
            participants.push_back(data);
        }
        emit finished();
    }
signals:
    void finished();
private:
    int slitscanHeight;
    std::vector<Participant> participants;
    std::vector<struct DynamicAOI> dynamicAOIs;
    VideoInfo videoInfo;
    std::string stimuliSource;

    QStringList filePathList;
};

#endif // PARTICIPANTMANAGER_H
