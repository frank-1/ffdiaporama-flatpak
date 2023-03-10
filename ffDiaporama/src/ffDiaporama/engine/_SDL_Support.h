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

#ifndef _SDL_SUPPORT_H
#define _SDL_SUPPORT_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// SDL Library
extern "C" {
    #ifdef _MSC_VER
    #include <SDL.h>
    #else
    #include <SDL/SDL.h>
    #endif
}
// Include some common various class
#include "cSoundBlockList.h"

// Functions to manage SDL library
void SDLAudioCallback(void *,u_int8_t *stream,int len);
void SDLFirstInit(double WantedDuration,bool SDLAncMode,int64_t SamplingRate);
void SDLLastClose();
void SDLSetFPS(double WantedDuration,bool SDLAncMode,int64_t SamplingRate);
void SDLFlushBuffers();

// SDL Version of the
class cSDLSoundBlockList : public cSoundBlockList {
public:
    explicit cSDLSoundBlockList();

    virtual int16_t *DetachFirstPacket();
    virtual void    AppendPacket(int64_t Position,int16_t *PacketToAdd);
};

// SDL global define values
extern bool                SDLIsAudioOpen;          // true if SDL work at least one time
extern SDL_AudioSpec       AudioSpec;               // SDL param bloc
extern cSDLSoundBlockList  MixedMusic;              // Sound to play

#endif // _SDL_SUPPORT_H
