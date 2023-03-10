/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2014 Dominique Levray <domledom@laposte.net>

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

#include "wgt_QVideoPlayer.h"
#include "MainWindow/mainwindow.h"
#include "ui_wgt_QVideoPlayer.h"

#define ICON_PLAYERPLAY                     ":/img/player_play.png"                 // FileName of play icon
#define ICON_PLAYERPAUSE                    ":/img/player_pause.png"                // FileName of pause icon

#define BUFFERING_NBR_FRAME                 10                                      // Number of frame wanted in the playing buffer

//*********************************************************************************************************************************************
// Base object for Movie frame
//*********************************************************************************************************************************************
QMovieLabel::QMovieLabel(QWidget *parent):QLabel(parent) {
}

QMovieLabel::~QMovieLabel() {
}

void QMovieLabel::mouseDoubleClickEvent(QMouseEvent *) {
    emit DoubleClick();
}

void QMovieLabel::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button()==Qt::RightButton) emit RightClickEvent(event);  else QLabel::mouseReleaseEvent(event);
}

void QMovieLabel::SetImage(QImage Image) {
    CurrentImage=Image;
    repaint();
}

void QMovieLabel::SetImage(QImage *Image) {
    CurrentImage=Image->copy();
    repaint();
}

void QMovieLabel::resizeEvent(QResizeEvent *Ev) {
    QLabel::resizeEvent(Ev);
    repaint();
}

void QMovieLabel::paintEvent(QPaintEvent *) {
    if (CurrentImage.isNull()) return;
    QPainter Painter(this);
    qreal Ratio=qreal(CurrentImage.width())/qreal(CurrentImage.height());
    int MaxW=this->width();
    int MaxH=qreal(MaxW)/Ratio;
    if (MaxH>this->height()) {
        MaxH=this->height();
        MaxW=qreal(MaxH*Ratio);
    }
    if (CurrentImage.width()!=MaxW) {
        QImage CI=CurrentImage.scaledToWidth(MaxW);
        int LeftBar  =(this->width()-CI.width())/2;
        int RightBar =this->width()-CI.width()-LeftBar;
        int TopBar   =(this->height()-CI.height())/2;
        int BottomBar=this->height()-CI.height()-TopBar;
        Painter.drawImage(LeftBar,TopBar,CI);
        if (LeftBar) {
            Painter.fillRect(QRectF(0,0,LeftBar,this->height()),Qt::black);
            Painter.fillRect(QRectF(this->width()-RightBar,0,RightBar,this->height()),Qt::black);
        }
        if (TopBar) {
            Painter.fillRect(QRectF(LeftBar,0,CI.width(),TopBar),Qt::black);
            Painter.fillRect(QRectF(LeftBar,this->height()-BottomBar,CI.width(),BottomBar),Qt::black);
        }
    } else {
        int LeftBar  =(this->width()-CurrentImage.width())/2;
        int RightBar =this->width()-CurrentImage.width()-LeftBar;
        int TopBar   =(this->height()-CurrentImage.height())/2;
        int BottomBar=this->height()-CurrentImage.height()-TopBar;
        Painter.drawImage(LeftBar,TopBar,CurrentImage);
        if (LeftBar) {
            Painter.fillRect(QRectF(0,0,LeftBar,this->height()),Qt::black);
            Painter.fillRect(QRectF(this->width()-RightBar,0,RightBar,this->height()),Qt::black);
        }
        if (TopBar) {
            Painter.fillRect(QRectF(LeftBar,0,CurrentImage.width(),TopBar),Qt::black);
            Painter.fillRect(QRectF(LeftBar,this->height()-BottomBar,CurrentImage.width(),BottomBar),Qt::black);
        }
    }
}

//*********************************************************************************************************************************************
// Base object for image manipulation
//*********************************************************************************************************************************************

cImageList::cImageList() {
}

//====================================================================================================================

cImageList::~cImageList() {
    ClearList();
}

//====================================================================================================================
// Clear the list (make av_free of each packet)
//====================================================================================================================
void cImageList::ClearList() {
    while (List.count()>0) {
        cDiaporamaObjectInfo *Frame=DetachFirstImage();
        if (Frame) delete(Frame);
    }
}

//====================================================================================================================
// Detach the first image of the list (do not make delete)
//====================================================================================================================
cDiaporamaObjectInfo *cImageList::DetachFirstImage() {
    if (List.count()>0) return (cDiaporamaObjectInfo *)List.takeFirst(); else return NULL;
}

//====================================================================================================================
// Retreve a link to the first frame in the list
//====================================================================================================================
cDiaporamaObjectInfo *cImageList::GetFirstImage() {
    if (List.count()>0) return (cDiaporamaObjectInfo *)List[0]; else return NULL;
}

//====================================================================================================================
// Retreve a link to the last frame in the list
//====================================================================================================================
cDiaporamaObjectInfo *cImageList::GetLastImage() {
    if (List.count()>0) return (cDiaporamaObjectInfo *)List[List.count()-1]; else return NULL;
}

//====================================================================================================================
// Append a packet to the end of the list
//====================================================================================================================
void cImageList::AppendImage(cDiaporamaObjectInfo *Frame) {
    List.append(Frame);
}

//====================================================================================================================

wgt_QVideoPlayer::wgt_QVideoPlayer(QWidget *parent) : QWidget(parent),ui(new Ui::wgt_QVideoPlayer) {
    ui->setupUi(this);
    FLAGSTOPITEMSELECTION   = NULL;
    FileInfo                = NULL;
    Diaporama               = NULL;
    WantedFPS               = 12.5;
    IsValide                = false;
    IsInit                  = false;
    DisplayMSec             = true;                                 // add msec to display
    IconPause               = QIcon(ICON_PLAYERPLAY);               // QApplication::style()->standardIcon(QStyle::SP_MediaPlay)
    IconPlay                = QIcon(ICON_PLAYERPAUSE);              // QApplication::style()->standardIcon(QStyle::SP_MediaPause)
    PlayerPlayMode          = false;                                // Is player currently play mode
    PlayerPauseMode         = false;                                // Is player currently plause mode
    IsSliderProcess         = false;
    ActualPosition          = -1;
    tDuration               = QTime(0,0,0,0);
    ResetPositionWanted     = false;
    Deinterlace             = false;

    this->FileInfo          = FileInfo;

    Music.SetFPS(MixedMusic.WantedDuration,MixedMusic.Channels,MixedMusic.SamplingRate,MixedMusic.SampleFormat);

    ui->CustomRuler->ActiveSlider(0);
    ui->CustomRuler->setSingleStep(25);

    ui->MovieFrame->setText("");
    ui->MovieFrame->setAttribute(Qt::WA_OpaquePaintEvent);

    connect(&Timer,SIGNAL(timeout()),this,SLOT(s_TimerEvent()));
    connect(ui->VideoPlayerPlayPauseBT,SIGNAL(clicked()),this,SLOT(s_VideoPlayerPlayPauseBT()));
    connect(ui->MovieFrame,SIGNAL(DoubleClick()),this,SLOT(s_DoubleClick()));
    connect(ui->MovieFrame,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_RightClickEvent(QMouseEvent *)));

    // Slider control
    connect(ui->CustomRuler,SIGNAL(sliderPressed()),this,SLOT(s_SliderPressed()));
    connect(ui->CustomRuler,SIGNAL(sliderReleased()),this,SLOT(s_SliderReleased()));
    connect(ui->CustomRuler,SIGNAL(valueChanged(int)),this,SLOT(s_SliderMoved(int)));
    connect(ui->CustomRuler,SIGNAL(PositionChangeByUser()),this,SLOT(s_PositionChangeByUser()));
    connect(ui->VideoPlayerSaveImageBT,SIGNAL(pressed()),this,SLOT(s_SaveImage()));
}

//============================================================================================

wgt_QVideoPlayer::~wgt_QVideoPlayer() {
    SetPlayerToPause();         // Ensure player is correctly stoped
    delete ui;
}

//============================================================================================

void wgt_QVideoPlayer::closeEvent(QCloseEvent *) {
    SetPlayerToPause();
}

//====================================================================================================================

void wgt_QVideoPlayer::showEvent(QShowEvent *) {
    if ((!IsInit)&&(Diaporama==NULL)) {
        SetPlayerToPlay();
        IsInit=true;
    }
}

//============================================================================================

void wgt_QVideoPlayer::s_SaveImage() {
    emit SaveImageEvent();
}

//============================================================================================

void wgt_QVideoPlayer::s_DoubleClick() {
    emit DoubleClick();
}

//============================================================================================

void wgt_QVideoPlayer::s_RightClickEvent(QMouseEvent *event) {
    emit RightClickEvent(event);
}

//============================================================================================

int wgt_QVideoPlayer::GetButtonBarHeight() {
    return ui->VideoPlayerPlayPauseBT->height();
}

//============================================================================================

void wgt_QVideoPlayer::SetBackgroundColor(QColor Background) {
    QString Sheet=QString("background-color: rgb(%1,%2,%3);").arg(Background.red(),10).arg(Background.green(),10).arg(Background.blue(),10);
}

//============================================================================================

void wgt_QVideoPlayer::EnableWidget(bool State) {
    if (ui->CustomRuler!=NULL) ui->CustomRuler->setEnabled(State);
}

//============================================================================================
// Init a diaporama show
//============================================================================================

bool wgt_QVideoPlayer::InitDiaporamaPlay(cDiaporama *Diaporama) {
    if (Diaporama==NULL) return false;
    ApplicationConfig   =Diaporama->ApplicationConfig;
    this->Diaporama     =Diaporama;
    WantedFPS           =Diaporama->ApplicationConfig->PreviewFPS;
    ImageList.Diaporama =Diaporama;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->CustomRuler->ActiveSlider(Diaporama->GetDuration());
    PlayerPlayMode  = true;
    PlayerPauseMode = true;
    ui->VideoPlayerPlayPauseBT->setIcon(IconPause);
    QApplication::restoreOverrideCursor();
    return true;
}

//============================================================================================
// Init a video show
//============================================================================================

bool wgt_QVideoPlayer::StartPlay(cVideoFile *theFileInfo,double theWantedFPS) {
    if (theFileInfo==NULL) return false;
    FileInfo =theFileInfo;
    WantedFPS=theWantedFPS;
    IsValide=true;
    SDLFlushBuffers();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    SetActualDuration(QTime(0,0,0,0).msecsTo(FileInfo->Duration));
    resizeEvent(NULL);
    PlayerPlayMode  = true;
    PlayerPauseMode = true;
    ui->VideoPlayerPlayPauseBT->setIcon(IconPause);
    QApplication::restoreOverrideCursor();
    return true;
}

//============================================================================================
// Pause -> play
//============================================================================================

void wgt_QVideoPlayer::SetPlayerToPlay() {
    if (!(PlayerPlayMode && PlayerPauseMode)) return;
    PlayerPlayMode  = true;
    PlayerPauseMode = false;
    ui->VideoPlayerPlayPauseBT->setIcon(IconPlay);
    SDLFlushBuffers();

    // Start timer
    TimerTick           =true;
    PreviousTimerEvent  =QTime();
    TimerDelta          =0;
    Timer.start(int(double(1000)/WantedFPS)/2);   // Start Timer
}

//============================================================================================
// Play -> pause
//============================================================================================

void wgt_QVideoPlayer::SetPlayerToPause() {
    if (!(PlayerPlayMode && !PlayerPauseMode)) return;
    Timer.stop();                                   // Stop Timer
    if (ThreadPrepareVideo.isRunning()) ThreadPrepareVideo.waitForFinished();
    if (ThreadPrepareImage.isRunning()) ThreadPrepareImage.waitForFinished();
    if (ThreadAssembly.isRunning())     ThreadAssembly.waitForFinished();
    PlayerMutex.lock();
    if (SDL_GetAudioStatus()==SDL_AUDIO_PLAYING) SDL_PauseAudio(1);
    MixedMusic.ClearList();                         // Free sound buffers
    Music.ClearList();                              // Free sound buffers
    ImageList.ClearList();                          // Free ImageList
    PlayerPlayMode  = true;
    PlayerPauseMode = true;
    ui->VideoPlayerPlayPauseBT->setIcon(IconPause);
    ui->BufferState->setValue(0);
    SDLFlushBuffers();
    PlayerMutex.unlock();
}

//============================================================================================
// Click on the play/pause button
//============================================================================================

void wgt_QVideoPlayer::s_VideoPlayerPlayPauseBT() {
    if ((!PlayerPlayMode)||((PlayerPlayMode && PlayerPauseMode)))    SetPlayerToPlay();      // Stop/Pause -> play
        else if (PlayerPlayMode && !PlayerPauseMode)                 SetPlayerToPause();     // Pause -> play
}

//============================================================================================
// Click on the handle of the slider
//============================================================================================

void wgt_QVideoPlayer::s_SliderPressed() {
    PreviousPause    = PlayerPauseMode;    // Save pause state
    IsSliderProcess  = true;
    SetPlayerToPause();
}

//============================================================================================
// En slider process
//============================================================================================

void wgt_QVideoPlayer::s_SliderReleased() {
    IsSliderProcess  = false;
    s_SliderMoved(ActualPosition);
    // Restore saved pause state
    if (!PreviousPause) SetPlayerToPlay();
}

//============================================================================================
// Slider is moving by user
// Slider is moving by timer
// Slider is moving by seek function
//============================================================================================

void wgt_QVideoPlayer::s_SliderMoved(int Value) {
    if (((MainWindow *)ApplicationConfig->TopLevelWindow)->InPlayerUpdate) return;
    ((MainWindow *)ApplicationConfig->TopLevelWindow)->InPlayerUpdate=true;

    if (ResetPositionWanted) SetPlayerToPause();

    // Update display in controls
    ui->CustomRuler->setValue(Value);
    ActualPosition=Value;
    ui->Position->setText(GetCurrentPos().toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));
    ui->Duration->setText(tDuration.toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));

    //***********************************************************************
    // If playing
    //***********************************************************************
    if (PlayerPlayMode && !PlayerPauseMode) {

        if (((FileInfo)&&(ActualPosition>=QTime(0,0,0,0).msecsTo(tDuration)))||((Diaporama)&&(Value>=Diaporama->GetDuration()))) {

            SetPlayerToPause();    // Stop if it's the end

        } else if (ImageList.List.count()>1) {                        // Process
            // Retrieve frame information
            cDiaporamaObjectInfo *Frame=ImageList.DetachFirstImage();

            // Display frame
            if (!Frame->RenderedImage.isNull()) ui->MovieFrame->SetImage(Frame->RenderedImage.scaledToHeight(ui->MovieFrame->height()));

            // If Diaporama mode and needed, set Diaporama to another object
            if (Diaporama) {
                if (Diaporama->CurrentCol!=Frame->CurrentObject_Number) {
                    Diaporama->CurrentCol=Frame->CurrentObject_Number;
                    ((MainWindow *)ApplicationConfig->TopLevelWindow)->SetTimelineCurrentCell(Frame->CurrentObject_Number);

                    // Update slider mark
                    if (Diaporama->List.count()>0)
                        SetStartEndPos(
                                Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                           // Current slide
                                Diaporama->List[Diaporama->CurrentCol]->GetDuration(),
                                (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1),                            // Previous slide
                                (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1]->GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,    // Next slide
                                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1]->GetDuration():0);
                    else SetStartEndPos(0,0,-1,0,-1,0);
               }
               Diaporama->CurrentPosition=Value;
            }

            // Start sound (if not previously started)
            if ((PlayerPlayMode) && (!PlayerPauseMode) && (SDL_GetAudioStatus()!=SDL_AUDIO_PLAYING)) SDL_PauseAudio(0);

            // Free frame
            delete Frame;
        }

    //***********************************************************************
    // If moving by user
    //***********************************************************************
    } else {

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        if (FileInfo) {

            QImage *VideoImage=FileInfo->ImageAt(true,ActualPosition,NULL,Deinterlace,1,false,false);
            if (VideoImage) {
                // Display frame
                ui->MovieFrame->SetImage(VideoImage->scaledToHeight(ui->MovieFrame->height()));
                delete VideoImage;
            }

        } else if (Diaporama) {
            if (ThreadPrepareVideo.isRunning()) ThreadPrepareVideo.waitForFinished();
            if (ThreadPrepareImage.isRunning()) ThreadPrepareImage.waitForFinished();
            if (ThreadAssembly.isRunning())     ThreadAssembly.waitForFinished();

            // Create a frame from actual position
            cDiaporamaObjectInfo *Frame=new cDiaporamaObjectInfo(NULL,ActualPosition,Diaporama,double(1000)/WantedFPS,false);

            qreal Ratio=qreal(ui->MovieFrame->width())/qreal(ui->MovieFrame->height());
            int H=ui->MovieFrame->height();
            int W=int(H*Ratio);
            if ((Frame->IsTransition)&&(Frame->TransitObject)) Diaporama->CreateObjectContextList(Frame,W,H,false,true,true,PreparedTransitBrushList,Diaporama);
            Diaporama->CreateObjectContextList(Frame,W,H,true,true,true,PreparedBrushList,Diaporama);
            PrepareImage(false,true,Frame,W,H);         // This will add frame to the ImageList

            if (ThreadAssembly.isRunning()) ThreadAssembly.waitForFinished();

            Frame=ImageList.DetachFirstImage();     // Then detach frame from the ImageList

            // Display frame
            ui->MovieFrame->SetImage(Frame->RenderedImage.scaledToHeight(ui->MovieFrame->height()));

            // If needed, set Diaporama to another object
            if ((Diaporama->CurrentCol!=Frame->CurrentObject_Number)&&((!Frame->IsTransition)||(Diaporama->CurrentCol!=Frame->TransitObject_Number))) {
                if (FLAGSTOPITEMSELECTION!=NULL) *FLAGSTOPITEMSELECTION=true;    // Ensure mainwindow no modify player widget position
                Diaporama->CurrentCol=Frame->CurrentObject_Number;
                ((MainWindow *)ApplicationConfig->TopLevelWindow)->SetTimelineCurrentCell(Frame->CurrentObject_Number);
                if (FLAGSTOPITEMSELECTION!=NULL) *FLAGSTOPITEMSELECTION=false;

                // Update slider mark
                if (Diaporama->List.count()>0)
                    SetStartEndPos(
                            Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                               // Current slide
                            Diaporama->List[Diaporama->CurrentCol]->GetDuration(),
                            (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1), // Previous slide
                            (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1]->GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                            Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,        // Next slide
                            Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1]->GetDuration():0);

            }
            Diaporama->CurrentPosition=Value;

            // Free frame
            delete Frame;
        }
        QApplication::restoreOverrideCursor();
    }
    ((MainWindow *)ApplicationConfig->TopLevelWindow)->InPlayerUpdate=false;
}

//============================================================================================
// Timer event
//============================================================================================

void wgt_QVideoPlayer::s_TimerEvent() {
    if (IsSliderProcess)                            return;     // No re-entrance
    if (!(PlayerPlayMode && !PlayerPauseMode))      return;     // Only if play mode

    TimerTick=!TimerTick;

    #ifdef Q_OS_WIN
    // Trylock is always true on Windows instead of unix/linux system
    if (TimerTick) {
    #else
    if (!PlayerMutex.tryLock()) { if (!TimerTick) return; else {
    #endif
        // specific case for windows because never a timer event can happens if a previous timer event was not ended
        // so next trylock is always true
        int Elapsed=0,Wanted=int(double(1000)/WantedFPS);
        if (!PreviousTimerEvent.isValid()) PreviousTimerEvent.start(); else Elapsed=PreviousTimerEvent.restart();
        if (Elapsed>Wanted) {
            TimerDelta+=Elapsed-Wanted;
            if (TimerDelta>=Wanted) {
                ToLog(LOGMSG_DEBUGTRACE,"FPS preview is too high: One image lost");
                if (ImageList.List.count()>0) {
                    delete ImageList.DetachFirstImage(); // Remove first image if we loose one tick
                } else {
                    // Increase next position to one frame
                    if (FileInfo) ActualPosition+=Wanted;
                        else      Diaporama->CurrentPosition+=Wanted;
                }
                TimerDelta-=Wanted;
            }
        }
    }
    #ifdef Q_OS_WIN
    PlayerMutex.lock();
    #else
    return;}
    #endif

    if (ThreadPrepareVideo.isRunning()) ThreadPrepareVideo.waitForFinished();
    if (ThreadPrepareImage.isRunning()) ThreadPrepareImage.waitForFinished();
    if (ThreadAssembly.isRunning())     ThreadAssembly.waitForFinished();

    int64_t LastPosition=0,NextPosition=0;

    if (ResetPositionWanted) {
        Mutex.lock();
        MixedMusic.ClearList();                         // Free sound buffers
        Music.ClearList();                              // Free sound buffers
        ImageList.ClearList();                          // Free ImageList
        ResetPositionWanted=false;
        Mutex.unlock();
    }

    if (ThreadAssembly.isRunning()) ThreadAssembly.waitForFinished();

    // If no image in the list then create the first
    if (ImageList.List.count()==0) {

        if (FileInfo) LastPosition=ActualPosition;
            else      LastPosition=Diaporama->CurrentPosition;
        NextPosition=LastPosition+int(double(1000)/WantedFPS);

        // If no image in the list then prepare a first frame
        if (FileInfo) {

            cDiaporamaObjectInfo *NewFrame=new cDiaporamaObjectInfo(NULL,NextPosition,NULL,double(1000)/WantedFPS,true);
            NewFrame->CurrentObject_StartTime   =0;
            PrepareVideoFrame(NewFrame,NewFrame->CurrentObject_InObjectTime);

        } else {

            cDiaporamaObjectInfo *Frame=new cDiaporamaObjectInfo(NULL,NextPosition,Diaporama,double(1000)/WantedFPS,true);

            // Ensure MusicTracks are ready
            if ((Frame->CurrentObject)&&(Frame->CurrentObject_MusicTrack==NULL)) {
                Frame->CurrentObject_MusicTrack=new cSDLSoundBlockList();
                Frame->CurrentObject_MusicTrack->SetFPS(double(1000)/double(WantedFPS),2,Diaporama->ApplicationConfig->PreviewSamplingRate,AV_SAMPLE_FMT_S16);
            }
            if ((Frame->TransitObject)&&(Frame->TransitObject_MusicTrack==NULL)&&(Frame->TransitObject_MusicObject!=NULL)&&(Frame->TransitObject_MusicObject!=Frame->CurrentObject_MusicObject)) {
                Frame->TransitObject_MusicTrack=new cSDLSoundBlockList();
                Frame->TransitObject_MusicTrack->SetFPS(double(1000)/double(WantedFPS),2,Diaporama->ApplicationConfig->PreviewSamplingRate,AV_SAMPLE_FMT_S16);
            }

            // Ensure SoundTracks are ready
            if ((Frame->CurrentObject)&&(Frame->CurrentObject_SoundTrackMontage==NULL)) {
                Frame->CurrentObject_SoundTrackMontage=new cSDLSoundBlockList();
                Frame->CurrentObject_SoundTrackMontage->SetFPS(double(1000)/double(WantedFPS),2,Diaporama->ApplicationConfig->PreviewSamplingRate,AV_SAMPLE_FMT_S16);
            }
            if ((Frame->TransitObject)&&(Frame->TransitObject_SoundTrackMontage==NULL)) {
                Frame->TransitObject_SoundTrackMontage=new cSDLSoundBlockList();
                Frame->TransitObject_SoundTrackMontage->SetFPS(double(1000)/double(WantedFPS),2,Diaporama->ApplicationConfig->PreviewSamplingRate,AV_SAMPLE_FMT_S16);
            }

            // Ensure background, image and soundtrack is ready
            qreal Ratio=qreal(ui->MovieFrame->width())/qreal(ui->MovieFrame->height());
            int H=ui->MovieFrame->height();
            int W=int(H*Ratio);
            if ((Frame->IsTransition)&&(Frame->TransitObject)) Diaporama->CreateObjectContextList(Frame,W,H,false,true,true,PreparedTransitBrushList,Diaporama);
            Diaporama->CreateObjectContextList(Frame,W,H,true,true,true,PreparedBrushList,Diaporama);
            PrepareImage(true,true,Frame,W,H);
            if (ThreadAssembly.isRunning()) ThreadAssembly.waitForFinished();
        }
    }

    cDiaporamaObjectInfo *PreviousFrame=ImageList.GetLastImage();

    if (FileInfo)       LastPosition=PreviousFrame->CurrentObject_InObjectTime;
    else if (Diaporama) LastPosition=PreviousFrame->CurrentObject_StartTime+PreviousFrame->CurrentObject_InObjectTime;

    NextPosition=LastPosition+int(double(1000)/WantedFPS);

    // Add image to the list if it's not full
    if ((FileInfo)&&(ImageList.List.count()<BUFFERING_NBR_FRAME)&&(!ThreadPrepareVideo.isRunning())) {

        cDiaporamaObjectInfo *NewFrame=new cDiaporamaObjectInfo(PreviousFrame,NextPosition,NULL,int(double(1000)/WantedFPS),true);
        NewFrame->CurrentObject_StartTime   =0;
        ThreadPrepareVideo.setFuture(QtConcurrent::run(this,&wgt_QVideoPlayer::PrepareVideoFrame,NewFrame,NewFrame->CurrentObject_InObjectTime));

    } else if (((Diaporama)&&(ImageList.List.count()<BUFFERING_NBR_FRAME))&&(!ThreadPrepareImage.isRunning()))  {

        cDiaporamaObjectInfo *Frame=new cDiaporamaObjectInfo(PreviousFrame,NextPosition,Diaporama,double(1000)/WantedFPS,true);

        // Ensure MusicTracks are ready
        if ((Frame->CurrentObject)&&(Frame->CurrentObject_MusicTrack==NULL)) {
            Frame->CurrentObject_MusicTrack=new cSDLSoundBlockList();
            Frame->CurrentObject_MusicTrack->SetFPS(double(1000)/double(WantedFPS),2,Diaporama->ApplicationConfig->PreviewSamplingRate,AV_SAMPLE_FMT_S16);
        }
        if ((Frame->TransitObject)&&(Frame->TransitObject_MusicTrack==NULL)&&(Frame->TransitObject_MusicObject!=NULL)&&(Frame->TransitObject_MusicObject!=Frame->CurrentObject_MusicObject)) {
            Frame->TransitObject_MusicTrack=new cSDLSoundBlockList();
            Frame->TransitObject_MusicTrack->SetFPS(double(1000)/double(WantedFPS),2,Diaporama->ApplicationConfig->PreviewSamplingRate,AV_SAMPLE_FMT_S16);
        }

        // Ensure SoundTracks are ready
        if ((Frame->CurrentObject)&&(Frame->CurrentObject_SoundTrackMontage==NULL)) {
            Frame->CurrentObject_SoundTrackMontage=new cSDLSoundBlockList();
            Frame->CurrentObject_SoundTrackMontage->SetFPS(double(1000)/double(WantedFPS),2,Diaporama->ApplicationConfig->PreviewSamplingRate,AV_SAMPLE_FMT_S16);
        }
        if ((Frame->TransitObject)&&(Frame->TransitObject_SoundTrackMontage==NULL)) {
            Frame->TransitObject_SoundTrackMontage=new cSDLSoundBlockList();
            Frame->TransitObject_SoundTrackMontage->SetFPS(double(1000)/double(WantedFPS),2,Diaporama->ApplicationConfig->PreviewSamplingRate,AV_SAMPLE_FMT_S16);
        }

        // Ensure background, image and soundtrack is ready
        qreal Ratio=qreal(ui->MovieFrame->width())/qreal(ui->MovieFrame->height());
        int H=ui->MovieFrame->height();
        int W=int(H*Ratio);
        if ((Frame->IsTransition)&&(Frame->TransitObject)) Diaporama->CreateObjectContextList(Frame,W,H,false,true,true,PreparedTransitBrushList,Diaporama);
        Diaporama->CreateObjectContextList(Frame,W,H,true,true,true,PreparedBrushList,Diaporama);
        ThreadPrepareImage.setFuture(QtConcurrent::run(this,&wgt_QVideoPlayer::PrepareImage,true,true,Frame,W,H));
    }

    PlayerMutex.unlock();

    // if TimerTick update the preview
    if ((TimerTick)&&(ui->CustomRuler!=NULL))
        s_SliderMoved(ImageList.GetFirstImage()->CurrentObject_StartTime+ImageList.GetFirstImage()->CurrentObject_InObjectTime);

    ui->BufferState->setValue(ImageList.List.count());
    if (ImageList.List.count()<2)
        ui->BufferState->setStyleSheet("QProgressBar:horizontal {\nborder: 0px;\nborder-radius: 0px;\nbackground: black;\npadding-top: 1px;\npadding-bottom: 2px;\npadding-left: 1px;\npadding-right: 1px;\n}\nQProgressBar::chunk:horizontal {\nbackground: red;\n}");
    else if (ImageList.List.count()<4)
        ui->BufferState->setStyleSheet("QProgressBar:horizontal {\nborder: 0px;\nborder-radius: 0px;\nbackground: black;\npadding-top: 1px;\npadding-bottom: 2px;\npadding-left: 1px;\npadding-right: 1px;\n}\nQProgressBar::chunk:horizontal {\nbackground: yellow;\n}");
    else if (ImageList.List.count()<=BUFFERING_NBR_FRAME)
        ui->BufferState->setStyleSheet("QProgressBar:horizontal {\nborder: 0px;\nborder-radius: 0px;\nbackground: black;\npadding-top: 1px;\npadding-bottom: 2px;\npadding-left: 1px;\npadding-right: 1px;\n}\nQProgressBar::chunk:horizontal {\nbackground: green;\n}");
}

//============================================================================================
// Function use directly or with thread to prepare an image number Column at given position
//============================================================================================

void wgt_QVideoPlayer::PrepareImage(bool SoundWanted,bool AddStartPos,cDiaporamaObjectInfo *Frame,int W,int H) {
    Diaporama->LoadSources(Frame,W,H,true,AddStartPos,PreparedTransitBrushList,PreparedBrushList);

    // Do Assembly
    ThreadAssembly.setFuture(QtConcurrent::run(this,&wgt_QVideoPlayer::StartThreadAssembly,ComputePCT(Frame->CurrentObject?Frame->CurrentObject->GetSpeedWave():0,Frame->TransitionPCTDone),Frame,W,H,SoundWanted));
}

void wgt_QVideoPlayer::StartThreadAssembly(double PCT,cDiaporamaObjectInfo *Frame,int W,int H,bool SoundWanted) {
    Diaporama->DoAssembly(PCT,Frame,W,H);
    Mutex.lock();
    // Append mixed musique to the queue
    if ((SoundWanted)&&(Frame->CurrentObject)) for (int j=0;j<Frame->CurrentObject_MusicTrack->NbrPacketForFPS;j++) {
        int16_t *Music=Frame->CurrentObject_MusicTrack->DetachFirstPacket();
        int16_t *Sound=(Frame->CurrentObject_SoundTrackMontage!=NULL)?Frame->CurrentObject_SoundTrackMontage->DetachFirstPacket():NULL;
        MixedMusic.MixAppendPacket(Frame->CurrentObject_StartTime+Frame->CurrentObject_InObjectTime,Music,Sound);
    }

    // Append this image to the queue
    ImageList.AppendImage(Frame);
    Mutex.unlock();
}

void wgt_QVideoPlayer::PrepareVideoFrame(cDiaporamaObjectInfo *NewFrame,int Position) {
    QImage *Temp=FileInfo->ImageAt(true,Position,&Music,Deinterlace,1,false,true);
    if (Temp) {
        NewFrame->RenderedImage=QImage(Temp->scaledToHeight(ui->MovieFrame->height()));
        delete Temp;
        for (int j=0;j<Music.NbrPacketForFPS;j++) MixedMusic.AppendPacket(Music.CurrentPosition,Music.DetachFirstPacket());
    }
    if (!NewFrame->RenderedImage.isNull()) ImageList.AppendImage(NewFrame);
        else delete NewFrame;
}
//============================================================================================
// Define zone selection on the ruller
//============================================================================================

void wgt_QVideoPlayer::SetStartEndPos(int StartPos,int Duration,int PreviousStartPos,int PrevisousDuration,int NextStartPos,int NextDuration) {
    ui->CustomRuler->StartPos          =StartPos;
    ui->CustomRuler->EndPos            =StartPos+Duration;
    ui->CustomRuler->PreviousStartPos  =PreviousStartPos;
    ui->CustomRuler->PrevisousEndPos   =PreviousStartPos+PrevisousDuration;
    ui->CustomRuler->NextStartPos      =NextStartPos;
    ui->CustomRuler->NextEndPos        =NextStartPos+NextDuration;
    ui->CustomRuler->repaint();
}

//============================================================================================
// Seek slider public function
//============================================================================================

void wgt_QVideoPlayer::SeekPlayer(int Value) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ActualPosition=-1;
    s_SliderMoved(Value);
    QApplication::restoreOverrideCursor();
}

//============================================================================================
// return current position in QTime format
//============================================================================================

QTime wgt_QVideoPlayer::GetCurrentPos() {
    if (ActualPosition!=-1) {
        int     TimeMSec    =ActualPosition-(ActualPosition/1000)*1000;
        int     TimeSec     =int(ActualPosition/1000);
        int     TimeHour    =TimeSec/(60*60);
        int     TimeMinute  =(TimeSec%(60*60))/60;
        QTime   tPosition;
        tPosition.setHMS(TimeHour,TimeMinute,TimeSec%60,TimeMSec);
        return tPosition;
    } else return QTime(0,0,0,0);
}

//============================================================================================
// return current duration in QTime format
//============================================================================================

QTime wgt_QVideoPlayer::GetActualDuration() {
    return tDuration;
}

//============================================================================================
// define duration (in msec)
//============================================================================================

void wgt_QVideoPlayer::SetActualDuration(int Duration) {
    if (ui->CustomRuler!=NULL) {
        ui->CustomRuler->setMaximum(Duration-1);
        //ui->CustomRuler->repaint();
    }
    ui->CustomRuler->TotalDuration=Duration;
    //ui->CustomRuler->repaint();
    int     TimeMSec    =(Duration %1000);
    int     TimeSec     =int(Duration/1000);
    int     TimeHour    =TimeSec/(60*60);
    int     TimeMinute  =(TimeSec%(60*60))/60;
    tDuration.setHMS(TimeHour,TimeMinute,TimeSec%60,TimeMSec);
    ui->Position->setText(GetCurrentPos().toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));
    ui->Duration->setText(tDuration.toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));
}

//============================================================================================

void wgt_QVideoPlayer::s_PositionChangeByUser() {
    ResetPositionWanted=true;
}
