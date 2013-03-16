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

#include "cCTexteFrameComboBox.h"

#define ICONSIZE    64
#define NBRCOLUMN   8

//========================================================================================================================

cCTexteFrameComboBoxItem::cCTexteFrameComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCTexteFrameComboBoxItem::cCTexteFrameComboBoxItem");
    ComboBox=NULL;
}

//========================================================================================================================

void cCTexteFrameComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCTexteFrameComboBoxItem::paint");
    int CurIndex=index.row()*NBRCOLUMN+index.column();

    if ((CurIndex>=0)&&(CurIndex<TextFrameList.List.count())) {
        painter->drawImage(option.rect.left()+(option.rect.width()-TextFrameList.List[CurIndex].Icon.width())/2,
                           option.rect.top()+(option.rect.height()-TextFrameList.List[CurIndex].Icon.height())/2,
                           TextFrameList.List[CurIndex].Icon);
    } else {
        painter->fillRect(option.rect,Qt::white);
    }
    if ((ComboBox!=NULL)&&(ComboBox->CurrentSel==index.row()*((QTableWidget *)ComboBox->view())->columnCount()+index.column())) {
        painter->setPen(QPen(Qt::red));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x()+3,option.rect.y()+3,option.rect.width()-6-1,option.rect.height()-6-1);
    }
    if (option.state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::blue));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
        painter->drawRect(option.rect.x()+1,option.rect.y()+1,option.rect.width()-1-2,option.rect.height()-1-2);
        painter->setPen(QPen(Qt::black));
        painter->drawRect(option.rect.x()+2,option.rect.y()+2,option.rect.width()-1-4,option.rect.height()-1-4);
    }
}

//========================================================================================================================

QSize cCTexteFrameComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCTexteFrameComboBoxItem::sizeHint");
    return QSize(ICONSIZE,ICONSIZE);
}

//******************************************************************************************************************
// Custom Background ComboBox
//******************************************************************************************************************

cCTexteFrameComboBox::cCTexteFrameComboBox(QWidget *parent):QComboBox(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCTexteFrameComboBox::cCTexteFrameComboBox");
    CurrentSel          =-1;
    STOPMAJ             =false;

    QTableWidget *Table=new QTableWidget();
    Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    Table->setSelectionMode(QAbstractItemView::SingleSelection);
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    for (int i=0;i<NBRCOLUMN;i++) {
        Table->insertColumn(0);
        Table->setColumnWidth(0,ICONSIZE);
    }
    setModel(Table->model());
    setView(Table);
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->view()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cCTexteFrameComboBox::PrepareTable() {
    int ScrollBarWidth=((QTableWidget *)view())->verticalScrollBar()->width();
    this->view()->setFixedWidth(ICONSIZE*NBRCOLUMN+ScrollBarWidth);
    setIconSize(QSize(ICONSIZE,ICONSIZE));
    setFixedHeight(ICONSIZE+4);

    for (int i=0;i<NBRCOLUMN;i++) ((QTableWidget *)view())->setColumnWidth(i,ICONSIZE);
    int CurIndex  =((QTableWidget *)view())->currentRow()*NBRCOLUMN+((QTableWidget *)view())->currentColumn();
    while (count()>0) removeItem(count()-1);
    int NbrItem=TextFrameList.List.count();
    int NbrRow=NbrItem/NBRCOLUMN;
    if (NbrRow*NBRCOLUMN<NbrItem) NbrRow++;
    for (int i=0;i<NbrRow;i++) {
        addItem(QIcon(QPixmap().fromImage(TextFrameList.List[i*NBRCOLUMN].Icon)),"");    //automaticaly do a Table->insertRow(Table->rowCount());
        ((QTableWidget *)view())->setRowHeight(((QTableWidget *)view())->rowCount()-1,ICONSIZE);
    }
    ((QTableWidget *)view())->setCurrentCell(CurIndex/NBRCOLUMN,CurIndex-(CurIndex/NBRCOLUMN)*NBRCOLUMN);
    setCurrentIndex(CurIndex/NBRCOLUMN);
}

//========================================================================================================================

void cCTexteFrameComboBox::hidePopup() {
    QComboBox::hidePopup();
    emit itemSelectionHaveChanged();
    MakeIcons();
    CurrentSel=((QTableWidget *)view())->currentRow()*NBRCOLUMN+((QTableWidget *)view())->currentColumn();
    if (CurrentSel>=TextFrameList.List.count()) CurrentSel=TextFrameList.List.count()-1;
}

//========================================================================================================================

void cCTexteFrameComboBox::keyReleaseEvent(QKeyEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCTexteFrameComboBox::keyReleaseEvent");
    int CurIndex=((QTableWidget *)view())->currentRow()*NBRCOLUMN+((QTableWidget *)view())->currentColumn();
    int aCurIndex=CurIndex;
    if ((event->key()==Qt::Key_Right)||(event->key()==Qt::Key_Down)) {
        if (CurIndex<TextFrameList.List.count()-1) CurIndex++;
    } else if ((event->key()==Qt::Key_Left)||(event->key()==Qt::Key_Up)) {
        if (CurIndex>0) CurIndex--;
    } else QComboBox::keyReleaseEvent(event);
    if (aCurIndex!=CurIndex) {
        STOPMAJ=true;
        ((QTableWidget *)view())->setCurrentCell(CurIndex/NBRCOLUMN,CurIndex-(CurIndex/NBRCOLUMN)*NBRCOLUMN);
        setCurrentIndex(CurIndex/NBRCOLUMN);
        MakeIcons();
        STOPMAJ=false;
        emit itemSelectionHaveChanged();
    }
}

//========================================================================================================================

void cCTexteFrameComboBox::MakeIcons() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCTexteFrameComboBox::MakeIcons");
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    int CurIndex=CurrentRow*NBRCOLUMN+CurrentCol;
    if (CurIndex<TextFrameList.List.count()) setItemIcon(CurrentRow,QIcon(QPixmap().fromImage(TextFrameList.List[CurIndex].Icon)));
}

//========================================================================================================================

void cCTexteFrameComboBox::s_ItemSelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCTexteFrameComboBox::s_ItemSelectionChanged");
    STOPMAJ=true;
    setCurrentIndex(((QTableWidget *)view())->currentRow());
    MakeIcons();
    emit currentIndexChanged(((QTableWidget *)view())->currentRow()*NBRCOLUMN+((QTableWidget *)view())->currentColumn());
    STOPMAJ=false;
}

//========================================================================================================================

void cCTexteFrameComboBox::SetCurrentTextFrame(QString BackgroundName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCBrushComboBox::SetCurrentBrush");
    if (STOPMAJ) return;
    CurrentSel=TextFrameList.SearchImage(BackgroundName);
    ((QTableWidget *)view())->setCurrentCell(CurrentSel/NBRCOLUMN,CurrentSel-(CurrentSel/NBRCOLUMN)*NBRCOLUMN);
    setCurrentIndex(CurrentSel/NBRCOLUMN);
    MakeIcons();
}

//========================================================================================================================

QString cCTexteFrameComboBox::GetCurrentTextFrame() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCBrushComboBox::GetCurrentTextFrame");
    CurrentSel=((QTableWidget *)view())->currentRow()*NBRCOLUMN+((QTableWidget *)view())->currentColumn();
    if (CurrentSel>=TextFrameList.List.count()) CurrentSel=TextFrameList.List.count()-1;
    return TextFrameList.List[CurrentSel].RessourceName;
}

//************************************************************************************************************************
// cPopupTexteFrame
//************************************************************************************************************************

cPopupTexteFrame::cPopupTexteFrame(QWidget *parent):QDialog(parent,Qt::FramelessWindowHint|Qt::Popup/*Qt::Tool|Qt::Window|Qt::FramelessWindowHint*/) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cPopupTexteFrame::cPopupTexteFrame");
    Table=NULL;
    CurrentSel=-1;
}

//========================================================================================================================

void cPopupTexteFrame::DoInitPopup(QPoint Pos) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cPopupTexteFrame::showEvent");
    setFocusPolicy(Qt::StrongFocus);
    move(Pos);

    Table=new QTableWidget(this);
    Table->setItemDelegate(&ItemDelegate);
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    Table->setSelectionMode(QAbstractItemView::SingleSelection);
    Table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    Table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    Table->setFocus();
    for (int i=0;i<NBRCOLUMN;i++) {
        Table->insertColumn(0);
        Table->setColumnWidth(0,ICONSIZE);
    }
    int NbrItem=TextFrameList.List.count();
    int NbrRow=NbrItem/NBRCOLUMN;
    if (NbrRow*NBRCOLUMN<NbrItem) NbrRow++;
    for (int i=0;i<NbrRow;i++) {
        Table->insertRow(Table->rowCount());
        Table->setRowHeight(Table->rowCount()-1,ICONSIZE);
    }
    int ScrollBarWidth=Table->verticalScrollBar()->width();
    Table->setFixedWidth(ICONSIZE*NBRCOLUMN+ScrollBarWidth);
    Table->setFixedHeight(ICONSIZE*4);    // 4 lines
    setFixedHeight(Table->height());
    setFixedWidth(Table->width());
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cPopupTexteFrame::s_ItemSelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cPopupTexteFrame::s_ItemSelectionChanged");
    CurrentSel=Table->currentRow()*NBRCOLUMN+Table->currentColumn();
    if (CurrentSel>=TextFrameList.List.count()) CurrentSel=-1;
    close();
}

//========================================================================================================================

QString cPopupTexteFrame::GetCurrentTextFrame() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cPopupTexteFrame::GetCurrentTextFrame");
    if ((CurrentSel<0)||(CurrentSel>=TextFrameList.List.count())) return "";
        else return TextFrameList.List[CurrentSel].RessourceName;
}
