/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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


#ifndef DLGMUSICPROPERTIES_H
#define DLGMUSICPROPERTIES_H

// Basic inclusions (common to all files)
#include "../../engine/_GlobalDefines.h"
#include "../../engine/_QCustomDialog.h"

// Specific inclusions
#include "../_Diaporama.h"

namespace Ui {
    class DlgMusicProperties;
}

class DlgMusicProperties : public QCustomDialog {
Q_OBJECT
public:
    cDiaporamaObject    *DiaporamaObject;

    explicit DlgMusicProperties(cDiaporamaObject *DiaporamaObject,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent=0);
    ~DlgMusicProperties();

    // function to be overloaded
    virtual void    DoInitDialog();                             // Initialise dialog
    virtual void    DoAccept();                                 // Call when user click on Ok button
    virtual void    DoRejet()           {/*Nothing to do*/}     // Call when user click on Cancel button
    virtual void    PrepareGlobalUndo();                        // Initiale Undo
    virtual void    DoGlobalUndo();                             // Apply Undo : call when user click on Cancel button

    virtual void    RefreshControl(bool RefreshList=false);

private slots:
    void    s_SameMusic();
    void    s_SameMusicNormal();
    void    s_SameMusicReduceVolume();
    void    s_SameMusicPause();
    void    s_NewMusic();
    void    s_AddMusic();
    void    s_RemoveMusic();
    void    s_PlayListTable_DoubleClick(QTableWidgetItem *);
    void    s_PlayListTable_SelectionChanged();
    void    s_UpMusic();
    void    s_DownMusic();

signals:
    void    SetModifyFlag();

private:
    void SetupUi();
    Ui::DlgMusicProperties *ui;
};

#endif // DLGMUSICPROPERTIES_H
