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

#ifndef DLGFFDPJRPROPERTIES_H
#define DLGFFDPJRPROPERTIES_H

// Basic inclusions (common to all files)
#include "../../engine/_GlobalDefines.h"
#include "../../engine/_QCustomDialog.h"

// Include some additional standard class
#include <QString>
#include <QDialog>

// Include some common various class
#include "../../engine/cBaseApplicationConfig.h"
#include "_Diaporama.h"

namespace Ui {
    class DlgffDPjrProperties;
}

class DlgffDPjrProperties : public QCustomDialog {
Q_OBJECT
public:
    cDiaporama *ffdProject;
    bool       IsPrjCreate;

    explicit DlgffDPjrProperties(bool IsPrjCreate,cDiaporama *ffdProject,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent = 0);
    ~DlgffDPjrProperties();

    // function to be overloaded
    virtual void    DoInitDialog();                             // Initialise dialog
    virtual bool    DoAccept();                                 // Call when user click on Ok button
    virtual void    DoRejet()           {/*Nothing to do*/}     // Call when user click on Cancel button
    virtual void    PrepareGlobalUndo() {/*Nothing to do*/}     // Initiale Undo
    virtual void    DoGlobalUndo()      {/*Nothing to do*/}     // Apply Undo : call when user click on Cancel button

private:
    Ui::DlgffDPjrProperties *ui;
};

#endif // DLGFFDPJRPROPERTIES_H
