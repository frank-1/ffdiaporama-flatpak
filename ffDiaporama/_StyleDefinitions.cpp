/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011 Dominique Levray <levray.dominique@bbox.fr>

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

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <windows.h>
#include <winbase.h>
#endif

#include "DlgManageStyle.h"

bool toAssending(const cStyleCollectionItem &Item1 ,const cStyleCollectionItem &Item2) {
    return Item1.StyleName.toUpper()<Item2.StyleName.toUpper();
}

//====================================================================================================================
// Style collections definition
//====================================================================================================================

cStyleCollectionItem::cStyleCollectionItem(bool IsGlobalConf,int IndexKey,QString TheStyleName,QString TheStyleDef) {
    FromGlobalConf  =IsGlobalConf;                          // true if device model is defined in global config file
    FromUserConf    =!IsGlobalConf;                         // true if device model is defined in user config file
    IsFind          =false;                                 // true if device model format is supported by installed version of ffmpeg
    StyleIndex      =IndexKey;                              // Style number index key
    StyleName       =TheStyleName;                          // Style name
    StyleDef        =TheStyleDef;                           // Style definition
}

//************************************************

cStyleCollectionItem::cStyleCollectionItem(cStyleCollectionItem *Item) {
    FromGlobalConf  =Item->FromGlobalConf;
    FromUserConf    =Item->FromUserConf;
    IsFind          =Item->IsFind;
    StyleIndex      =Item->StyleIndex;
    StyleName       =Item->StyleName;
    StyleDef        =Item->StyleDef;
    BckStyleName    =Item->BckStyleName;
    BckStyleDef     =Item->BckStyleDef;
}

//************************************************

cStyleCollectionItem::~cStyleCollectionItem() {

}

//************************************************

void cStyleCollectionItem::SaveToXML(QDomElement &domDocument,QString ElementName) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    Element.setAttribute("StyleIndex",      StyleIndex);
    Element.setAttribute("StyleName",       StyleName);
    Element.setAttribute("BckStyleName",    BckStyleName);
    Element.setAttribute("StyleDefinition", StyleDef);
    domDocument.appendChild(Element);
}

//************************************************

bool cStyleCollectionItem::LoadFromXML(QDomElement domDocument,QString ElementName,bool IsUserConfigFile,bool MustCheck) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        // Ensure BckStyleName is corresponding. Elsewhere return false
        if ((MustCheck)&&(Element.hasAttribute("BckStyleName"))&&((Element.attribute("BckStyleName")=="")||(Element.attribute("BckStyleName")!=BckStyleName))) return false;

        if (IsUserConfigFile) FromUserConf=true;
        StyleName=Element.attribute("StyleName");
        StyleDef =Element.attribute("StyleDefinition");

        if (FromUserConf==false) {
            BckStyleName=StyleName;
            BckStyleDef =StyleDef;
        }
        return true;
    } else return false;
}

//************************************************

QString cStyleCollectionItem::GetFilteredPart() {
    QString FilterPart="";
    QString Name=StyleName;
    for (int k=0;k<2;k++) {
        if (Name.startsWith("3:2-"))           { Name=Name.mid(QString("3:2-").length());    FilterPart=FilterPart+"3:2-";       }
        else if (Name.startsWith("2:3-"))      { Name=Name.mid(QString("2:3-").length());    FilterPart=FilterPart+"2:3-";       }
        else if (Name.startsWith("4:3-"))      { Name=Name.mid(QString("4:3-").length());    FilterPart=FilterPart+"4:3-";       }
        else if (Name.startsWith("3:4-"))      { Name=Name.mid(QString("3:4-").length());    FilterPart=FilterPart+"3:4-";       }
        else if (Name.startsWith("16:9-"))     { Name=Name.mid(QString("16:9-").length());   FilterPart=FilterPart+"16:9-";      }
        else if (Name.startsWith("9:16-"))     { Name=Name.mid(QString("9:16-").length());   FilterPart=FilterPart+"9:16-";      }
        else if (Name.startsWith("40:17-"))    { Name=Name.mid(QString("40:17-").length());  FilterPart=FilterPart+"40:17-";     }
        else if (Name.startsWith("2.35:1-"))   { Name=Name.mid(QString("2.35:1-").length()); FilterPart=FilterPart+"2.35:1-";    }
        else if (Name.startsWith("17:40-"))    { Name=Name.mid(QString("17:40-").length());  FilterPart=FilterPart+"17:40-";     }
    }
    return FilterPart;
}

//====================================================================================================================

cStyleCollection::cStyleCollection() {
    GeometryFilter  =false;
    SourceCollection=NULL;
}

//************************************************

cStyleCollection::~cStyleCollection() {
}

//************************************************

cStyleCollection *cStyleCollection::PrepUndo() {
    cStyleCollection *UndoCollection=new cStyleCollection();

    UndoCollection->SourceCollection=this;
    UndoCollection->CollectionName  =CollectionName;
    UndoCollection->GeometryFilter  =GeometryFilter;
    UndoCollection->ActiveFilter    =ActiveFilter;

    for (int i=0;i<Collection.count();i++) UndoCollection->Collection.append(new cStyleCollectionItem(Collection[i]));
    return UndoCollection;
}

//************************************************

void cStyleCollection::ApplyUndo(cStyleCollection *UndoCollection) {
    Collection.clear();
    for (int i=0;i<UndoCollection->Collection.count();i++) Collection.append(new cStyleCollectionItem(UndoCollection->Collection[i]));
}

//************************************************

void cStyleCollection::SortList() {
    qSort(Collection.begin(),Collection.end(),toAssending);
}

//************************************************

QString cStyleCollection::GetStyleName(QString StyleDef) {
    int i=0;
    if (GeometryFilter) while ((i<Collection.count())&&((Collection[i].StyleDef!=StyleDef)||(!Collection[i].StyleName.startsWith(ActiveFilter)))) i++;
        else            while ((i<Collection.count())&&(Collection[i].StyleDef!=StyleDef)) i++;

    if ((i<Collection.count())&&(Collection[i].StyleDef==StyleDef)) return Collection[i].StyleName.mid(Collection[i].GetFilteredPart().length());
        else return QApplication::translate("DlgManageStyle","Custom style");
}

//************************************************

QString cStyleCollection::GetStyleDef(QString StyleName) {
    int i=0;
    while ((i<Collection.count())&&(Collection[i].StyleName!=ActiveFilter+StyleName)) i++;
    if ((i<Collection.count())&&(Collection[i].StyleName==ActiveFilter+StyleName)) return Collection[i].StyleDef; else return "";
}

//************************************************

void cStyleCollection::SetProjectGeometryFilter(int Geometry) {
    SetImageGeometryFilter(Geometry,-1);
}

//************************************************

void cStyleCollection::SetImageGeometryFilter(int ProjectGeometry,int ImageGeometry) {
    if (ProjectGeometry==-1) {
        GeometryFilter=false;
        ActiveFilter  ="";
    } else {
        switch (ProjectGeometry) {
            case GEOMETRY_4_3:          ActiveFilter="4:3-";        break;
            case GEOMETRY_16_9:         ActiveFilter="16:9-";       break;
            case GEOMETRY_40_17:        ActiveFilter="2.35:1-";     break;
            default:                    ActiveFilter="";            break;
        }
        switch (ImageGeometry) {
            case IMAGE_GEOMETRY_3_2:    ActiveFilter=ActiveFilter+"3:2-";       break;   // Standard 3:2 landscape image
            case IMAGE_GEOMETRY_2_3:    ActiveFilter=ActiveFilter+"2:3-";       break;   // Standard 3:2 portrait image
            case IMAGE_GEOMETRY_4_3:    ActiveFilter=ActiveFilter+"4:3-";       break;   // Standard 4:3 landscape image
            case IMAGE_GEOMETRY_3_4:    ActiveFilter=ActiveFilter+"3:4-";       break;   // Standard 4:3 portrait image
            case IMAGE_GEOMETRY_16_9:   ActiveFilter=ActiveFilter+"16:9-";      break;   // Standard 16:9 landscape image
            case IMAGE_GEOMETRY_9_16:   ActiveFilter=ActiveFilter+"9:16-";      break;   // Standard 16:9 portrait image
            case IMAGE_GEOMETRY_40_17:  ActiveFilter=ActiveFilter+"40:17-";     break;   // Standard cinema landscape image
            case IMAGE_GEOMETRY_17_40:  ActiveFilter=ActiveFilter+"17:40-";     break;   // Standard cinema portrait image
        }
        GeometryFilter=true;
    }
}

//************************************************

void cStyleCollection::SaveToXML(QDomDocument &domDocument,QDomElement &root) {
    int         j=0;
    QDomElement Element=domDocument.createElement(CollectionName);

    for (int i=0;i<Collection.count();i++) if (Collection[i].FromUserConf) {
        Collection[i].SaveToXML(Element,QString(CollectionName+QString("Item_%1").arg(j)));
        j++;
    }
    if (j>0) root.appendChild(Element);
}

//************************************************

void cStyleCollection::LoadFromXML(QDomDocument &domDocument,QDomElement root,int TypeConfigFile) {
    if ((root.elementsByTagName(CollectionName).length()>0)&&(root.elementsByTagName(CollectionName).item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName(CollectionName).item(0).toElement();
        int i=0;
        while ((Element.elementsByTagName(CollectionName+QString("Item_%1").arg(i)).length()>0)&&(root.elementsByTagName(CollectionName+QString("Item_%1").arg(i)).item(0).isElement()==true)) {
            if (TypeConfigFile==GLOBALCONFIGFILE) {
                // Reading from global config file : append device
                Collection.append(cStyleCollectionItem(TypeConfigFile==GLOBALCONFIGFILE,i,"",""));
                Collection[i].LoadFromXML(Element,QString(CollectionName+QString("Item_%1").arg(i)),false,false);

                // Style name translation (Standard style only) - do it 2 times
                QString UnfilteredStyleName=Collection[i].StyleName;

                for (int k=0;k<2;k++) {
                    if (UnfilteredStyleName.startsWith("3:2-")) UnfilteredStyleName=UnfilteredStyleName.mid(QString("3:2-").length());
                    else if (UnfilteredStyleName.startsWith("2:3-")) UnfilteredStyleName=UnfilteredStyleName.mid(QString("2:3-").length());
                    else if (UnfilteredStyleName.startsWith("4:3-")) UnfilteredStyleName=UnfilteredStyleName.mid(QString("4:3-").length());
                    else if (UnfilteredStyleName.startsWith("3:4-")) UnfilteredStyleName=UnfilteredStyleName.mid(QString("3:4-").length());
                    else if (UnfilteredStyleName.startsWith("16:9-")) UnfilteredStyleName=UnfilteredStyleName.mid(QString("16:9-").length());
                    else if (UnfilteredStyleName.startsWith("9:16-")) UnfilteredStyleName=UnfilteredStyleName.mid(QString("9:16-").length());
                    else if (UnfilteredStyleName.startsWith("40:17-")) UnfilteredStyleName=UnfilteredStyleName.mid(QString("40:17-").length());
                    else if (UnfilteredStyleName.startsWith("2.35:1-")) UnfilteredStyleName=UnfilteredStyleName.mid(QString("2.35:1-").length());
                    else if (UnfilteredStyleName.startsWith("17:40-")) UnfilteredStyleName=UnfilteredStyleName.mid(QString("17:40-").length());
                }

                // work on full StyleName
                if (Collection[i].StyleName=="Big black text with white outlines")                      Collection[i].StyleName=QApplication::translate("DlgManageStyle","Big black text with white outlines");
                else if (Collection[i].StyleName=="Big light yellow text with dark brown shadow")       Collection[i].StyleName=QApplication::translate("DlgManageStyle","Big light yellow text with dark brown shadow");
                else if (Collection[i].StyleName=="Medium black text with white outlines")              Collection[i].StyleName=QApplication::translate("DlgManageStyle","Medium black text with white outlines");
                else if (Collection[i].StyleName=="Medium light yellow text with dark brown shadow")    Collection[i].StyleName=QApplication::translate("DlgManageStyle","Medium light yellow text with dark brown shadow");
                else if (Collection[i].StyleName=="Small white text with black outlines")               Collection[i].StyleName=QApplication::translate("DlgManageStyle","Small white text with black outlines");
                else if (Collection[i].StyleName=="Centered Blue Gradient")                             Collection[i].StyleName=QApplication::translate("DlgManageStyle","Centered Blue Gradient");
                else if (Collection[i].StyleName=="Centered Brown Gradient")                            Collection[i].StyleName=QApplication::translate("DlgManageStyle","Centered Brown Gradient");
                else if (Collection[i].StyleName=="Centered Dark-Gray Gradient")                        Collection[i].StyleName=QApplication::translate("DlgManageStyle","Centered Dark-Gray Gradient");
                else if (Collection[i].StyleName=="Centered Green Gradient")                            Collection[i].StyleName=QApplication::translate("DlgManageStyle","Centered Green Gradient");
                else if (Collection[i].StyleName=="Centered Light-Gray Gradient")                       Collection[i].StyleName=QApplication::translate("DlgManageStyle","Centered Light-Gray Gradient");
                else if (Collection[i].StyleName=="Centered Red Gradient")                              Collection[i].StyleName=QApplication::translate("DlgManageStyle","Centered Red Gradient");
                else if (Collection[i].StyleName=="Transparent block (no brush)")                       Collection[i].StyleName=QApplication::translate("DlgManageStyle","Transparent block (no brush)");
                else if (Collection[i].StyleName=="Rounded rectangle with small brown border")          Collection[i].StyleName=QApplication::translate("DlgManageStyle","Rounded rectangle with small brown border");
                else if (Collection[i].StyleName=="Rectangle with no effect")                           Collection[i].StyleName=QApplication::translate("DlgManageStyle","Rectangle with no effect");

                // work on Unfiltered StyleName part
                else if (UnfilteredStyleName=="Image geometry-Full image")                              Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Image geometry-Full image"));
                else if (UnfilteredStyleName=="Project geometry-Adjust on the width-Bottom")            Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Project geometry-Adjust on the width-Bottom"));
                else if (UnfilteredStyleName=="Project geometry-Adjust on the width-Middle")            Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Project geometry-Adjust on the width-Middle"));
                else if (UnfilteredStyleName=="Project geometry-Adjust on the width-Top")               Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Project geometry-Adjust on the width-Top"));
                else if (UnfilteredStyleName=="Project geometry-Adjust on the height-Left")             Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Project geometry-Adjust on the height-Left"));
                else if (UnfilteredStyleName=="Project geometry-Adjust on the height-Center")           Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Project geometry-Adjust on the height-Center"));
                else if (UnfilteredStyleName=="Project geometry-Adjust on the height-Right")            Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Project geometry-Adjust on the height-Right"));
                else if (UnfilteredStyleName=="Full screen")                                            Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Full screen"));
                else if (UnfilteredStyleName=="TV margins size")                                        Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","TV margins size"));
                else if (UnfilteredStyleName=="Maximum size for image geometry mode")                   Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Maximum size for image geometry mode"));
                else if (UnfilteredStyleName=="TV margins maximum size for image geometry mode")        Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","TV margins maximum size for image geometry mode"));
                else if (UnfilteredStyleName=="High half of the screen")                                Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","High half of the screen"));
                else if (UnfilteredStyleName=="Left half of the screen")                                Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Left half of the screen"));
                else if (UnfilteredStyleName=="Left high quarter of the screen")                        Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Left high quarter of the screen"));
                else if (UnfilteredStyleName=="Left low quarter of the screen")                         Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Left low quarter of the screen"));
                else if (UnfilteredStyleName=="Low half of the screen")                                 Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Low half of the screen"));
                else if (UnfilteredStyleName=="Right half of the screen")                               Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Right half of the screen"));
                else if (UnfilteredStyleName=="Right high quarter of the screen")                       Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Right high quarter of the screen"));
                else if (UnfilteredStyleName=="Right low quarter of the screen")                        Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","Right low quarter of the screen"));
                else if (UnfilteredStyleName=="TV margins")                                             Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","TV margins"));
                else if (UnfilteredStyleName=="50% screen size-Centered")                               Collection[i].StyleName.replace(UnfilteredStyleName,QApplication::translate("DlgManageStyle","50% screen size-Centered"));
            } else {
                // Reading from user config file : search if Style already exist, then load it else append a new one
                QString ElementName=QString(CollectionName+QString("Item_%1").arg(i));
                if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
                    QDomElement TheElement=domDocument.elementsByTagName(ElementName).item(0).toElement();
                    int IndexKey=TheElement.attribute("StyleIndex").toInt();
                    int j=0;
                    while ((j<Collection.count())&&(Collection[j].StyleIndex!=IndexKey)) j++;
                    bool NotAppend=(j<Collection.count())&& Collection[j].LoadFromXML(Element,QString(CollectionName+QString("Item_%1").arg(i)),true,true);
                    if (!NotAppend) {
                        j=Collection.count();
                        Collection.append(cStyleCollectionItem(false,j/*IndexKey*/,"",""));
                        Collection[j].LoadFromXML(Element,QString(CollectionName+QString("Item_%1").arg(i)),true,false);
                    }
                }
            }
            i++;
        }
    }
    SortList();
}

//************************************************
void cStyleCollection::FillCollectionCB(QComboBox *CB,QString ActualStyleName,bool AdditionnalFramingStyle) {
    CB->setUpdatesEnabled(false);
    CB->clear();
    if (AdditionnalFramingStyle) {
        CB->addItem(QIcon(ICON_FRAMING_WIDTH),QApplication::translate("DlgManageStyle","Adjust to image width"));
        CB->addItem(QIcon(ICON_FRAMING_HEIGHT),QApplication::translate("DlgManageStyle","Adjust to image height"));
        CB->addItem(QIcon(ICON_FRAMING_FULL),QApplication::translate("DlgManageStyle","Adjust to full image"));
        CB->addItem(QIcon(ICON_FRAMING_CUSTOM),QApplication::translate("DlgManageStyle","Custom"));
    }
    QString Item="";
    int     i;
    bool    IsFind=false;
    for (i=0;i<Collection.count();i++) if (((!GeometryFilter)&&(Collection[i].GetFilteredPart()==""))||((GeometryFilter&&(Collection[i].GetFilteredPart()==ActiveFilter)))) {
        Item=Collection[i].StyleName.mid(Collection[i].GetFilteredPart().length());
        CB->addItem(Collection[i].FromUserConf?QIcon(ICON_USERCONF):QIcon(ICON_GLOBALCONF),Item);
    }
    for (i=0;i<CB->count();i++) if (ActualStyleName==CB->itemText(i)) {
        CB->setCurrentIndex(i);
        IsFind=true;
    }
    if (!IsFind) {
        if (AdditionnalFramingStyle) CB->setCurrentIndex(3); else CB->setCurrentIndex(-1);
    }
    CB->view()->setFixedWidth(500);
    CB->setUpdatesEnabled(true);
}

//************************************************

QString cStyleCollection::PopupCollectionMenu(QWidget *ParentWindow,QString ActualStyleDef) {
    QString Item="";
    bool    IsStyleFound =false;
    QMenu   *ContextMenu =new QMenu(ParentWindow);
    QMenu   *UpdateMenu  =new QMenu(ParentWindow);
    QAction *ActionCreate=new QAction(QApplication::translate("DlgManageStyle","Create new style"),ParentWindow);
    QAction *ActionManage=new QAction(QApplication::translate("DlgManageStyle","Manage existing style"),ParentWindow);
    UpdateMenu->setTitle(QApplication::translate("DlgManageStyle","Update existing style"));

    for (int i=0;i<Collection.count();i++)
      if (((!GeometryFilter)&&(Collection[i].GetFilteredPart()==""))||
          (((GeometryFilter)&&(Collection[i].GetFilteredPart()==ActiveFilter)))
      ) {
        Item=Collection[i].StyleName.mid(Collection[i].GetFilteredPart().length());

        QAction *NormalAction=new QAction(Collection[i].FromUserConf?QIcon(ICON_USERCONF):QIcon(ICON_GLOBALCONF),Item,ParentWindow);
        NormalAction->setIconVisibleInMenu(true);
        if (Collection[i].StyleDef==ActualStyleDef) {
            NormalAction->setText("*"+NormalAction->text());
            IsStyleFound=true;
        }
        ContextMenu->addAction(NormalAction);
        QAction *UpdateAction=new QAction(Collection[i].FromUserConf?QIcon(ICON_USERCONF):QIcon(ICON_GLOBALCONF),Item,ParentWindow);
        UpdateAction->setIconVisibleInMenu(true);
        UpdateAction->setToolTip(QApplication::translate("DlgManageStyle","Update style"));
        UpdateMenu->addAction(UpdateAction);
    }

    ContextMenu->addSeparator();
    ContextMenu->addAction(ActionCreate);
    ContextMenu->addMenu(UpdateMenu);
    ContextMenu->addAction(ActionManage);
    if (IsStyleFound) {
        ActionCreate->setEnabled(false);
        UpdateMenu->setEnabled(false);
    }
    QAction *Ret=ContextMenu->exec(QCursor::pos());

    if (Ret!=NULL) {
        Item="";
        if (Ret==ActionCreate)                                                              CreateNewStyle(ParentWindow,ActualStyleDef);
        else if (Ret==ActionManage)                                                         ManageExistingStyle(ParentWindow);
        else if (Ret->toolTip()==QApplication::translate("DlgManageStyle","Update style"))  UpdateExistingStyle((GeometryFilter?ActiveFilter:"")+Ret->text(),ActualStyleDef);
        else Item=/*(GeometryFilter?ActiveFilter:"")+*/Ret->text();
    }
    delete ContextMenu;
    delete UpdateMenu;
    delete ActionCreate;
    delete ActionManage;
    return Item;
}

//************************************************

void cStyleCollection::UpdateExistingStyle(QString StyleName,QString ActualStyleDef) {
    int i=0;
    while ((i<Collection.count())&&(Collection[i].StyleName!=StyleName)) i++;
    if ((i<Collection.count())&&(Collection[i].StyleName==StyleName)) {
        Collection[i].StyleDef=ActualStyleDef;
        Collection[i].FromUserConf=true;
    }
}

//************************************************

void cStyleCollection::CreateNewStyle(QWidget *ParentWindow,QString ActualStyleDef) {
    bool    Ok,Continue=true;
    QString Text="";

    while (Continue) {
        Continue=false;
        Text=QInputDialog::getText(ParentWindow,QApplication::translate("DlgManageStyle","Create new style"),QApplication::translate("DlgManageStyle","Style name:"),QLineEdit::Normal,Text,&Ok);
        if (Ok && !Text.isEmpty()) {
            Text=(GeometryFilter?ActiveFilter:"")+Text;
            int i=0;
            while ((i<Collection.count())&&(Collection[i].StyleName!=Text)) i++;
            if ((i<Collection.count())&&(Collection[i].StyleName==Text)) {
                if (QMessageBox::question(ParentWindow,QApplication::translate("DlgManageStyle","Create new style"),
                                          QApplication::translate("DlgManageStyle","A style with this name already exist.\nDo you want to overwrite-it ?"),
                                          QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) Collection[i].StyleDef=ActualStyleDef;
                else Continue=true;
            } else {
                int StyleIndex=0;
                for (int i=0;i<Collection.count();i++) if (Collection[i].StyleIndex>StyleIndex) StyleIndex=Collection[i].StyleIndex;
                StyleIndex++;
                Collection.append(cStyleCollectionItem(false,StyleIndex,Text,ActualStyleDef));
            }
        }
    }
    SortList();
}

//************************************************

void cStyleCollection::ManageExistingStyle(QWidget *ParentWindow) {
    DlgManageStyle Dlg(this,ParentWindow);
    Dlg.exec();
}

//************************************************

void cStyleCollection::StringToStringList(QString Item,QStringList &List) {
    int i=0;
    while ((i<Collection.count())&&(Collection[i].StyleName!=ActiveFilter+Item)) i++;
    if (i<Collection.count()) StringDefToStringList(Collection[i].StyleDef,List);
}

void cStyleCollection::StringDefToStringList(QString String,QStringList &List) {
    while (String.contains("###")) {
        List.append(String.left(String.indexOf("###")));
        String=String.mid(String.indexOf("###")+QString("###").length());
    }
    if (!String.isEmpty()) List.append(String);
}

//************************************************

QString cStyleCollection::DecodeString(QString String) {
    if (!String.contains("###GLOBALSTYLE###:")) return String;
    int StyleIndex=String.mid(QString("###GLOBALSTYLE###:").length()).toInt();
    int i=0;
    while ((i<Collection.count())&&(Collection[i].StyleIndex!=StyleIndex)) i++;
    if ((i<Collection.count())&&(Collection[i].StyleIndex==StyleIndex)) return Collection[i].StyleName.mid(Collection[i].GetFilteredPart().length());
        else return "";
}

//************************************************

QString cStyleCollection::EncodeString(QComboBox *CB,int ProjectGeometry,int ImageGeometry) {
    QString CurStyleName=CB->currentText();
    SetImageGeometryFilter(ProjectGeometry,ImageGeometry);
    if (GeometryFilter) CurStyleName=ActiveFilter+CurStyleName;
    int i=0;
    while ((i<Collection.count())&&(Collection[i].StyleName!=CurStyleName)) i++;
    if ((i<Collection.count())&&(Collection[i].StyleName==CurStyleName)) {
        if (Collection[i].FromGlobalConf) return QString("###GLOBALSTYLE###:%1").arg(Collection[i].StyleIndex);
        else return Collection[i].StyleName;
    }
    return CurStyleName;
}
