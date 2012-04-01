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

#ifndef DLGSLIDEPROPERTIES_H
#define DLGSLIDEPROPERTIES_H

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"
#include "../sharedfiles/_QCustomDialog.h"

// Specific inclusions
#include "_Diaporama.h"
#include "cCustomGraphicsRectItem.h"
#include "_ApplicationDefinitions.h"

#include "cInteractiveZone.h"

namespace Ui {
    class DlgSlideProperties;
}

class DlgSlideProperties : public QCustomDialog {
Q_OBJECT
public:
    QDomDocument            *Undo;                      // Save object before modification for cancel button

    double                  DisplayW,DisplayH;
    cDiaporamaObject        *CurrentSlide;              // Current slide

    SELECTMODE              ShotSelectMode;             // Current shot selection mode
    cDiaporamaShot          *CurrentShot;               // Current shot (if selection mode = SELECTMODE_ONE)
    int                     CurrentShotNbr;             // Current shot number (if selection mode = SELECTMODE_ONE)

    cCompositionList        *CompositionList;           // Link to current block List
    QList<bool>             IsSelected;                 // Table of selection state in the current block list
    int                     NbrSelected;                // Number of selected blocks
    SELECTMODE              BlockSelectMode;            // Current block selection mode
    cCompositionObject      *CurrentCompoObject;        // Current block object (if selection mode = SELECTMODE_ONE)
    int                     CurrentCompoObjectNbr;      // Number of Current block object (if selection mode = SELECTMODE_ONE)

    double                  ProjectGeometry;

    QString                 FramingStyleLabelPixmap;

    // Re-entrence flags
    bool                    InRefreshStyleControls;
    bool                    InRefreshControls;
    bool                    InSelectionChange;
    bool                    StopMajFramingStyle;

    explicit DlgSlideProperties(cDiaporamaObject *DiaporamaObject,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent = 0);
    ~DlgSlideProperties();

    // function to be overloaded
    virtual void            DoInitDialog();                             // Initialise dialog
    virtual void            DoAccept() {/*Nothing to do*/}              // Call when user click on Ok button
    virtual void            DoRejet()  {/*Nothing to do*/}              // Call when user click on Cancel button
    virtual void            PrepareGlobalUndo();                        // Initiale Undo
    virtual void            DoGlobalUndo();                             // Apply Undo : call when user click on Cancel button
    virtual void            SaveWindowState();
    virtual void            RestoreWindowState();

    void                    RefreshStyleControls();
    void                    RefreshControls(bool UpdateInteractiveZone=true);
    void                    AdjustApectRatio(cBrushDefinition *CurrentBrush,cCompositionObject *CurrentTextItem);
    void                    GetForDisplayUnit(double &DisplayW,double &DisplayH);
    void                    s_Scene_DoubleClick();                      // User double click on a block in the scene widget

    // Utility functions
    cCompositionObject      *GetSelectedCompositionObject();                // Return selected CompositionObject
    cCompositionObject      *GetGlobalCompositionObject(int IndexKey);      // Return CompositionObject in the global composition list for specific IndexKey
    cCompositionObject      *GetSelectedGlobalCompositionObject();          // Return selected CompositionObject in the global composition list
    void                    ApplyGlobalPropertiesToAllShots(cCompositionObject *GlobalBlock);
    void                    CopyBlockProperties(cCompositionObject *SourceBlock,cCompositionObject *DestBlock);

    void                    ApplyToContexte(bool ApplyGlobal);

protected:
    virtual void    resizeEvent(QResizeEvent *);
    virtual void    showEvent(QShowEvent *);

private slots:
    void            s_Event_ClipboardChanged();
    void            s_RefreshSceneImage();
    void            s_TVMarginsBt();

    void            OKPrevious();
    void            OKNext();

    // Slide settings
    void            s_SlideSet_SlideNameChange(QString NewText);
    void            s_SlideSet_NewChapter(int state);

    // Shot table & settings
    void            s_ShotTable_SelectionChanged();             // User select a shot in the ShotTable widget
    void            s_ShotTable_AddShot();
    void            s_ShotTable_RemoveShot();
    void            s_ShotTable_DragMoveItem();
    void            s_ShotTable_MoveLeft();
    void            s_ShotTable_MoveRight();
    void            s_ShotTable_DurationChange(QTime NewValue);

    // Block table
    void            s_Scene_SelectionChanged();                 // User select a block in the scene widget
    void            s_BlockTable_SelectionChanged();            // User select a block in the BlocTable widget
    void            s_BlockTable_StartSelectionChange();
    void            s_BlockTable_EndSelectionChange();
    void            s_BlockTable_ItemDoubleClicked(QMouseEvent *);
    void            s_BlockTable_ItemRightClicked(QMouseEvent *);
    void            s_BlockTable_AddNewTextBlock();
    void            s_BlockTable_AddNewFileBlock();
    void            s_BlockTable_RemoveBlock();
    void            s_BlockTable_Copy();
    void            s_BlockTable_Cut();
    void            s_BlockTable_Paste();
    void            s_BlockTable_MoveBlockUp();
    void            s_BlockTable_MoveBlockDown();

    // Block settings : Call of other dialog
    void            s_BlockSettings_Arrange();
    void            s_BlockSettings_TextEditor();
    void            s_BlockSettings_ImageEditCorrect();
    void            s_BlockSettings_VideoEdit();
    void            s_BlockSettings_Information();

    // Block settings : Basic values
    void            s_BlockSettings_ToggleVisibleState();
    void            s_BlockSettings_GetSound();

    // Block settings : Coordinates
    void            s_BlockSettings_PosXValue(double);
    void            s_BlockSettings_PosYValue(double);
    void            s_BlockSettings_PosWidthValue(double);
    void            s_BlockSettings_PosHeightValue(double);

    // Block settings : Rotations
    void            s_BlockSettings_RotateZValue(int);
    void            s_BlockSettings_ResetRotateZValue();
    void            s_BlockSettings_RotateXValue(int);
    void            s_BlockSettings_ResetRotateXValue();
    void            s_BlockSettings_RotateYValue(int);
    void            s_BlockSettings_ResetRotateYValue();

    // Block settings : Shape
    void            s_BlockSettings_ShapeBackgroundForm(int);
    void            s_BlockSettings_ShapeOpacity(int);
    void            s_BlockSettings_ShapeShadowFormValue(int);
    void            s_BlockSettings_ShapeShadowDistanceValue(int);
    void            s_BlockSettings_ShapePenSize(int);
    void            s_BlockSettings_ShapePenColor(int);
    void            s_BlockSettings_ShapePenStyle(int);
    void            s_BlockSettings_ShapeShadowColor(int);

    // Block settings : Style
    void            s_ChangeFramingStyle(int);
    void            s_CoordinateStyleBT();
    void            s_BlockShapeStyleBT();

    // Block settings : Alignment
    void            s_BlockTable_AlignTop();
    void            s_BlockTable_AlignMiddle();
    void            s_BlockTable_AlignBottom();
    void            s_BlockTable_AlignLeft();
    void            s_BlockTable_AlignCenter();
    void            s_BlockTable_AlignRight();

    // Block settings : Text animation
    void            s_BlockSettings_TextAnimZoom(int);
    void            s_BlockSettings_TextAnimZoomReset();
    void            s_BlockSettings_TextAnimScrollX(int);
    void            s_BlockSettings_TextAnimScrollXReset();
    void            s_BlockSettings_TextAnimScrollY(int);
    void            s_BlockSettings_TextAnimScrollYReset();

    // Block settings : Block animation
    void            s_BlockSettings_BlockAnimType(int);

    // Block settings/Block animation : Multiple turn animation
    void            s_BlockSettings_BlockAnimTurnZValue(int);
    void            s_BlockSettings_BlockAnimTurnZReset();
    void            s_BlockSettings_BlockAnimTurnXValue(int);
    void            s_BlockSettings_BlockAnimTurnXReset();
    void            s_BlockSettings_BlockAnimTurnYValue(int);
    void            s_BlockSettings_BlockAnimTurnYReset();

    // Block settings/Block animation : Dissolve animation
    void            s_BlockSettings_BlockAnimDissolveType(int);

    // Block settings/Interactive zone messages
    void            s_BlockSettings_IntZoneTransformBlocks(double DeltaX,double DeltaY,double ScaleX,double ScaleY,double Sel_X,double Sel_Y,double Sel_W,double Sel_H);
    void            s_BlockSettings_IntZoneDisplayTransformBlocks(double DeltaX,double DeltaY,double ScaleX,double ScaleY,double Sel_X,double Sel_Y,double Sel_W,double Sel_H);

private:
    void            MakeFormIcon(QComboBox *UICB);
    void            MakeBorderStyleIcon(QComboBox *UICB);

    void            s_ShotTable_DisplayDuration();
    void            RefreshBlockTable(int SetCurrentIndex);

    Ui::DlgSlideProperties *ui;
};

#endif // DLGSLIDEPROPERTIES_H
