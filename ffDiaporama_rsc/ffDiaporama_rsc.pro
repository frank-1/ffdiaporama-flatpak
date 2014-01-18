# ======================================================================
#  This file is part of ffDiaporama
#  ffDiaporama is a tools to make diaporama as video
#  Copyright (C) 2011-2014 Dominique Levray <domledom@laposte.net>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, write to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
# ======================================================================

isEmpty(PREFIX) {
    PREFIX = /usr
}

QT         -= core gui
QMAKE_STRIP = echo
APPFOLDER   = ffDiaporama
TARGET      = ffDiaporama_rsc
TEMPLATE    = lib
CONFIG     += staticlib

OTHER_FILES += readme.txt

#--------------------------------------------------------------
# INSTALLATION
#--------------------------------------------------------------

General.path        = $$PREFIX/share/$$APPFOLDER
General.files       = RSCBUILDVERSION.txt
INSTALLS            += General

background.path     = $$PREFIX/share/$$APPFOLDER/background
background.files    = background/*.*
INSTALLS 	    += background

clipart.path        = $$PREFIX/share/$$APPFOLDER/clipart
clipart.files       = clipart/*.*
ffdclipart.path     = $$PREFIX/share/$$APPFOLDER/clipart/ffDiaporama
ffdclipart.files    = clipart/ffDiaporama/*.*
INSTALLS 	    += ffdclipart clipart

model.path          = $$PREFIX/share/$$APPFOLDER/model
model.files         = model/*.*
model_thumb.path    = $$PREFIX/share/$$APPFOLDER/model/Thumbnails
model_thumb.files   = model/Thumbnails/*.*
model_tts.path      = $$PREFIX/share/$$APPFOLDER/model/Titles
model_tts.files     = model/Titles/*.*
model_tts43.path    = $$PREFIX/share/$$APPFOLDER/model/Titles/4_3
model_tts43.files   = model/Titles/4_3/*.*
model_tts169.path   = $$PREFIX/share/$$APPFOLDER/model/Titles/16_9
model_tts169.files  = model/Titles/16_9/*.*
model_tts4017.path  = $$PREFIX/share/$$APPFOLDER/model/Titles/40_17
model_tts4017.files = model/Titles/40_17/*.*
model_fat.path      = $$PREFIX/share/$$APPFOLDER/model/FanArts
model_fat.files     = model/FanArts/*.*
INSTALLS 	    += model_thumb model_tts model_tts43 model_tts169 model_tts4017 model_fat model

luma.path           = $$PREFIX/share/$$APPFOLDER/luma
luma.files          = luma/*.*
luma_Bar.path       = $$PREFIX/share/$$APPFOLDER/luma/Bar
luma_Bar.files      = luma/Bar/*.*
luma_Box.path       = $$PREFIX/share/$$APPFOLDER/luma/Box
luma_Box.files      = luma/Box/*.*
luma_Center.path    = $$PREFIX/share/$$APPFOLDER/luma/Center
luma_Center.files   = luma/Center/*.*
luma_Checker.path   = $$PREFIX/share/$$APPFOLDER/luma/Checker
luma_Checker.files  = luma/Checker/*.*
luma_Clock.path     = $$PREFIX/share/$$APPFOLDER/luma/Clock
luma_Clock.files    = luma/Clock/*.*
luma_Snake.path     = $$PREFIX/share/$$APPFOLDER/luma/Snake
luma_Snake.files    = luma/Snake/*.*
INSTALLS 	    += luma_Bar luma_Box luma_Center luma_Checker luma_Clock luma_Snake luma
