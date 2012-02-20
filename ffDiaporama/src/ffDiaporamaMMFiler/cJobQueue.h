/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2012 Dominique Levray <levray.dominique@bbox.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
   ====================================================================== */

#ifndef CJOBQUEUE_H
#define CJOBQUEUE_H

#include <QDomElement>
#include <QDomDocument>
#include <QString>
#include <QListWidget>
#include "../sharedfiles/cBaseApplicationConfig.h"
#include "../sharedfiles/cBaseMediaFile.h"

#define JOBQUEUESRC                         "JobProcessing"         // Name use as source for log message

#define JOBQUALIF_SOURCE_KEEP               0x0001
#define JOBQUALIF_SOURCE_REMOVE             0x0002
#define JOBQUALIF_SOURCE_ADDSUFFIX          0x0004
#define JOBQUALIF_SOURCE_MOVE               0x0008
#define JOBQUALIF_SOURCE_ALL                (JOBQUALIF_SOURCE_KEEP|JOBQUALIF_SOURCE_REMOVE|JOBQUALIF_SOURCE_ADDSUFFIX|JOBQUALIF_SOURCE_MOVE)

#define JOBQUALIF_DESTNAME_ADDSUFFIX        0x0010
#define JOBQUALIF_DESTNAME_ADDSIZESUFFIX    0x0020
#define JOBQUALIF_DESTPLACE_INFOLDER        0x0040

#define JOBQUALIF_DESTINATION_OVERWRITE     0x0100
#define JOBQUALIF_DESTINATION_COPYTHUMB     0x0200

#define JOBTYPE_NOTDEFINED                  0
#define JOBTYPE_OPENFILE                    1
#define JOBTYPE_DISPLAYINFO                 2
#define JOBTYPE_REMOVEFILE                  3

#define JOBTYPE_IMAGE_CONVERTIMAGE          4
#define JOBQUALIF_IMAGE_CONVERTIMAGE        (/*JOBQUALIF_SOURCE_KEEP|JOBQUALIF_SOURCE_REMOVE|JOBQUALIF_SOURCE_ADDSUFFIX|JOBQUALIF_SOURCE_MOVE|*/JOBQUALIF_DESTNAME_ADDSUFFIX|JOBQUALIF_DESTNAME_ADDSIZESUFFIX|JOBQUALIF_DESTPLACE_INFOLDER|JOBQUALIF_DESTINATION_OVERWRITE/*|JOBQUALIF_DESTINATION_COPYTHUMB*/)
#define JOBDEFAULT_IMAGE_CONVERTIMAGE       (JOBQUALIF_SOURCE_KEEP|JOBQUALIF_DESTNAME_ADDSIZESUFFIX)

#define JOBTYPE_AUDIO_CONVERTAUDIO          5
#define JOBQUALIF_AUDIO_CONVERTAUDIO        (/*JOBQUALIF_SOURCE_KEEP|JOBQUALIF_SOURCE_REMOVE|JOBQUALIF_SOURCE_ADDSUFFIX|JOBQUALIF_SOURCE_MOVE|*/JOBQUALIF_DESTNAME_ADDSUFFIX|JOBQUALIF_DESTPLACE_INFOLDER|JOBQUALIF_DESTINATION_OVERWRITE/*|JOBQUALIF_DESTINATION_COPYTHUMB*/)
#define JOBDEFAULT_AUDIO_CONVERTAUDIO       (JOBQUALIF_SOURCE_KEEP)

#define JOBTYPE_VIDEO_CONVERTVIDEO          6
#define JOBQUALIF_VIDEO_CONVERTVIDEO        (/*JOBQUALIF_SOURCE_KEEP|JOBQUALIF_SOURCE_REMOVE|JOBQUALIF_SOURCE_ADDSUFFIX|JOBQUALIF_SOURCE_MOVE|*/JOBQUALIF_DESTNAME_ADDSUFFIX|JOBQUALIF_DESTNAME_ADDSIZESUFFIX|JOBQUALIF_DESTPLACE_INFOLDER|JOBQUALIF_DESTINATION_OVERWRITE/*|JOBQUALIF_DESTINATION_COPYTHUMB*/)
#define JOBDEFAULT_VIDEO_CONVERTVIDEO       (JOBQUALIF_SOURCE_KEEP)

#define NBR_JOBTYPE                         7

extern  uint32_t    PossibleJobsSettings[NBR_JOBTYPE];

#define JOBSTATUS_READYTOSTART              0
#define JOBSTATUS_STARTED                   1
#define JOBSTATUS_PAUSED                    2
#define JOBSTATUS_CANCELED                  3
#define JOBSTATUS_SUCCEEDED                 4
#define JOBSTATUS_ENDEDWITHERROR            5

#define EVENT_JobStatusChanged              100
#define EVENT_FileListChanged               101
#define EVENT_FolderChanged                 102

class cJob {
public:
    // Job qualification & settings
    uint32_t    JobQualif;
    uint32_t    JobSettings;
    QString     SourceSuffix;
    QString     SourceFolder;
    QString     DestinationSuffix;
    QString     DestinationFolder;
    QString     DestinationExtension;

    // Job controls
    QDateTime   DateTime;
    int         JobType;
    int         JobStatus;
    int         PercentDone;
    QStringList SourcesAndDests;
    int         Succeded;
    int         CurrentIndex;
    QString     CurrentAction;

    // Job specific
    QString     Command;

    cJob();
    ~cJob();

    virtual void        SaveToXML(QDomElement &domDocument,QString ElementName);
    virtual bool        LoadFromXML(QDomElement domDocument,QString ElementName);

    virtual QString     ComputeDestinationName(QString Source,QString SizeSuffix);
    virtual QString     ComputeNewSourceName(QString Source);

    virtual bool        IsCommandListContain(QString ToFind);
    virtual int         CommandListValue(QString ToFind);
    virtual QString     CommandListValueString(QString ToFind);
};

//***************************

class cJobQueue {
public:
    cBaseApplicationConfig  *BaseApplicationConfig;
    QList<cJob*>            List;                       // list of cJobObject
    QStringList             StatusText;
    QStringList             JobTypeText;
    QList<cBaseMediaFile*>  MediaList;

    cJobQueue();
    ~cJobQueue();

    virtual void        SaveToXML(QDomElement &domDocument,QString ElementName);
    virtual bool        LoadFromXML(QDomElement domDocument,QString ElementName);

    // Utility
    virtual void        PostJobStatusChanged(cJob *Job,QString Action,int Percent);
    virtual QString     ComputeFFMPEGCommand(cJob *Job);

    // Jobs
    virtual bool        ApplyDestinationOverWriting(QString Destination,uint32_t JobSettings);
    virtual bool        ApplySourceTransformation(QString Source,QString NewSource);
    virtual void        ConvertIMG(cJob *Job);
    virtual void        ProcessFFMPEGJob(cJob *Job);

};

#endif // CJOBQUEUE_H
