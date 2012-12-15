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

#ifndef CBRUSHDEFINITION_H
#define CBRUSHDEFINITION_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Include some additional standard class
#include <QRectF>
#include <QPainter>
#include <QImage>
#include <QString>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

// Include qimageblitz lib
#include <qimageblitz/qimageblitz.h>
#include <qimageblitz/blitzcpu.h>

// Include fmt_filters lib
#include "../fmt_filters/fmt_filters.h"

// Include some common various class
#include "_SpeedWave.h"
#include "_Shape.h"
#include "cLuLoImageCache.h"
#include "cBaseMediaFile.h"
#include "cSoundBlockList.h"

// Brush type definition
#define BRUSHTYPE_NOBRUSH                   0
#define BRUSHTYPE_SOLID                     1
#define BRUSHTYPE_PATTERN                   2
#define BRUSHTYPE_GRADIENT2                 3
#define BRUSHTYPE_GRADIENT3                 4
#define BRUSHTYPE_IMAGELIBRARY              5
#define BRUSHTYPE_IMAGEDISK                 6

// Brush Gradient orientation
#define GRADIENTORIENTATION_UPLEFT          0
#define GRADIENTORIENTATION_UP              1
#define GRADIENTORIENTATION_UPRIGHT         2
#define GRADIENTORIENTATION_LEFT            3
#define GRADIENTORIENTATION_RADIAL          4
#define GRADIENTORIENTATION_RIGHT           5
#define GRADIENTORIENTATION_BOTTOMLEFT      6
#define GRADIENTORIENTATION_BOTTOM          7
#define GRADIENTORIENTATION_BOTTOMRIGHT     8

// Composition type definition
#define COMPOSITIONTYPE_BACKGROUND          0
#define COMPOSITIONTYPE_OBJECT              1
#define COMPOSITIONTYPE_SHOT                2

// Define possible values for images type
#define IMAGETYPE_UNKNOWN                   0
#define IMAGETYPE_PHOTOLANDSCAPE            1
#define IMAGETYPE_PHOTOPORTRAIT             2
#define IMAGETYPE_CLIPARTLANDSCAPE          3   // Cliparts are images less than 1920x1080
#define IMAGETYPE_CLIPARTPORTRAIT           4
#define IMAGETYPE_VIDEOLANDSCAPE            5
#define IMAGETYPE_VIDEOPORTRAIT             6
#define NBR_IMAGETYPE                       7

// Brush Auto Framing definition
#define AUTOFRAMING_CUSTOMUNLOCK            0
#define AUTOFRAMING_CUSTOMLOCK              1
#define AUTOFRAMING_CUSTOMIMGLOCK           2
#define AUTOFRAMING_CUSTOMPRJLOCK           3
#define AUTOFRAMING_FULLMAX                 4
#define AUTOFRAMING_FULLMIN                 5
#define AUTOFRAMING_HEIGHTLEFTMAX           6
#define AUTOFRAMING_HEIGHTLEFTMIN           7
#define AUTOFRAMING_HEIGHTMIDLEMAX          8
#define AUTOFRAMING_HEIGHTMIDLEMIN          9
#define AUTOFRAMING_HEIGHTRIGHTMAX          10
#define AUTOFRAMING_HEIGHTRIGHTMIN          11
#define AUTOFRAMING_WIDTHTOPMAX             12
#define AUTOFRAMING_WIDTHTOPMIN             13
#define AUTOFRAMING_WIDTHMIDLEMAX           14
#define AUTOFRAMING_WIDTHMIDLEMIN           15
#define AUTOFRAMING_WIDTHBOTTOMMAX          16
#define AUTOFRAMING_WIDTHBOTTOMMIN          17
#define NBR_AUTOFRAMING                     18

#define AUTOFRAMING_ICON_GEOMETRY_LOCKED    ":/img/Geometry_Lock.png"
#define AUTOFRAMING_ICON_GEOMETRY_UNLOCKED  ":/img/Geometry_Unlock.png"
#define AUTOFRAMING_ICON_GEOMETRY_PROJECT   ":/img/Geometry_ProjectLock.png"
#define AUTOFRAMING_ICON_GEOMETRY_IMAGE     ":/img/Geometry_ImageLock.png"

#define AUTOFRAMING_GEOMETRY_CUSTOM         0
#define AUTOFRAMING_GEOMETRY_PROJECT        1
#define AUTOFRAMING_GEOMETRY_IMAGE          2

//============================================

#define AUTOCOMPOSIZE_CUSTOM                0
#define AUTOCOMPOSIZE_REALSIZE              1
#define AUTOCOMPOSIZE_FULLSCREEN            2
#define AUTOCOMPOSIZE_TVMARGINS             3
#define AUTOCOMPOSIZE_TWOTHIRDSSCREEN       4
#define AUTOCOMPOSIZE_HALFSCREEN            5
#define AUTOCOMPOSIZE_THIRDSCREEN           6
#define AUTOCOMPOSIZE_QUARTERSCREEN         7

//============================================
// OnOffFilter mask definition
//============================================

#define FilterEqualize                      0x0001
#define FilterDespeckle                     0x0002
#define FilterGray                          0x0004
#define FilterNegative                      0x0008
#define FilterEmboss                        0x0010
#define FilterEdge                          0x0020
#define FilterAntialias                     0x0040
#define FilterNormalize                     0x0080
#define FilterCharcoal                      0x0100
#define FilterOil                           0x0200

//============================================
// Auto Framing
//============================================

class cAutoFramingDef {
public:
    QString     ToolTip;
    int         GeometryType;
    bool        IsInternal;

    cAutoFramingDef() {}
    cAutoFramingDef(QString ToolTip,bool IsInternal,int GeometryType);
};
extern cAutoFramingDef AUTOFRAMINGDEF[NBR_AUTOFRAMING];
void   AutoFramingDefInit();

//============================================
// Brush
//============================================

extern  QBrush  Transparent;    // Transparent brush
QBrush  *GetGradientBrush(QRectF Rect,int BrushType,int GradientOrientation,QString ColorD,QString ColorF,QString ColorIntermed,double Intermediate);

//*********************************************************************************************************************************************
// Base object for background library object
//*********************************************************************************************************************************************

class   cBackgroundObject {
public:
    bool                    IsValide;
    QString                 FilePath;
    QDateTime               ModifDateTime;
    cBaseApplicationConfig *ApplicationConfig;
    QString                 Name;

    cBackgroundObject(QString FileName,cBaseApplicationConfig *ApplicationConfig);

    QImage* GetBackgroundImage();
    QImage  GetBackgroundThumb(int Geometry);

private:
    int         CurrentGeometry;
    QImage      Thumbnail[3];
};

//*********************************************************************************************************************************************
// Global class containing background library
//*********************************************************************************************************************************************

class   cBackgroundList {
public:
    QList<cBackgroundObject> List;                       // list of brush

    cBackgroundList();

    void    ScanDisk(QString Path,cBaseApplicationConfig *ApplicationConfig);
    int     SearchImage(QString NameToFind);
};
extern  cBackgroundList BackgroundList;

//*********************************************************************************************************************************************
// Base object for brush object
//*********************************************************************************************************************************************

class cBrushDefinition {
public:
    int                     TypeComposition;            // Type of composition object (COMPOSITIONTYPE_BACKGROUND, COMPOSITIONTYPE_OBJECT, COMPOSITIONTYPE_SHOT)
    int                     BrushType;                  // 0=no brush, 1=Solid, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors
    int                     PatternType;                // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
    int                     GradientOrientation;        // 0=Radial, 1->4=Linear from a corner, 5->9=Linear from a border
    QString                 ColorD;                     // First Color
    QString                 ColorF;                     // Last Color
    QString                 ColorIntermed;              // Intermediate Color
    double                  Intermediate;               // Intermediate position of 2nd color (in %) for gradient 3 colors
    QString                 BrushImage;                 // Image name if image from library
    double                  SoundVolume;                // Volume of soundtrack
    bool                    Deinterlace;                // Add a YADIF filter to deinterlace video (on/off)
    cImageFile              *Image;                     // Embeded Object for title and image type
    cVideoFile              *Video;                     // Embeded Object for video type

    // Image correction part
    double                  X;                          // X position (in %) relative to up/left corner
    double                  Y;                          // Y position (in %) relative to up/left corner
    double                  ZoomFactor;                 // Zoom factor (in %)
    double                  ImageRotation;              // Image rotation (in °)
    int                     Brightness;                 // Brightness adjustment
    int                     Contrast;                   // Contrast adjustment
    double                  Gamma;                      // Gamma adjustment
    int                     Red;                        // Red adjustment
    int                     Green;                      // Green adjustment
    int                     Blue;                       // Blue adjustment
    bool                    LockGeometry;               // True if geometry is locked
    double                  AspectRatio;                // Aspect Ratio of image
    bool                    FullFilling;                // Background image disk only : If true aspect ratio is not keep and image is deformed to fill the frame

    double                  GaussBlurSharpenSigma,BlurSharpenRadius;       // Blur/Sharpen parameters
    int                     TypeBlurSharpen,QuickBlurSharpenSigma;
    double                  Desat,Swirl,Implode;        // Filter parameters
    double                  WaveAmp,WaveFreq;           // Filter parameters

    int                     OnOffFilter;                // On-Off filter = combination of Despeckle, Equalize, Gray and Negative;
    int                     ImageSpeedWave;

    // Link to global objects
    cBaseApplicationConfig  *ApplicationConfig;
    cBackgroundList         *BackgroundList;

                            cBrushDefinition(cBaseApplicationConfig *TheApplicationConfig,cBackgroundList *TheBackgroundList);
                            ~cBrushDefinition();

    void                    CopyFromBrushDefinition(cBrushDefinition *BrushToCopy);
    void                    SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath);
    bool                    LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag);
    QBrush                  *GetBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush,bool UseBrushCache=false);

    int                     GetImageType();

    // Image correction part
    QImage                  ApplyFilter(QImage Image);

    // Utility functions used to draw thumbnails of image
    QImage                  *ImageToWorkspace(QImage *SrcImage,int WantedSize,qreal &maxw,qreal &maxh,qreal &minw,qreal &minh,qreal &x1,qreal &x2,qreal &x3,qreal &x4,qreal &y1,qreal &y2,qreal &y3,qreal &y4);
    void                    ApplyMaskToImageToWorkspace(QImage *SrcImage,QRectF CurSelRect,int BackgroundForm,int AutoFramingStyle=-1);
    void                    ApplyMaskToImageToWorkspace(QImage *SrcImage,int AutoFramingStyle,int BackgroundForm,int WantedSize,qreal maxw,qreal maxh,qreal minw,qreal minh,qreal X,qreal Y,qreal ZoomFactor,qreal AspectRatio,qreal ProjectGeometry);

    // Utility functions used to define default framing
    int                     GetCurrentFramingStyle(qreal ProjectGeometry);
    bool                    CalcWorkspace(qreal &dmax,qreal &maxw,qreal &maxh,qreal &minw,qreal &minh,qreal &x1,qreal &x2,qreal &x3,qreal &x4,qreal &y1,qreal &y2,qreal &y3,qreal &y4);
    void                    ApplyAutoFraming(int AutoAdjust,qreal ProjectGeometry);
    void                    s_AdjustWTop(qreal ProjectGeometry);
    void                    s_AdjustWMidle(qreal ProjectGeometry);
    void                    s_AdjustWBottom(qreal ProjectGeometry);
    void                    s_AdjustHLeft(qreal ProjectGeometry);
    void                    s_AdjustHMidle(qreal ProjectGeometry);
    void                    s_AdjustHRight(qreal ProjectGeometry);
    void                    s_AdjustWH();
    void                    s_AdjustMinWTop(qreal ProjectGeometry);
    void                    s_AdjustMinWMidle(qreal ProjectGeometry);
    void                    s_AdjustMinWBottom(qreal ProjectGeometry);
    void                    s_AdjustMinHLeft(qreal ProjectGeometry);
    void                    s_AdjustMinHMidle(qreal ProjectGeometry);
    void                    s_AdjustMinHRight(qreal ProjectGeometry);
    void                    s_AdjustMinWH();

private:
    QBrush                  *GetLibraryBrush(QRectF Rect);
    QBrush                  *GetImageDiskBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush,bool UseBrushCache);
    int                     GetHeightForWidth(int WantedWith,QRectF Rect);
    int                     GetWidthForHeight(int WantedHeight,QRectF Rect);
};

#endif // CBRUSHDEFINITION_H
