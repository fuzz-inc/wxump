/*
Copyright 2016 fuzz, Inc. All rights reserved. 
   http://www.fuzz.co.jp

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of fuzz, Inc. nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FUZZ, INC. BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/***********************************************************************//**
	@file
***************************************************************************/
#include <map>
#include "ump/ump.hpp"
/***********************************************************************//**
	@brief 
***************************************************************************/
#if defined(UMP_PLATFORM_MAC)
#define _FILE_OFFSET_BITS 64
#define __WXMAC__
#define __WXOSX__
#define __WXOSX_COCOA__
#endif
#include "wx/wx.h"
#include "wx/aui/framemanager.h"
#include "wx/cmdline.h"
#include "wx/fileconf.h"
#include "wx/rawbmp.h"
#include "wx/stdpaths.h"
#include "wx/tglbtn.h"
/***********************************************************************//**
	@brief 
***************************************************************************/
namespace wxump {
class Application;
class Client;
class Config;
class ControlPlayer;
class ControlWindow;
class Conversion;
class HaiImage;
class HaiObject;
class HaiRender;
class LayoutPos;
class LayoutRect;
class LayoutRenderer;
class LayoutSize;
class LayoutValue;
class LogWindow;
class MainFrame;
class Player;
class ResultWindow;
class Server;
class TakuInfo;
class TakuWindow;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
