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

#ifndef CIMGINTERACTIVEZONE_H
#define CIMGINTERACTIVEZONE_H

#include "../../sharedfiles/_GlobalDefines.h"
#include "../_Diaporama.h"
#include "../_ApplicationDefinitions.h"

#include <QWidget>

enum TRANSFOTYPE {NOTYETDEFINED,MOVEBLOCK,RESIZEUPLEFT,RESIZEDOWNLEFT,RESIZEUPRIGHT,RESIZEDOWNRIGHT,RESIZELEFT,RESIZERIGHT,RESIZEUP,RESIZEDOWN};

class cImgInteractiveZone : public QWidget {
Q_OBJECT
public:
    int                 MagneticRuler;

    cCompositionObject  *CompoObject;
    int                 BackgroundForm;
    cBrushDefinition    *CurrentBrush;
    int                 VideoPosition;

    QImage              *ForegroundImage;
    QImage              *CachedImage;
    double              dmax;
    double              DstX,DstY,DstW,DstH;
    double              maxw,maxh;

    QRectF              SceneRect;
    QRectF              CurSelRect;
    bool                IsCapture;                      // True if there is an active capture
    QPoint              CapturePos;

    // Transformations
    TRANSFOTYPE         TransfoType;
    double              Move_X,Move_Y;                  // Blocks move
    double              Scale_X,Scale_Y;                // Blocks resize

    // Rulers
    QList<double>   MagnetVert;
    QList<double>   MagnetHoriz;

    explicit            cImgInteractiveZone(QWidget *parent = 0);
                        ~cImgInteractiveZone();

    void                RefreshDisplay();
    void                InitCachedImage(cCompositionObject *TheCompoObject,int TheBackgroundForm,cBrushDefinition *TheCurrentBrush,int TheVideoPosition);

protected:
    virtual void        paintEvent(QPaintEvent *event);

    virtual void	mouseMoveEvent(QMouseEvent *event);
    virtual void	mousePressEvent(QMouseEvent *event);
    virtual void	mouseReleaseEvent(QMouseEvent *event);
    virtual void	keyPressEvent(QKeyEvent *event);
    virtual void	keyReleaseEvent(QKeyEvent *event);

signals:
    void    TransformBlock(double Move_X,double Move_Y,double Scale_X,double Scale_Y);
    void    DisplayTransformBlock(double Move_X,double Move_Y,double Scale_X,double Scale_Y);

private:
    bool    IsInRect(QPoint Pos,QRect Rect);
    bool    IsInSelectedRect(QPoint Pos);
    void    ManageCursor(QPoint Pos,Qt::KeyboardModifiers Modifiers);
    QRect   ComputeNewCurSelRect();
    void    DrawSelect(QPainter &Painter,QRectF Rect,bool WithHandles);
};

#endif // CIMGINTERACTIVEZONE_H
